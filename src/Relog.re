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

type reporter = {
  report: 'a 'b. (event('a, 'b), ~over: unit => unit, unit => 'b) => 'b,
};
let nop_reporter = {
  report: (_, ~over, k) => {
    over();
    k();
  },
};

let pp_header = (ppf, l) => Format.fprintf(ppf, "[DEBUG]");

let format_reporter = (~level=Level.Debug, ~app=Format.std_formatter, ()) => {
  let report = ({level: evtLevel, message, ts, namespace}, ~over, k) => {
    let k = _ => {
      over();
      k();
    };
    message @@
    (
      fmt => {
        let ppf = app;
        if (Level.compare(level, evtLevel) >= 0) {
          let timestamp = ts |> DateTime.toISO;
          let ns_fmt =
            Belt.Option.mapWithDefault(namespace, "", ns => "[" ++ ns ++ "]");
          let level_fmt =
            evtLevel
            |> Level.toString
            |> Js.String.toUpperCase
            |> Printf.sprintf("%-5s")
            |> Level.colorize(evtLevel);

          Format.kfprintf(
            k,
            ppf,
            "@[%s@ [%s]@ %s@]@.@[" ^^ fmt ^^ "@]",
            timestamp,
            level_fmt,
            ns_fmt,
          );
        } else {
          k();
        };
      }
    );
  };
  {report: report};
};

let report = (event, ~over, k) => format_reporter().report(event, ~over, k);

let over = () => ();
let kunit = _ => ();
let kmsg: type a b. (unit => b, event(a, b)) => b =
  (k, event) =>
    if (true) {
      report(event, ~over, k);
    } else {
      k();
    };

let now = () => DateTime.local();

let makeEvent = (~ts=now(), ~message, ~namespace=?, level) => {
  ts,
  level,
  message,
  namespace,
  context: Js.Dict.empty(),
};

let _reporter = ref(nop_reporter);

/* let consoleReporter = (level, evt) =>
   }; */

let setReporter = reporter => _reporter := reporter;

module type Logger = {
  let namespace: string;
  let trace: msgf('a, unit) => unit;
  let debug: msgf('a, unit) => unit;
  let info: msgf('a, unit) => unit;
  let warn: msgf('a, unit) => unit;
  let error: msgf('a, unit) => unit;
};

module MakeLogger = (M: {let namespace: string;}) : Logger => {
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

let () = {
  module TestL =
    MakeLogger({
      let namespace = "TestLogger";
    });
  TestL.debug(m => m("Hello %s", "world"));
};