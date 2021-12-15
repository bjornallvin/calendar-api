const moment = require("moment");
const chalk = require("chalk");

exports.debug = (...args) => {
  let now = moment();
  args = [
    chalk.blue(now.format("YYYY-MM-DD HH:mm:ss")),
    chalk.green(": "),
  ].concat(args);
  console.log(args.join(""));
};

exports.pad = (num, size) => {
  var s = num + "";
  while (s.length < size) s = "0" + s;
  return s;
};

exports.handleApi = (req, res) => {
  const params = req.params[0].split("/");
  if (params.length == 0 || params[0] == "") {
    res.json({
      error: "No endpoint in request",
    });
  } else if (params[0] == "mail") {
    if (params.length == 1 || params[1] == "") {
      res.json({
        error: "No mail method given",
      });
    } else if (params[1] == "test") {
      mail.sendTestMail().then((info) => {
        res.json({
          message: "Test mail sent",
          result: "ok",
          info,
        });
      });
    } else {
      res.json({
        error: "Unknown mail method",
        endpoint: params[1],
      });
    }
  } else {
    res.json({
      error: "Unknown endpoint",
      endpoint: params[0],
    });
  }
};
