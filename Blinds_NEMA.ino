#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#import "MainPageHtml.h"

const char* ssid = "MyWifi";  //"CGOffice"; //
const char* password = "3132143880";

ESP8266WebServer server(80);

const int led = 13;


String rotationInfo = "";
boolean Direction = true;


const int enablePin = D1; 
const int stepPin = D3; 
const int dirPin = D4; 


void setup(void) {
    
  pinMode(led, OUTPUT);

  pinMode(enablePin,OUTPUT); 
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);

  digitalWrite(enablePin,LOW);

  delay(3000);
  digitalWrite(led, 0);
  Serial.begin(115200);
  
  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  delay(3000);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) 
  {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/inline", []() {    server.send(200, "text/plain", "this works as well");  });
  server.on("/getMotorStatus", []() {    server.send(200, "text/plain", "Motor Status Result");  });
  server.on("/test", handleRoot);
  server.on("/paramtest", []() {    server.send(200, "text/plain", server.arg("PARAM1"));  });
  server.on("/SetMotorLocation", SetMotorTemp);
  server.on("/SetEnableMotor", SetEnableMotor);

  //server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");


}


void loop() {
  server.handleClient();
}

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}


void SetMotorTemp()
{
  String msg = "";
  Serial.println("starting");
  
  rotationInfo = "";
  if (server.hasArg("STEPS") && server.hasArg("CLOCK")) 
  {
    msg += "Params Found - 1 \n\n";
    int steps;
    int delayValue = 100;
    bool clockWise;
    steps = server.arg("STEPS").toInt();
    clockWise = server.arg("CLOCK") == "1";
    Direction = clockWise;    //!Direction;  

    if (server.hasArg("TIMEDELAY")){
      delayValue = server.arg("TIMEDELAY").toInt();
    }

    if(Direction == true){
      digitalWrite(dirPin,LOW);
    }
    else{
      digitalWrite(dirPin,HIGH);
    }
    
    msg += "Stage - 2 \n\n"; 
    msg += "Value A: " + String(clockWise) + " \n\n"; 
    msg += "Value B: " + String(steps) + " \n\n"; 
    msg += "Value D: " + String(Direction) + " \n\n"; 

    rotationInfo = rotationInfo + "Starting Rotation Set" + "\n";

    stepper(steps, delayValue);
    
    msg += rotationInfo; 
  }
  else
  {
    msg += " not all params are found : STEPS /CLOCK  \n\n";
  }
  Serial.println("finished");
  Serial.println(msg);
  server.send(200, "text/plain", msg);  
}

void SetEnableMotor()
{
  String msg = "";
  
  if(server.hasArg("ENABLEPIN")){
      msg += "Motor Param Found \n\n";
      bool enablePinBool = server.arg("ENABLEPIN") == "1";
      if(enablePinBool){
        digitalWrite(stepPin,HIGH); 
        server.send(200, "text/plain", "Pin Set HIGH");  
      }
      else{
        digitalWrite(stepPin,LOW); 
        server.send(200, "text/plain", "Pin Set LOW");  
      }
  }
  server.send(200, "text/plain", "Now Param Found: ENABLEPIN");    
  
}














void stepper(int xw, int delayValue) 
{
  for (int x = 0; x < xw; x++) 
  {
    digitalWrite(stepPin,HIGH); 
    delay(delayValue); 
    digitalWrite(stepPin,LOW); 
    delay(delayValue);
  }
}



void handleNotFound() 
{
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}
