Relog.setReporter(Relog.format_reporter(~level=Relog.Level.Debug, ()));
module TestL =
  Relog.Make({
    let namespace = "Examples:Core";
  });

TestL.trace(m => m("Hello %s from TRACE", "world"));
TestL.debug(m => m("Hello %s from DEBUG", "world"));
TestL.info(m => m("Hello %s from INFO", "world"));
TestL.warn(m => m("Hello %s from WARN", "world"));
TestL.error(m => m("Hello %s from ERROR", "world"));