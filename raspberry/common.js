const moment = require("moment");
const chalk = require("chalk");
const arduino = require("./arduino")

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
  console.log(params)


  if (params.length == 0 || params[0] == "") {
    res.json({
      error: "No endpoint in request",
    });
  } else if (params[0] == "calendar") {
    
    const cmd = params[1];

    if (cmd == "set-slot") {
      const slot = parseInt(params[2]);
      const value = parseInt(params[3]);
      board[slot] = value;
      console.log(board)
      arduino.sendBoardToArduino()
    }
    
    if (cmd == "toggle-slot") {
      const slot = parseInt(params[2]);
      board[slot] = board[slot] ? 0 : 1;
      arduino.sendBoardToArduino()
    }

    
    if (cmd == "set-animation") {
      const value = params[2];
      arduino.sendToArduino("<SA"+ value + ">")
    }

    if (cmd == "all-on") {
      board = new Array(24).fill(1);
      arduino.sendBoardToArduino()
    }

    if (cmd == "all-off") {
      board = new Array(24).fill(0);
      arduino.sendBoardToArduino()
    }

    res.json({
      error: "No endpoint in request",
    });

  
  } else {
    res.json({
      error: "Unknown endpoint",
      endpoint: params[0],
    });
  }
};
