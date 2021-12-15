const express = require("express");
const router = express.Router();
const common = require("./common");

router.route("/api/*").get(common.handleApi);

module.exports = router;
