#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <config.h>

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/**
 * MQTT Feeds
*/
Adafruit_MQTT_Subscribe issuetron = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/issuetron");

// LCD declaration
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup()
{
  Serial.begin(9600);
  delay(10);
  pinMode(D3, OUTPUT);
  digitalWrite(D3, LOW);
  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");
  Serial.println(F("Issuetron starting...\n\n"));
  // Connect to WiFi access point
  lcd.clear();
  lcd.print("Connecting to");
  Serial.print("Connecting to: ");
  lcd.setCursor(0, 1);
  Serial.println(WLAN_SSID);
  lcd.print(WLAN_SSID);
  // Begin WiFi connection attempt
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  // Retry connection until WiFi is available
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  lcd.clear();
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected");
  Serial.println(WiFi.localIP());
  // Setup MQTT subscription for issuetron feed.
  mqtt.subscribe(&issuetron);
  delay(1000);
  lcd.clear();
  lcd.print("Listening...");
}

void loop()
{
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    // check if its the issuetron feed, you may add more feeds
    // following the code above and this block
    if (subscription == &issuetron)
    {
      Serial.print(F("Got Issuetron call: "));
      Serial.println((char *)issuetron.lastread);
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("Issue opened!");
      lcd.autoscroll();
      // https://arduinojson.org/v6/assistant/
      const size_t capacity = JSON_OBJECT_SIZE(5) + 140;
      // example json: {"blink":false,"time":15,"title":"No blink test #2","user":"mxarc", "repo": "issuetron-3000"}
      DynamicJsonDocument doc(capacity);
      // put lastread info into JSON string
      const char *json = (char *)issuetron.lastread;
      // call ArduinoJSON library
      deserializeJson(doc, json);
      // now we have access to the json keys and values
      bool blink = doc["blink"];        // true
      int time = doc["time"];           // 15
      const char *title = doc["title"]; // "Hello world 5.0"
      const char *user = doc["user"];   // "mxarc"
      const char *repo = doc["repo"];   // "issuetron-3000"
      if (title)
      {
        delay(1000);
        lcd.clear();
        lcd.print(user);
        lcd.autoscroll();
        lcd.setCursor(0, 1);
        lcd.print(title);
        lcd.autoscroll();
      }
      else
      {
        lcd.print("Beep! Boop!");
        lcd.setCursor(0, 1);
        lcd.print("New issue");
      }
      // check time not less than one
      if (time < 1)
      {
        time = 1;
      }
      // Do the blink thingy
      if (blink)
      {
        for (int i = 0; i < time; i++)
        {
          delay(120);
          digitalWrite(D3, HIGH);
          delay(1000);
          digitalWrite(D3, LOW);
        }
      }
      else
      {
        digitalWrite(D3, HIGH);
        delay(time * 1000);
        digitalWrite(D3, LOW);
      }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Listening...");
    }
  }
  // ping the server to keep the mqtt connection alive
  if (!mqtt.ping())
  {
    mqtt.disconnect();
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect()
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000); // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}
