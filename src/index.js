/**
 * Add required imports
 */
const core = require("@actions/core");
const github = require("@actions/github");
const MQTT = require("async-mqtt");

// GitHub Action inputs
const io_user = core.getInput("io_user");
const io_key = core.getInput("io_key");
const blink = core.getInput("blink");
const time = core.getInput("time");

// Get issue context so we can display a message on a lcd screen
const issueContext = github.getOctokit(
  "https://github.com/mxarc/issuetron-3000"
);

console.log(issueContext.issues.list({ per_page: 1, sort: "created" }));
/*const topic = `mxarc/feeds/issuetron/json`;

(async () => {
  const client = await MQTT.connectAsync("mqtt://io.adafruit.com", {
    port: 1883,
    username: io_user,
    password: io_key,
  });
  try {
    core.info("Sending call to IoT device...");
    await client.publish(topic, JSON.stringify({ blink, time }));
    await client.end();
    core.info("Message sent!");
    core.info("Done");
    core.setOutput("success", "true");
  } catch (error) {
    core.setOutput("success", "false");
    core.info(error.stack);
    process.exit();
  }
})();*/
