# ESP8266 - DHT22 sensor

Simple example program for ESP8266 Node MCU dev. kit and DHT22 sensor. ESP8266 periodically reads DHT22 for temperature and humidity and provides this data via HTTP server which is accessible via WiFi. Data on the web page are refreshed periodically by AJAX functions.

DHT22 connection: 
```
 DHT22       Node MCU
+------+    +-----+
| GND  |----| GND |
| NC   |    |     |
| DATA |----| D4  |
| VCC  |----| D2  |
+------+    +-----+
```



