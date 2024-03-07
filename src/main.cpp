#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Wifi management ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

const char *SSID = "Redmi 10C"; //Redmi 10C
const char *PWD = "ogre3187"; //ogre3187
unsigned long lastMillis = 0; //wifi retry variable

String get_wifi_status(int status){
  switch(status){
    case WL_IDLE_STATUS:
      return "WL_IDLE_STATUS";
    case WL_SCAN_COMPLETED:
      return "WL_SCAN_COMPLETED";
    case WL_NO_SSID_AVAIL:
      return "WL_NO_SSID_AVAIL";
    case WL_CONNECT_FAILED:
      return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST:
      return "WL_CONNECTION_LOST";
    case WL_CONNECTED:
      return "WL_CONNECTED";
    case WL_DISCONNECTED:
      return "WL_DISCONNECTED";
    default :
      return "";
    break;
  }
}

void connectToWiFi(){
  int status = WL_IDLE_STATUS;
  Serial.print("Connecting to WiFi : ");
  Serial.println(get_wifi_status(status));
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(SSID, PWD);
  Serial.println(SSID);
  lastMillis = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    status = WiFi.status();
    Serial.println(get_wifi_status(status));
    if (millis() - lastMillis > 10000)
      ESP.restart();
  }

  Serial.println("Connection success.");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());  
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ OnBoard Led management ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define LED 2
bool LEDState = 0;
char LEDStateText[2][15] = {"LED Eteinte","LED Allumée"};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Server management ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

WebServer myServer(80); //http port : 80

void handleRoot(){
  String page = "<!DOCTYPE html>";
  page += "<html lang='fr'>";
  page += "    <head>";
  page += "        <title> Serveur ESP32 </title>";
  page += "        <meta http-equiv='refresh' content='60' name='view port' content='width=device-width, initial-scale=1' charset='UTF-8'/>";
  page += "        <link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>";
  page += "    </head>";
  page += "    <body>";
  page += "        <div class='w3-card w3-blue w3-padding-small w3-jumbo w3-center'>";
  page += "            <p>ETAT LED : "; page += LEDStateText[LEDState]; page +="</p>";
  page += "        </div>";
  page += "        <div class='w3-bar'>";
  page += "            <a href='/on' class='w3-bar-item w3-button w3-green w3-border w3-jumbo' style='width : 50%;'>ON</a>";
  page += "            <a href='/off' class='w3-bar-item w3-button w3-red w3-border w3-jumbo' style='width : 50%;'>OFF</a>";
  page += "        </div>";
  page += "        <div class='w3-center w3-padding-16'>";
  page += "            <p>Serveur hébergé sur un ESP32</p>";
  page += "            <i>Premier test</i>";
  page += "        </div>";
  page += "    </body>";
  page += "</html>";

  myServer.setContentLength(page.length());
  myServer.send(200, "text/html", page);
}

void handleNotFound(){
  myServer.send(404, "text/plain", "404 : Not found !");
}

void handleOn(){
  digitalWrite(LED, 1);
  LEDState = 1;
  myServer.sendHeader("Location", "/");
  myServer.send(303);
}

void handleOff(){
  digitalWrite(LED, 0);
  LEDState = 0;
  myServer.sendHeader("Location", "/");
  myServer.send(303);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Execution ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void setup() {

  Serial.begin(9600);

  connectToWiFi();

  myServer.on("/", handleRoot);
  myServer.onNotFound(handleNotFound);
  myServer.on("/on", handleOn);
  myServer.on("/off", handleOff);
  myServer.begin();
  Serial.println("Active server !");

  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);

}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    connectToWiFi();
  }
  myServer.handleClient();
}