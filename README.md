<p align="center">
<img src="https://github.com/mxarc/issuetron-3000/blob/master/logo.png" width="500" title="Issuetron Logo">
</p>

# issuetron-3000 🚨

🚨 Turn on a light signal on the physical world when an issue is opened on one of your repos

This is my entry for the [dev.to GitHub Actions hackathon](https://dev.to/devteam/announcing-the-github-actions-hackathon-on-dev-3ljn)

## 📖 Índice

- [issuetron-3000 🚨](#issuetron-3000-)
  - [📖 Índice](#-índice)
  - [About this](#about-this)
  - [How to use](#how-to-use)
  - [Setting up Adafruit IO](#setting-up-adafruit-io)
  - [Setting up your IoT device](#setting-up-your-iot-device)
  - [License](#license)

## About this

This is a really simple action intended to connect the real world with IoT devices connected to Adafruit IO Cloud.

It allows repo mantainers to keep alerted about new issues on their repos, made for the heros of the Open-Source 💙

I wanted tro try GitHub actions and I thoguth the dev.to hackathon was the best motivation to do it.

## How to use

Example workflow file:

```yml
on:
  issues:
    types: [opened]

jobs:
  do_iot_thing:
    runs-on: ubuntu-latest
    name: A job to turn on a light when someone opens an issue
    steps:
      - name: Send activation to ESP8266 device
        uses: 'mxarc/issuetron-3000@1.0'
        id: activation
        with:
          time: '15'
          blink: true
          io_user: '${{ secrets.IO_USER }}'
          io_key: '${{ secrets.IO_KEY }}'
          io_feed: 'issuetron'
          send_context: true
      # Use the output from the `hello` step
      - name: Check if we succeeded to turn on the light
        run: echo "The result of the action is ${{ steps.activation.outputs.success }}"
```

## Setting up Adafruit IO

TODO

## Setting up your IoT device

TODO

## License

[MIT](LICENSE)
