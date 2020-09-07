/**
 * Add required imports
 */
const core = require('@actions/core');
const github = require('@actions/github');
const MQTT = require('async-mqtt');

// GitHub Action inputs
const io_user = core.getInput('io_user');
const io_key = core.getInput('io_key');
const blink = core.getInput('blink');
const time = core.getInput('time');

// Required env vars
const required_vars = ['IO_USER', 'IO_KEY', 'GITHUB_EVENT_PATH'];

required_vars.forEach((env) => {
  if (!process.env[env] || !process.env[env].length) {
    console.error(
      `Env var ${env} is not defined. Please check your repo secrets`
    );
    process.exit(1);
  }
});

const eventContent = fs.readFileSync(process.env.GITHUB_EVENT_PATH, 'utf8');

console.log(eventContent);
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
