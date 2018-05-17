// Simple testing program for ESP8266 Node MCU dev. kit and DHT22 sensor
// ESP8266 periodically reads DHT22 for temperature and humidity and 
// provides this data via HTTP server which is accessible via WiFi.
// Data on the web page are refreshed periodically by AJAX functions.
// DHT22 connection: 
//
//    DHT22       Node MCU
//   +------+    +-----+
//   | GND  |----| GND |
//   | NC   |    |     |
//   | DATA |----| D4  |
//   | VCC  |----| D2  |
//   +------+    +-----+
//

#include "DHT.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#include "index.h"        // Our HTML webpage contents with javascripts

#define VCC_PIN 4         // Provide VCC on this pin
#define DHTPIN  2         // what digital pin we're connected to
#define DHTTYPE DHT22     // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);     // Initialize DHT sensor.
ESP8266WebServer server(80);  // Port 80

// Station access to the router
const char* g_SSID_STA = "UPC3225540";
const char* g_PSWD_STA = "BCJMCBJA";

// Soft. AP access
const char* g_SSID_AP = "TempHumidity";
const char* g_PSWD_AP = "onforever";

IPAddress g_STA_IP;       // IP obtained from router's DHCP
IPAddress g_AP_IP;        // Our SW AP IP
String    g_PUB_IP;       // Public IP of the router

float     g_H             = 0;     // Hummidity
float     g_T             = 0;     // Temperature
int       g_CH            = 10;    // Channel Wifi
long      g_LAST_MEAS     = 0;     // The last measurement
long      g_LAST_ACTIVITY = 0;     // The last activity on http
long      g_DUR_ACTIVITY  = 150;   // The last activity on http
long      g_LAST_IP       = 0;     // The last public IP check
const int c_DURATION_MEAS = 3000;  // The frequency of temperature measurement
const int c_PERIOD_IP     = 60000; // Check public IP period

//==============================================================
//                  FUNCTIONS
//==============================================================

// HTTP sever - handle requests
void handleRoot(){
  String s = MAIN_page;             // Read HTML contents
  server.send(200, "text/html", s); // Send web page
}

// HTTP sever - handle AJAX requests for values
void handleValues() {
  // LED ON when activity has been started
  digitalWrite(16, LOW);
  g_LAST_ACTIVITY = millis();

  // Update IP info
  g_STA_IP = WiFi.localIP();
  g_AP_IP  = WiFi.softAPIP();
  
  String values;
  values = String(g_T) + ";" + String(g_H) + ";" + 
           g_STA_IP.toString() + ";" + g_AP_IP.toString() + ";" + g_PUB_IP;
  server.send(200, "text/plane", values); // Send Temp value only to client ajax request
}
/*
// HTTP sever - receive channel value from the client
void handleChannel() {
 String channelValue = "1";
 String t_state = server.arg("ChannelValue"); //Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
 g_CH = t_state.toInt();
 WiFi.softAP(g_SSID_AP, g_PSWD_AP, g_CH, false);
 Serial.println(t_state);
 Serial.println(g_CH);
 String wifiChannelState = String(g_CH); //Feedback parameter
 
 server.send(200, "text/plane", wifiChannelState); //Send web page
}
*/

// DHT22 - loop measuring
void MeasLoop(long now){
  // Take a measurement at a fixed time
  if( now - g_LAST_MEAS > c_DURATION_MEAS ){ 
    //digitalWrite(16, LOW);
    float h = 0;
    float t = 0;
    
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    t = dht.readTemperature();
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      delay(1000);
      return;
    }

    g_H = h;
    g_T = t;

    Serial.print("Humidity: ");
    Serial.print(g_H);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(g_T);
    Serial.print(" *C ");
    Serial.print(" \n");
    
    g_LAST_MEAS = millis();  //Remember the last time measurement

    //digitalWrite(16, HIGH);
  }
}

// Get my external IP
String GetExternalIP()
{
  HTTPClient http;
  int httpCode = 0;
  String ip = "0.0.0.0";

  Serial.print("GetExternalIP(): Send HTTP request... ");
  http.begin("http://api.ipify.org/");
  httpCode = http.GET();
  Serial.print("Got HTTP response code: ");
  Serial.print(httpCode);

  if (httpCode > 0) {
    String payload = http.getString();
    ip = payload;
    Serial.println(" IP: " + payload);
  }

  http.end();
  return ip;
}

//==============================================================
//                  SETUP
//==============================================================
void setup(void) {
  Serial.begin(9600);
  Serial.println("DHTxx test!");
  dht.begin();
  
  // Run SW AP
  WiFi.softAP(g_SSID_AP, g_PSWD_AP/*, 10, false*/);

  Serial.println("");
  Serial.println("Soft AP IP address: " + WiFi.softAPIP().toString() );

  // Setup WIFI to connect to AP (router)
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(g_SSID_STA, g_PSWD_STA);
  Serial.println("");

  // Wait for WIFI connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  g_PUB_IP = GetExternalIP();

  // HTTP server - assign handlers
  server.on("/", handleRoot);
  server.on("/readTemp", handleValues);
  //server.on("/setChannel", handleChannel);
  server.begin();
  Serial.println("HTTP server started at ip " + WiFi.localIP().toString() );

  // Setup LED
  pinMode(16, OUTPUT);

  // Provide VCC to sensor
  pinMode(VCC_PIN, OUTPUT);
  digitalWrite(VCC_PIN, HIGH);
}


//==============================================================
//                  LOOP
//==============================================================
void loop() {
  long t = millis();
  long time_ip = millis();
  
  MeasLoop( t );
  
  // Server
  server.handleClient();

  // Turn activity LED off
  if ( millis() - g_LAST_ACTIVITY > g_DUR_ACTIVITY ) {
    digitalWrite(16, HIGH);
  }

  // Update public IP
  if ( millis() - g_LAST_IP > c_PERIOD_IP) {
    g_PUB_IP = GetExternalIP();
    g_LAST_IP = millis();
  }
 
}



