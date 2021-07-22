//#include <ESP8266WiFi.h>
#include <Wire.h>
#include <WiFi.h>
#include <ThingsBoard.h>
#include <Adafruit_ADS1015.h>


Adafruit_ADS1115 ads1115(0x48); // construct an ads1115 at address 0x49
float Voltage0 = 0.0;
float Voltage1 = 0.0;
float Voltage2 = 0.0;
float Voltage3 = 0.0;
float soil1 = 0.0;
float soil2 = 0.0;
float soil3 = 0.0;
float soil4 = 0.0;
float tempsoil1 = 0.0;
float tempsoil2 = 0.0;
float tempsoil3 = 0.0;
float tempsoil4 = 0.0;

#define WIFI_AP "XXXXXXXX"
#define WIFI_PASSWORD "XXXXXXXXXX"

#define TOKEN "XXXXXXXXX"

char thingsboardServer[] = "XXXXXXXXXX";

WiFiClient wifiClient;

ThingsBoard tb(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;
void setup() {
 Serial.begin(9600);
 ads1115.begin();  // Initialize ads1115
 Wire.begin();
 delay(500);

  InitWiFi();
  lastSend = 0;
}


void loop() {

  if ( !tb.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendData();
    lastSend = millis();
  }

  tb.loop();
}
void getAndSendData() 
{

int16_t adc0, adc1, adc2, adc3;
 
  adc0 = ads1115.readADC_SingleEnded(0);
  adc1 = ads1115.readADC_SingleEnded(1);
  adc2 = ads1115.readADC_SingleEnded(2);
  adc3 = ads1115.readADC_SingleEnded(3);
  float Voltage1 = (adc0 * 0.1875)/1000;
  float Voltage2 = (adc1 * 0.1875)/1000;
  float Voltage3 = (adc2 * 0.1875)/1000;
  float Voltage4 = (adc3 * 0.1875)/1000;
  
  soil1 =(Voltage1-2.1)/(-0.018);
  if (soil1 >= 100)
{
  soil1 = 100; // Soil sat
}
else
{
soil1 = abs(soil1);
}
 //Serial.print("Voltage ch1 (V) = "); Serial.println(Voltage1 );
 Serial.print("soil1 (%) = "); Serial.println(soil1);

 tempsoil1 =(Voltage2-0.5)/0.010;
 //Serial.print("Voltage ch2 (V) = "); Serial.println(Voltage2 );
 Serial.print("tempsoil1 (C) = "); Serial.println(tempsoil1);
 
 
 tb.sendTelemetryFloat("soil1 (%)", soil1);
 tb.sendTelemetryFloat("tempsoil1 (C)", tempsoil1);

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
