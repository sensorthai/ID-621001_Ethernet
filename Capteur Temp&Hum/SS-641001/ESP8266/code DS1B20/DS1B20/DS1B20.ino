#include <OneWire.h>
#include <DallasTemperature.h>
#include "PubSubClient.h"
#include "ESP8266WiFi.h"


const char* ssid = "XXXXX";
const char* password = "XXXXX";

#define TOKEN "XXXXXXX" //Ambil dari device thingsboard di atas
char ThingsboardHost[] = "xxxxxx";

#define ONE_WIRE_BUS D6
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
;

WiFiClient wifiClient;
PubSubClient client(wifiClient);
int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup()
{
  lastSend = 0;
   Serial.begin(9600);
   sensors.begin();
   WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("connected to");
  Serial.println(ssid);
  client.setServer( ThingsboardHost, 1883 );
}

void loop()
{
if ( !client.connected() ) 
{
    reconnect();
}

if ( millis() - lastSend > 5000 ) { // kirim data suhu tiap 1 detik
    getAndSendTemperatureData();
    lastSend = millis();
  }
  
}

void getAndSendTemperatureData()
{
  Serial.println("Collecting temperature data.");

  sensors.requestTemperatures();
  Serial.print("Temperature: ");
  Serial.print(sensors.getTempCByIndex(0)); 
  Serial.println(" *C ");
 
  // Prepare a JSON payload string
  String payload = "{";
 payload += "\"temperature\":";
 payload += String(sensors.getTempCByIndex(0)); 
  payload += "}";

  char attributes[1000];
  payload.toCharArray( attributes, 1000 );
  client.publish( "v1/devices/me/telemetry",attributes);
  Serial.println( attributes );
   
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("Esp8266", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.println( " : retrying in 5 seconds]" );
      delay( 500 );
    }
  }
}
