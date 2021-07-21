#include <WiFi.h>
#include <ThingsBoard.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"



Adafruit_SHT31 sht31 = Adafruit_SHT31();

#define WIFI_AP "XXXXXXX"
#define WIFI_PASSWORD "XXXXXXX"

#define TOKEN "temp"

char thingsboardServer[] = "XXXXXXXX";

WiFiClient wifiClient;

ThingsBoard tb(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("SHT31 test");
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
   while (1) delay(1);
  }

  Serial.print("Heater Enabled State: ");
  if (sht31.isHeaterEnabled())
    Serial.println("ENABLED");
  else
    Serial.println("DISABLED");
  delay(10);
  InitWiFi();
  lastSend = 0;
}

void loop()
{
  if ( !tb.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
    lastSend = millis();
  }

  tb.loop();
}

void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");

  // Reading temperature or humidity takes about 250 milliseconds!
   float t = sht31.readTemperature();
   float h = sht31.readHumidity();


  

  Serial.println("Sending data to ThingsBoard:");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");

  tb.sendTelemetryFloat("temperature", t);
  tb.sendTelemetryFloat("humidity", h);
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!tb.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    if ( tb.connect(thingsboardServer, TOKEN) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED]" );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
