const core = require("@actions/core");
const github = require("@actions/github");

const io_user = core.getInput("io_user");
const io_key = core.getInput("io_key");
const blink = core.getInput("blink");
const time = core.getInput("time");

const MQTT = require("async-mqtt");

const topic = `mxarc/feeds/issuetron/json`;

(async () => {
  const client = await MQTT.connectAsync("mqtt://io.adafruit.com", {
    port: 1883,
    username: io_user,
    password: io_key,
  });
  try {
    console.log("Publishing data...");
    await client.publish(topic, JSON.stringify({ blink, time }));
    await client.end();
    // This line doesn't run until the client has disconnected without error
    console.log("Published data!");
    console.log("Done");
    core.setOutput("success", "true");
  } catch (error) {
    // just a stub while I work on the device code
    core.setOutput("success", "false");
    // Do something about it!
    console.log(error.stack);
    process.exit();
  }
})();
