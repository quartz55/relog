module Level = {
  type t =
    | Trace
    | Debug
    | Info
    | Warn
    | Error;

  let verbosity =
    fun
    | Trace => 5
    | Debug => 4
    | Info => 3
    | Warn => 2
    | Error => 1;

  let compare = (lhs, rhs) =>
    Pervasives.compare(verbosity(lhs), verbosity(rhs));

  let fromVerbosity = v =>
    switch (v) {
    | _ when v > 4 => Some(Trace)
    | 4 => Some(Debug)
    | 3 => Some(Info)
    | 2 => Some(Warn)
    | 1 => Some(Error)
    | _ => None
    };

  let fromString =
    fun
    | "trace" => Some(Trace)
    | "debug" => Some(Debug)
    | "info" => Some(Info)
    | "warn" => Some(Warn)
    | "error" => Some(Error)
    | _ => None;

  let toString =
    fun
    | Trace => "trace"
    | Debug => "debug"
    | Info => "info"
    | Warn => "warn"
    | Error => "error";

  let colorize = (level, str) =>
    switch (level) {
    | Error => str |> Chalk.red
    | Warn => str |> Chalk.yellow
    | Info => str |> Chalk.green
    | Debug => str |> Chalk.blue
    | Trace => str |> Chalk.gray
    };
};

type msgf('a, 'b) = (format4('a, Format.formatter, unit, 'b) => 'a) => 'b;
type log('a) = msgf('a, unit) => unit;

type event('a, 'b) = {
  ts: DateTime.t,
  level: Level.t,
  message: msgf('a, 'b),
  namespace: option(string),
  context: Js.Dict.t(string),
};

let now = () => DateTime.local();

let makeEvent = (~ts=now(), ~message, ~namespace=?, level) => {
  ts,
  level,
  message,
  namespace,
  context: Js.Dict.empty(),
};

type reporter = {
  report: 'a 'b. (event('a, 'b), ~over: unit => unit, unit => 'b) => 'b,
};

let nop_reporter = {
  report: (_, ~over, k) => {
    over();
    k();
  },
};

let pp_ts = (ppf, ts) => Format.fprintf(ppf, "%s", ts |> DateTime.toISO);
let pp_level = (ppf, level) => {
  let level_fmt =
    level
    |> Level.toString
    |> Js.String.toUpperCase
    |> Printf.sprintf("%-5s")
    |> Level.colorize(level);
  Format.fprintf(ppf, "[%s]", level_fmt);
};
let pp_namespace = (ppf, ns) => {
  switch (ns) {
  | Some(ns) => Format.fprintf(ppf, "[%s]", ns)
  | None => ()
  };
};
let format_reporter = (~level=Level.Debug, ~out=Format.std_formatter, ()) => {
  let report = ({level: evtLevel, message, ts, namespace}, ~over, k) => {
    let k = _ => {
      over();
      k();
    };
    if (Level.compare(level, evtLevel) >= 0) {
      message(fmt =>
        Format.kfprintf(
          k,
          out,
          "@[%a@ %a@ %a@]@.@[" ^^ fmt ^^ "@]@.",
          pp_ts,
          ts,
          pp_level,
          evtLevel,
          pp_namespace,
          namespace,
        )
      );
    } else {
      k();
    };
  };
  {report: report};
};

let _reporter = ref(nop_reporter);
let setReporter = reporter => _reporter := reporter;
let report = (event, ~over, k) => _reporter^.report(event, ~over, k);

let over = () => ();
let kunit = _ => ();
let kmsg: type a b. (unit => b, event(a, b)) => b =
  (k, event) => report(event, ~over, k);

module type Logger = {
  let namespace: string;
  let trace: log('a);
  let debug: log('a);
  let info: log('a);
  let warn: log('a);
  let error: log('a);
};

module Make = (M: {let namespace: string;}) : Logger => {
  let namespace = M.namespace;

  let log = (~message, level) => {
    let evt = makeEvent(~message, ~namespace, level);
    kmsg(kunit, evt);
  };
  let trace = message => log(~message, Level.Trace);
  let debug = message => log(~message, Level.Debug);
  let info = message => log(~message, Level.Info);
  let warn = message => log(~message, Level.Warn);
  let error = message => log(~message, Level.Error);
};

/* let () = {
     setReporter(format_reporter());
     module TestL =
       Make({
         let namespace = "TestLogger";
       });
     TestL.trace(m => m("Hello %s", "world"));
     TestL.debug(m => m("Hello %s", "world"));
     TestL.info(m => m("Hello %s", "world"));
     TestL.warn(m => m("Hello %s", "world"));
     TestL.error(m => m("Hello %s", "world"));
   }; */