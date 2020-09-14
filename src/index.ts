/**
*  _                    _                   
  (_)___ ___ _   _  ___| |_ _ __ ___  _ __  
  | / __/ __| | | |/ _ \ __| '__/ _ \| '_ \ 
  | \__ \__ \ |_| |  __/ |_| | | (_) | | | |
  |_|___/___/\__,_|\___|\__|_|  \___/|_| |_|
*
* Repo url: https://github.com/mxarc/issuetron-3000
* author: @mxarc
*/

// Add required imports
import core from '@actions/core';
import MQTT from 'async-mqtt';
import fs from 'fs';
import { Payload } from './payload.interface';

// GitHub Action inputs
const io_user = core.getInput('io_user').trim();
const io_key = core.getInput('io_key').trim();
const io_feed = core.getInput('io_feed').trim();
const send_context = core.getInput('send_context') || true;
const blink: boolean = Boolean(core.getInput('blink')) || true;
const time: number = parseInt(core.getInput('time')) || 10;
const event_path = process.env.GITHUB_EVENT_PATH;

// Required env vars
const required_vars: Array<string> = [event_path, io_user, io_key, io_feed];
required_vars.forEach((val) => {
  if (!val || !val.length) {
    core.error(`Env var ${val} is not defined. Please check workflow inputs`);
    process.exit(1);
  }
});

const eventContent: string = fs.readFileSync(event_path, 'utf8');
const eventObj: GitHubEvent.RootObject = JSON.parse(eventContent);

const isValidJson = (val: any) => {
  return val instanceof Array || val instanceof Object ? true : false;
};

if (!isValidJson(eventObj)) {
  core.error('GitHub event path is not a valid JSON object');
  process.exit(1);
}

// check if event payload is available on local folder
// more info on https://developer.github.com/webhooks/event-payloads/#issues
if (!fs.existsSync(event_path)) {
  core.error('GitHub event info is not on available');
  process.exit(1);
}

// let the show begin
(async () => {
  // here we create a MQTT client to connect over Adafruit IO brooker
  const client: MQTT.AsyncClient = await MQTT.connectAsync(
    'mqtt://io.adafruit.com',
    {
      port: 1883,
      username: io_user,
      password: io_key,
    }
  );
  try {
    const topic = `${io_user}/feeds/${io_feed}/json`;
    core.info('Sending call to IoT device...');
    core.info('Issue info:');
    core.info(`Title: ${eventObj.issue.title}`);
    core.info(`User: ${eventObj.issue.user.login}`);
    // now let's make a call to save the world

    const payload: Payload = {
      blink,
      time,
      // conditionally expand object to include issue context if it's desired
      ...(send_context && {
        title: eventObj.issue.title,
        user: eventObj.issue.user.login,
      }),
    };

    await client.publish(topic, JSON.stringify(payload));
    // now let's finish the client connection
    await client.end();
    core.info('Message sent!');
    core.info('Done');
    core.setOutput('success', 'true');
  } catch (error) {
    // oops! something wrong must happened, please blame the developer of this action
    core.setOutput('success', 'false');
    core.setFailed(error.message);
    process.exit(1);
  }
})();
