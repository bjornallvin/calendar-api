const common = require("./common");

var mockInterval = null;
var mockData = null;

var arduino = null;

exports.init = () => {
  if (config.arduinoAvailable) {
    var SerialPort = require("serialport");
    const Readline = require("@serialport/parser-readline");
    const parser = new Readline();

    arduino = new SerialPort(
      config.arduinoPath,
      {
        baudRate: 9600,
        parser,
      },
      function (err) {
        if (err) {
          console.log("Arduino error: ", err.message);
        } else {
          console.log("Connected to Arduino at ", config.arduinoPath);
          setTimeout(() => {
            // sen config values to arduino
            //exports.sendStartupValuesToArduino(arduino);
          }, 2000);
        }
      }
    );

    arduino.pipe(parser);

    parser.on("data", function (data) {
      console.log("incoming data from arduino1: " + data);
      arduino_data = data.trim();
      extractArduinoData(arduino_data);
    });
  } else {
    console.log("No arduino available");
    arduino = {
      write: (s) => {},
    };
  }
};

/**
 * Send all values needed for startup to a specific arduino
 * Gets all values needed from the global 'config' object
 */
exports.sendStartupValuesToArduino = (arduino) => {
  /*this.sendValueToArduino(arduino, "CWE", "waterEmptyDistance", 2);
    this.sendValueToArduino(arduino, "CWF", "waterFullDistance", 2);
    this.sendValueToArduino(arduino, "CPH", "maxPressure", 3);
    this.sendValueToArduino(arduino, "CPL", "minPressure", 3);
    this.sendValueToArduino(arduino, "CSR", "mistingScheduleRunTime", 4);
    this.sendValueToArduino(arduino, "CSP", "mistingSchedulePauseTime", 4);
    this.sendValueToArduino(arduino, "SS", "arduinoSimulation");*/
  // if set to run on schedule from the start, send command to start schedule
  /*if (config.runState == "schedule") {
        arduino.write("<RG>");
    }*/
};

/**
 * Send a specific message & value to a specific arduino
 */
exports.sendBoardToArduino = (name, id, padding = 0) => {

  values = board.join('');
  console.log(values);
  arduino.write("<SB" + values + ">");
};

/**
 * Sends data to both arduinos over serial connection
 */
exports.sendToArduino = (data) => {
  console.log("sending to arduino:", data)
  arduino.write(data);
};

extractArduinoData = (data) => {
  let source = null;
  const arduinoData = {};

  //console.log(data);

  if (data.startsWith("[[") && data.endsWith("]]")) {
    source = "control";
    arduinoData.controlsLastUpdated = Date.now();
    //console.log("updating controls");
  } else if (data.startsWith("{{") && data.endsWith("}}")) {
    source = "sensors";
    arduinoData.sensorsLastUpdated = Date.now();
    //console.log("updating sensors");
  }
  config.values = {
    ...config.values,
    ...arduinoData,
  };

  if (source !== null) {
    const valueString = data.substring(2, data.length - 2);
    const valuePairs = valueString.split(";");
    for (i = 0; i < valuePairs.length; i++) {
      let valuePairArr = valuePairs[i].split(":");
      let data_name = valuePairArr[0];
      let data_value = valuePairArr[1];

      const state_update = {
        [data_name]: data_value,
        [data_name + "_last_set"]: Date.now(),
      };

      //console.log(valuePairs[i]);
      let prev_value = config.values ? config.values[data_name] : "";
      let prev_updated = config.values
        ? config.values[data_name + "_last_changed"]
        : "";

      // has data changed?
      if (prev_value != data_value) {
        state_update[data_name + "_last_changed"] = Date.now();

        //some updates should be written to database
        if (data_name == "PMP" || data_name == "MST" || data_name == "BP") {
          console.log("Adding datapoint for ", data_name);
          datastore.addDataPoint({
            name: data_name,
            value: data_value,
            date: state_update[data_name + "_last_changed"],
          });
        }
      }

      //console.log({ state_update });

      config.values = {
        ...config.values,
        ...state_update,
      };
    }
  }
};
