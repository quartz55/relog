type t;

module Opts = {
  [@bs.deriving abstract]
  type t = {
    [@bs.optional]
    zone: string,
    [@bs.optional]
    locale: string,
    [@bs.optional]
    outputCalendar: string,
    [@bs.optional]
    numberingSystem: string,
  };
  let make = t;
};

type opts = Opts.t;
let makeOpts = Opts.make;

[@bs.scope "DateTime"] [@bs.module "luxon"] external local: unit => t = "";

[@bs.scope "DateTime"] [@bs.module "luxon"] external utc: unit => t = "";

[@bs.scope "DateTime"] [@bs.module "luxon"] external fromISO: string => t = "";

[@bs.scope "DateTime"] [@bs.module "luxon"]
external fromMillis: float => t = "";
[@bs.scope "DateTime"] [@bs.module "luxon"]
external fromMillisOpts: (float, Js.t({..})) => t = "fromMillis";
[@bs.scope "DateTime"] [@bs.module "luxon"]
external fromSeconds: int => t = "";
[@bs.scope "DateTime"] [@bs.module "luxon"]
external fromFormat: (string, string) => t = "";

[@bs.get] external year: t => int = "";
[@bs.get] external month: t => int = "";
[@bs.get] external day: t => int = "";
[@bs.get] external hour: t => int = "";
[@bs.get] external minute: t => int = "";
[@bs.get] external second: t => int = "";
[@bs.get] external weekday: t => int = "";

[@bs.send.pipe: t] external toString: string = "";
[@bs.send.pipe: t] external toLocaleString: string = "";
[@bs.send.pipe: t] external toISO: string = "";
[@bs.send.pipe: t] external toISOOpts: Js.t({..}) => string = "toISO";
[@bs.send.pipe: t] external toISODate: string = "";
[@bs.send.pipe: t] external toISOWeekDate: string = "";
[@bs.send.pipe: t] external toISOTime: string = "";
[@bs.send.pipe: t] external toMillis: int = "";
[@bs.send.pipe: t] external toSeconds: int = "";
[@bs.send.pipe: t] external toFormat: string = "";
[@bs.send.pipe: t] external toSQLTime: string = "";