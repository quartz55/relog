let () = {
  module ExLogger =
    Relog.Make({
      let namespace = "Examples";
    });
  module L =
    Relog.Derive(
      ExLogger,
      {
        let namespace = "Let";
      },
    );
  Relog.setReporter(Relog.format_reporter(~level=Relog.Level.Info, ()));

  L.trace(m => m("Logging from inside a let context: %.3f", 1.2345));
  L.debug(m => m("Logging from inside a let context: %.3f", 1.2345));
  L.info(m => m("Logging from inside a let context: %.3f", 1.2345));
  L.warn(m => m("Logging from inside a let context: %.3f", 1.2345));
  L.error(m => m("Logging from inside a let context: %.3f", 1.2345));
};