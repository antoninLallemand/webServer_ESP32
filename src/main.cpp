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

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variable management ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

enum time {hour, minute, second};

uint8_t DeadLines[4][2] = {
  {10, 30},
  {14, 20},
  {18, 00},
  {21, 40}
};

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Server management ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

WebServer myServer(80); //http port : 80

struct User {
    const char *username;
    const char *password;
};

// Define multiple users with their usernames and passwords
User users[] = {
    {"admin", "sausage"},
    {"adrien", "jesuisadri1"},
    {"uncollegue", "cestuncollegue"}
};

//check user credentials
bool authenticateUsers() {
    for (size_t i = 0; i < sizeof(users) / sizeof(users[0]); ++i) {
        if (myServer.authenticate(users[i].username, users[i].password))
            return true;
    }
    return false;
}


uint8_t stringToUint8(String input){
  char inputChar[input.length()+1];
  strcpy(inputChar, input.c_str()); //convert string formated time to a char array
  /*return number as an integer function of the case*/
  switch(input.length()){
    case 0 :
      return 80;  //no changes
    break;
    case 1 : 
      return (inputChar[0]-0x30);
    case 2 :
      return (inputChar[0]-0x30)*10 + inputChar[1]-0x30;
    default :
      return 81;  //bad entry
    break;
  }
}

//function called when client reach the home root with 200 code (success)
void handleRoot(){

  if (!authenticateUsers())
      return myServer.requestAuthentication();

  String page = "<!DOCTYPE html>";
  //load page as a string
  page += "<html lang='fr'>";
  page += "    <head>";
  page += "        <title> Distribution </title>";
  page += "        <meta http-equiv='refresh' content='60' name='view port' content='width=device-width, initial-scale=1' charset='UTF-8'/>";
  page += "        <link rel='stylesheet' href='./styles.css'>";
  page += "    </head>";
  page += "    <body style='background-color: rgb(80, 78, 78); color: blanchedalmond; text-align: center;'>";
  page += "        <div class='title' style='background-color: cadetblue; font-family: Calibri; font-size: 50px; margin: 0; padding: 10px;'>";
  page += "            <h3>Distribution Times</h3>";
  page += "        </div>";
  page += "        <div class='main'>";
  page += "            <table style='margin: 0 auto; font-size: 30px; font-family: Calibri; border-collapse: separate; border-spacing: 50px 10px;'>";
  page += "               <theader>";
  page += "                 <tr>";
  page += "                   <th>First Deadline</th>";
  page += "                   <th>Second Deadline</th>";
  page += "                   <th>Third Deadline</th>";
  page += "                   <th>Fourth Deadline</th>";
  page += "                 </tr>";
  page += "               </theader>";
  page += "               <tbody>";
  page += "                 <tr>";
  page += "                   <td>";
  page += "                     <form method='get' action='/submitF'>";
  page += "                       <label for='inputNumber'>Hour :&nbsp;&nbsp;&nbsp;&nbsp;</label>";
  page += "                       <input type='number' id='inputFirstHour' name='inputFirstHour' style='width: 50px;'><br>";
  page += "                       <label for='inputNumber'>Minute :</label>";
  page += "                       <input type='number' id='inputFirstMinute' name='inputFirstMinute' style='width: 50px; margin-bottom: 10px;'><br>";
  page += "                       <input type='submit' value='Submit' style='font-size: 20px; background-color: cadetblue; border-radius: 5px; padding: 5%; color: blanchedalmond; margin-top: 3%;'>";
  page += "                     </form>";
  page += "                   </td>";
  page += "                   <td>";
  page += "                     <form method='get' action='/submitS'>";
  page += "                       <label for='inputNumber'>Hour :&nbsp;&nbsp;&nbsp;&nbsp;</label>";
  page += "                       <input type='number' id='inputSecondHour' name='inputSecondHour' style='width: 50px;'><br>";
  page += "                       <label for='inputNumber'>Minute :</label>";
  page += "                       <input type='number' id='inputSecondMinute' name='inputSecondMinute' style='width: 50px; margin-bottom: 10px;'><br>";
  page += "                       <input type='submit' value='Submit' style='font-size: 20px; background-color: cadetblue; border-radius: 5px; padding: 5%; color: blanchedalmond;'>";
  page += "                     </form>";
  page += "                   </td>";
  page += "                   <td>";
  page += "                     <form method='get' action='/submitT'>";
  page += "                       <label for='inputNumber'>Hour :&nbsp;&nbsp;&nbsp;&nbsp;</label>";
  page += "                       <input type='number' id='inputThirdHour' name='inputThirdHour' style='width: 50px;'><br>";
  page += "                       <label for='inputNumber'>Minute :</label>";
  page += "                       <input type='number' id='inputThirdMinute' name='inputThirdMinute' style='width: 50px; margin-bottom: 10px;'><br>";
  page += "                       <input type='submit' value='Submit' style='font-size: 20px; background-color: cadetblue; border-radius: 5px; padding: 5%; color: blanchedalmond;'>";
  page += "                     </form>";
  page += "                   </td>";
  page += "                   <td>";
  page += "                     <form method='get' action='/submitFo'>";
  page += "                       <label for='inputNumber'>Hour :&nbsp;&nbsp;&nbsp;&nbsp;</label>";
  page += "                       <input type='number' id='inputFourthHour' name='inputFourthHour' style='width: 50px;'><br>";
  page += "                       <label for='inputNumber'>Minute :</label>";
  page += "                       <input type='number' id='inputFourthMinute' name='inputFourthMinute' style='width: 50px; margin-bottom: 10px;'><br>";
  page += "                       <input type='submit' value='Submit' style='font-size: 20px; background-color: cadetblue; border-radius: 5px; padding: 5%; color: blanchedalmond;'>";
  page += "                     </form>";
  page += "                 </tr>";
  page += "                 <tr>";
  page += "                   </td>";
  page += "                   <td><p>Time : "; page += DeadLines[0][hour]; page += "h"; page += DeadLines[0][minute]; page += "</p></td>";
  page += "                   <td><p>Time : "; page += DeadLines[1][hour]; page += "h"; page += DeadLines[1][minute]; page += "</p></td>";
  page += "                   <td><p>Time : "; page += DeadLines[2][hour]; page += "h"; page += DeadLines[2][minute]; page += "</p></td>";
  page += "                   <td><p>Time : "; page += DeadLines[3][hour]; page += "h"; page += DeadLines[3][minute]; page += "</p></td>";
  page += "                 </tr>";
  page += "               </tbody>";
  page += "            </table>";
  page += "        </div>";
  page += "    </body>";
  page += "</html>";

  myServer.setContentLength(page.length()); //make faster the reloading after button press
  myServer.send(200, "text/html", page);
}

//function called when client calls a unkow root (404 not found)
void handleNotFound(){
  myServer.send(404, "text/plain", "404 : Not found !");
}

//reads values and fills deadlines array
void handleSubmitedValues(String arg, uint8_t line, uint8_t column){
  String inputString = myServer.arg(arg);
  Serial.print("Input number: ");
  Serial.println(inputString);
  uint8_t inputNumber = stringToUint8(inputString);
  if(column == hour){  //hour
    if(inputNumber<24){
        DeadLines[line][column] = inputNumber;
    }
    else{
      return;
    }
  }
  else{ //minute
    if(inputNumber<60){
        DeadLines[line][column] = inputNumber;
    }
    else{
      return;
    }
  }
}

//function called when client open root /submit (when submit button is pressed)
void handleSubmitF(){
  handleSubmitedValues("inputFirstHour", 0, hour);
  handleSubmitedValues("inputFirstMinute", 0, minute);
  myServer.sendHeader("Location", "/"); //come back to home root
  myServer.send(303); //displaced to another page
}

void handleSubmitS(){ 
  handleSubmitedValues("inputSecondHour", 1, hour);
  handleSubmitedValues("inputSecondMinute", 1, minute);
  myServer.sendHeader("Location", "/"); //come back to home root
  myServer.send(303); //displaced to another page
}

void handleSubmitT(){
  handleSubmitedValues("inputThirdHour", 2, hour);
  handleSubmitedValues("inputThirdMinute", 2, minute);
  myServer.sendHeader("Location", "/"); //come back to home root
  myServer.send(303); //displaced to another page
}

void handleSubmitFo(){
  handleSubmitedValues("inputFourthHour", 3, hour);
  handleSubmitedValues("inputFourthMinute", 3, minute);
  myServer.sendHeader("Location", "/"); //come back to home root
  myServer.send(303); //displaced to another page 
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Execution ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void setup() {

  Serial.begin(9600);

  connectToWiFi();

  //links roots and associated callback functions
  myServer.on("/", handleRoot);
  myServer.onNotFound(handleNotFound);
  myServer.on("/submitF", HTTP_GET, handleSubmitF);
  myServer.on("/submitS", HTTP_GET, handleSubmitS);
  myServer.on("/submitT", HTTP_GET, handleSubmitT);
  myServer.on("/submitFo", HTTP_GET, handleSubmitFo);
  //starting server
  myServer.begin();
  Serial.println("Active server !");

}

void loop() {
  //trys to reconnect when wifi connection is lost
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    connectToWiFi();
  }

  //manage every client requests
  myServer.handleClient();
}
