let () = {
  Relog.setReporter(Relog.format_reporter(~level=Relog.Level.Info, ()));

  module LetLogger =
    Relog.Make({
      let namespace = "Examples:Let";
    });

  LetLogger.trace(m => m("Logging from inside a let context: %.3f", 1.2345));
  LetLogger.debug(m => m("Logging from inside a let context: %.3f", 1.2345));
  LetLogger.info(m => m("Logging from inside a let context: %.3f", 1.2345));
  LetLogger.warn(m => m("Logging from inside a let context: %.3f", 1.2345));
  LetLogger.error(m => m("Logging from inside a let context: %.3f", 1.2345));
};