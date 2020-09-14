#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// the led output pin
#define LED 5

// WiFi Settings
#define WLAN_SSID "..."
#define WLAN_PASS "hello123"

// Adafruit settings
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883 // use 8883 for SSL
#define AIO_USERNAME "<your_username>"
#define AIO_KEY "<password>"

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

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup()
{
  Serial.begin(9600);
  delay(10);
  pinMode(D1, OUTPUT);
  digitalWrite(D1, LOW);
  Serial.println(F("Issuetron starting...\n\n"));
  // Connect to WiFi access point
  Serial.print("Connecting to: ");
  Serial.println(WLAN_SSID);
  // Begin WiFi connection attempt
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  // Retry connection until WiFi is available
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // Setup MQTT subscription for issuetron feed.
  mqtt.subscribe(&issuetron);
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
      // Do the blink thingy
      for (int i = 0; i < 5; i++)
      {
        delay(120);
        digitalWrite(D1, HIGH);
        delay(1300);
        digitalWrite(D1, LOW);
      }
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