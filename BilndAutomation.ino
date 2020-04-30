#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#import "MainPageHtml.h"

  const char* ssid = "MyWifi";  //"CGOffice"; //
  const char* password = "3132143880";
  ESP8266WebServer server(80);

  const int led = 13;


  String rotationInfo = "";
  boolean Direction = true;
  int MotorType = 2;   // 1 = Nema , 2 = SmallMotor 284 something


  uint8_t ConfigPin1 = D1;
  uint8_t ConfigPin2 = D2;
  uint8_t ConfigPin3 = D3;
  uint8_t ConfigPin4 = D4;

  const int enablePin = ConfigPin1; 
  const int stepPin = ConfigPin3; 
  const int dirPin = ConfigPin4; 

  
  uint8_t IN1 = ConfigPin1;
  uint8_t IN2 = ConfigPin2;
  uint8_t IN3 = ConfigPin3;
  uint8_t IN4 = ConfigPin4;
  int Step28MotorSteps = 0;

void setup(void) {
    
  pinMode(led, OUTPUT);

  pinMode(ConfigPin1, OUTPUT);    
  pinMode(ConfigPin2, OUTPUT);
  pinMode(ConfigPin3, OUTPUT);
  pinMode(ConfigPin4, OUTPUT);
  
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
    bool turnOffMotor = true;
    bool clockWise;
    
    steps = server.arg("STEPS").toInt();
    clockWise = server.arg("CLOCK") == "1";
    Direction = clockWise;    //!Direction;  

    if (server.hasArg("TIMEDELAY")){
      delayValue = server.arg("TIMEDELAY").toInt();
    }

    if(server.hasArg("TURNOFFMOTOR")){
      msg += "Motor Param Found \n\n";
      turnOffMotor = server.arg("TURNOFFMOTOR") == "1";
    }

    msg += "Value A: " + String(clockWise) + " \n\n"; 
    msg += "Value B: " + String(steps) + " \n\n"; 
    msg += "Value D: " + String(Direction) + " \n\n"; 

    if(MotorType == 1){
      ProcessNemaMotorAction(steps, delayValue);
    }
    else if(MotorType == 2){
      ProcessStep28MotorAction(steps, delayValue, turnOffMotor);
    }
    
    msg += "Stage - 2 \n\n"; 
    rotationInfo = rotationInfo + "Starting Rotation Set" + "\n";

    
    
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


void ProcessNemaMotorAction(int steps, int delayValue)
{
  if(Direction == true){
    digitalWrite(dirPin,LOW);
  }
  else{
    digitalWrite(dirPin,HIGH);
  }
  NemaStepper(steps, delayValue);
}


void ProcessStep28MotorAction(int steps, int delayValue, bool turnOffMotor)
{

  Step28Stepper(steps, delayValue);

  if(turnOffMotor){
    rotationInfo = rotationInfo + "Done - Turning Off motors" + "\n";
    Step28TurnOffMotor();
  }
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














void NemaStepper(int xw, int delayValue) 
{
  for (int x = 0; x < xw; x++) 
  {
    digitalWrite(stepPin,HIGH); 
    delay(delayValue); 
    digitalWrite(stepPin,LOW); 
    delay(delayValue);
  }
}





void Step28TurnOffMotor()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void Step28Stepper(int xw, int delayValue) 
{
  for (int x = 0; x < xw; x++) 
  {
    switch (Step28MotorSteps) 
    {
      case 0:
        Serial.println("Step 0");
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
      case 1:
        Serial.println("Step 1");
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, HIGH);
        break;
      case 2:
        Serial.println("Step 2");
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
      case 3:
        Serial.println("Step 3");
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
      case 4:
        Serial.println("Step 4");
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 5:
        Serial.println("Step 5");
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 6:
        Serial.println("Step 6");
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 7:
        Serial.println("Step 7");
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
      default:
        Serial.println("Step Default");
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
    }

    delay(delayValue);
    if (Direction == 1) {
      Step28MotorSteps++;
    }
    if (Direction == 0) {
      Step28MotorSteps--;
    }
    if (Step28MotorSteps > 7) {
      Step28MotorSteps = 0;
    }
    if (Step28MotorSteps < 0) {
      Step28MotorSteps = 7;
    }
    
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
