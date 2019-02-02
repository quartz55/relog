module ExLogger =
  Relog.Make({
    let namespace = "Examples";
  });
module L =
  Relog.Derive(
    ExLogger,
    {
      let namespace = "Core";
    },
  );

Relog.setReporter(Relog.format_reporter(~level=Relog.Level.Debug, ()));

L.trace(m => m("Hello %s from TRACE", "world"));
L.debug(m => m("Hello %s from DEBUG", "world"));
L.info(m => m("Hello %s from INFO", "world"));
L.warn(m => m("Hello %s from WARN", "world"));
L.error(m => m("Hello %s from ERROR", "world"));

Relog.setReporter(
  Relog.format_reporter(~color=false, ~level=Relog.Level.Debug, ()),
);

L.trace(m => m("Hello %s from TRACE (no color)", "world"));
L.debug(m => m("Hello %s from DEBUG (no color)", "world"));
L.info(m => m("Hello %s from INFO (no color)", "world"));
L.warn(m => m("Hello %s from WARN (no color)", "world"));
L.error(m => m("Hello %s from ERROR (no color)", "world"));