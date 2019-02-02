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

type msgf('a) = (format4('a, Format.formatter, unit, unit) => 'a) => unit;
type log('a) = msgf('a) => unit;

type event('a) = {
  ts: DateTime.t,
  level: Level.t,
  message: msgf('a),
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

type reporter = {report: 'a. event('a) => unit};

let pp_ts = (ppf, ts) => Format.fprintf(ppf, "%s", ts |> DateTime.toISO);
let pp_level = (ppf, (color, level)) => {
  let level_fmt =
    level
    |> Level.toString
    |> Js.String.toUpperCase
    |> Printf.sprintf("%-5s")
    |> (color ? Level.colorize(level) : (v => v));
  Format.fprintf(ppf, "[%s]", level_fmt);
};
let pp_namespace = (ppf, ns) => {
  switch (ns) {
  | Some(ns) => Format.fprintf(ppf, "[%s]", ns)
  | None => ()
  };
};

type formater = {format: 'a. (Format.formatter, event('a)) => unit};

let default_formatter = (~color=true, ()) => {
  let format = (ppf, evt) => {
    let {level, message, ts, namespace} = evt;
    message(fmt =>
      Format.fprintf(
        ppf,
        "@[%a@ %a@ %a@]@.@[" ^^ fmt ^^ "@]@.",
        pp_ts,
        ts,
        pp_level,
        (color, level),
        pp_namespace,
        namespace,
      )
    );
  };
  {format: format};
};

let make_reporter = (~level=Level.Debug, ~formatter=default_formatter(), ()) => {
  let report: type a. event(a) => unit =
    evt =>
      if (Level.compare(level, evt.level) >= 0) {
        formatter.format(Format.std_formatter, evt);
      };
  {report: report};
};

let nop_reporter = {report: _ => ()};

let format_reporter = (~level=Level.Debug, ~color=true, ()) => {
  make_reporter(~level, ~formatter=default_formatter(~color, ()), ());
};

let _reporter = ref(nop_reporter);
let setReporter = reporter => _reporter := reporter;
let report = event => _reporter^.report(event);

let msg: type a. event(a) => unit = event => report(event);

module type LoggerS = {let namespace: string;};

module type Logger = {
  let namespace: string;
  let trace: log('a);
  let debug: log('a);
  let info: log('a);
  let warn: log('a);
  let error: log('a);
};

module Make = (M: LoggerS) : Logger => {
  let namespace = M.namespace;

  let log = (~message, level) => {
    let evt = makeEvent(~message, ~namespace, level);
    msg(evt);
  };
  let trace = message => log(~message, Level.Trace);
  let debug = message => log(~message, Level.Debug);
  let info = message => log(~message, Level.Info);
  let warn = message => log(~message, Level.Warn);
  let error = message => log(~message, Level.Error);
};

module Derive = (P: LoggerS, M: LoggerS) : Logger => {
  include Make({
    let namespace = [P.namespace, M.namespace] |> String.concat(":");
  });
};