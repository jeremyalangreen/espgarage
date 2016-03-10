#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>
#include "WiFiManager.h"
#include <ArduinoOTA.h>

ESP8266WebServer HTTP(80);

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void debug() {

}

void setup() {
  uint8_t MAC_addr[6];
  char MAC_string[18]="";
  
  WiFi.macAddress(MAC_addr);
  for (int i = 0; i < sizeof(MAC_addr); ++i){
    sprintf(MAC_string,"%s%02x",MAC_string,MAC_addr[i]);
  }
  
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting WiFi...");
  Serial.print("MAC address is: ");
  Serial.println(MAC_string);

  WiFiManager wifiManager;
  
  wifiManager.setAPCallback(configModeCallback);

  if (!wifiManager.autoConnect()) {
    Serial.println("Failed to connect to WiFi");
    ESP.reset();
    delay(1000);
  }
 
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  
  ArduinoOTA.begin();
  
  Serial.printf("Starting HTTP...\n");
  HTTP.on("/", HTTP_GET, [](){
      HTTP.send(200, "text/plain", "Hello World!");
      Serial.println("Handling http request for /");
  });
  HTTP.on("/ssdp/schema.xml", HTTP_GET, [](){
    SSDP.schema(HTTP.client());
    Serial.println("Handling http request for /ssdp/schema.xml");
  });
  HTTP.begin();

  Serial.printf("Starting SSDP...\n");
  SSDP.setSchemaURL("ssdp/schema.xml");
  SSDP.setDeviceType("urn:jeremygreen:device:espgarage:1");
  SSDP.setHTTPPort(80);
  SSDP.setName("ESP8266 Garage Door Opener");
  SSDP.setSerialNumber(ESP.getChipId());
  SSDP.setURL("/");
  SSDP.setModelName("ESPGarage");
  SSDP.setModelNumber("v1");
  SSDP.setModelURL("http://brownout.com");
  SSDP.setManufacturerURL("http://brownout.com");
  SSDP.setManufacturer("Jeremy Green");
  SSDP.begin();
 
}

void loop() {
  // put your main code here, to run repeatedly:

  ArduinoOTA.handle();
  HTTP.handleClient();
  delay(1);
}
