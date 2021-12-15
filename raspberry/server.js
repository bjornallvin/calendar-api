global.config = require("./config");
console.log({ aeroState });

const common = require("./common");
const arduino = require("./arduino");
const express = require("express");
const router = require("./routes");
const app = express();
const parser = require("body-parser");
const config = require("./config");

arduino.init();

app.use(parser.urlencoded({ extended: false }));
app.use(parser.json());
app.use("/", router);
app.listen(aeroState.port, function () {
  common.debug("listening on port " + config.port);
});

module.exports = app;