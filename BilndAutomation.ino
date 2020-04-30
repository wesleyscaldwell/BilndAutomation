#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#import "MainPageHtml.h"
#include <EEPROM.h>

  const char* ssid = "CGOffice"; //"MyWifi";  //"CGOffice"; //
  const char* password = "3132143880";
  ESP8266WebServer server(80);

  //Properties
  int eepromStepsStart = 0;
  int eepromStepsLen = 5;
  int eepromLocationStart = 5;
  int eepromLocationLen = 2;
  int eepromOpenCloseStepsStart = 7;
  int eepromOpenCloseStepsLen = 5;

  const int led = 13;
  int stepsPerRotation = 4096;
  int currentLocation = 1;        //1 = Up , 2 = Middle, 3 = Down
  int OpenCloseSteps = 0;


  String rotationInfo = "";
  boolean Direction = true;
  int MotorType = 2;   // 1 = Nema , 2 = SmallMotor 284 something

#if(1 == 1)
  uint8_t ConfigPin1 = 5; //D1;
  uint8_t ConfigPin2 = 4; //D2;
  uint8_t ConfigPin3 = 0; //D3;
  uint8_t ConfigPin4 = 2; //D4;
#else
  uint8_t ConfigPin1 = D1;
  uint8_t ConfigPin2 = D2;
  uint8_t ConfigPin3 = D3;
  uint8_t ConfigPin4 = D4;
#endif
  

  const int enablePin = ConfigPin1; 
  const int stepPin = ConfigPin3; 
  const int dirPin = ConfigPin4; 

  
  uint8_t IN1 = ConfigPin1;
  uint8_t IN2 = ConfigPin2;
  uint8_t IN3 = ConfigPin3;
  uint8_t IN4 = ConfigPin4;
  int Step28MotorSteps = 0;

  

void setup(void) {
  EEPROM.begin(512);
  pinMode(led, OUTPUT);

  pinMode(ConfigPin1, OUTPUT);    
  pinMode(ConfigPin2, OUTPUT);
  pinMode(ConfigPin3, OUTPUT);
  pinMode(ConfigPin4, OUTPUT);
  
  digitalWrite(enablePin,LOW);

  delay(1000);
  digitalWrite(led, 0);
  Serial.begin(115200);

  //EepromSave(0, 10, "test");
  //EepromSave(10, 5, "n2-5");
  //EepromSave(20, 3, "testing");
  
  //Serial.println("done saving");
  //Serial.println(EepromGet(0, 10));
  //Serial.println(EepromGet(10, 5));
  //Serial.println(EepromGet(20, 3));
  //Serial.println("done get");
  Serial.println("----");
  Serial.println(stepsPerRotation);
  int valSteps = EepromGetInt(eepromStepsStart,eepromStepsLen);
  if(valSteps > 0) {  stepsPerRotation = valSteps; }

  int valLocation = EepromGetInt(eepromLocationStart,eepromLocationLen);
  if(valLocation > 0) {  currentLocation = valLocation; }

  int valOpenCloseSteps = EepromGetInt(eepromOpenCloseStepsStart,eepromOpenCloseStepsLen);
  if(valOpenCloseSteps > 0) {  OpenCloseSteps = valOpenCloseSteps; }

  //EepromSaveInt(30, 2, 12);
  //int val2 = EepromGetInt(30,2);
  
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
  server.on("/SetMotor", ProcessMotorAction);
  server.on("/SetStepsPerRotation", SetStepPerRotation);
  server.on("/SetOpenCloseSteps", SetStepToOpenAndClose);
  server.on("/SetEnableMotor", SetEnableMotor);
  server.on("/SetBlindOpen", SetBlindOpen);
  server.on("/SetBlindClosed", SetBlindClosed);

  server.on("/TurnOff", Step28TurnOffMotor);

  
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
  s.replace("%stepsPerRotation%", String(stepsPerRotation));
  s.replace("%currentLocation%", String(currentLocation));
  s.replace("%OpenCloseSteps%", String(OpenCloseSteps)); 

  server.send(200, "text/html", s); //Send web page
}


void ProcessMotorAction()
{
  String msg = "";
  Serial.println("starting");
  
  rotationInfo = "";
  if (server.hasArg("CLOCK") && (server.hasArg("STEPS") || server.hasArg("ROTATIONS"))) 
  {
    msg += "Params Found - 1 \n\n";
    int steps;
    int delayValue = 100;
    bool turnOffMotor = true;
    bool clockWise;

    if(server.hasArg("ROTATIONS"))
    {
      float rotations = server.arg("ROTATIONS").toFloat();
      steps = rotations * stepsPerRotation;
    } 
    else
    {
      steps = server.arg("STEPS").toInt();
    }
    
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

    
    
    msg += "Stage - 2 \n\n"; 
    rotationInfo = rotationInfo + "Starting Rotation Set" + "\n";

    ActionMotor(steps, delayValue, turnOffMotor);
    
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


void SetStepPerRotation()
{
  String msg = "";
  
  if(server.hasArg("STEPS")){
      msg += "Steps Param Found \n\n";
      stepsPerRotation = server.arg("STEPS").toInt();
      EepromSaveInt(eepromStepsStart, eepromStepsLen, stepsPerRotation);
      msg += "Steps Updated and saved";
      server.send(200, "text/plain", msg);  
  }
  server.send(200, "text/plain", "Now Param Found: STEPS");    
}

void SetStepToOpenAndClose()
{
  String msg = "";
  
  if(server.hasArg("STEPS")){
      msg += "Steps Param Found \n\n";
      OpenCloseSteps = server.arg("STEPS").toInt();
      EepromSaveInt(eepromOpenCloseStepsStart, eepromOpenCloseStepsLen, OpenCloseSteps);
      msg += "OpenCloseSteps Updated and saved";
      server.send(200, "text/plain", msg);  
  }
  server.send(200, "text/plain", "Now Param Found: STEPS");    
}

void SetBlindOpen()
{
  String msg = "";
  
  if(currentLocation == 1)
  {
      msg += "Opening Blinds  \n\n";
      
      Direction = true;
      ActionMotor(OpenCloseSteps, 1, true);
      currentLocation = 2;
      EepromSaveInt(eepromLocationStart, eepromLocationLen, currentLocation);
      
      msg += "Blinds Open";
      server.send(200, "text/plain", msg);  
  }
  server.send(200, "text/plain", "Now In a valid State");    
}

void SetBlindClosed()
{
  String msg = "";
  
  if(currentLocation == 2)
  {
      msg += "Closing Blinds  \n\n";

      Direction = false;
      ActionMotor(OpenCloseSteps, 1, true);
      
      currentLocation = 1;
      EepromSaveInt(eepromLocationStart, eepromLocationLen, currentLocation);

      
      msg += "Blinds Closed";
      server.send(200, "text/plain", msg);  
  }
  server.send(200, "text/plain", "Now In a valid State");    
}














void ActionMotor(int steps, int delayValue, bool turnOffMotor)
{
    if(MotorType == 1){
      ProcessNemaMotorAction(steps, delayValue);
    }
    else if(MotorType == 2){
      ProcessStep28MotorAction(steps, delayValue, turnOffMotor);
    }
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

String EepromGet(int loc, int locLen){
  
  int maxLen = (loc + locLen);
  char rtnValue[locLen];
  String rtnString = "";

  for (int i = 0; i < locLen; i++) 
  {     
    char newVal;
    rtnValue[i] = newVal;   
  }
  
  for (int i = loc; i < maxLen; i++) {
    int col = i - loc;
    rtnValue[col] = EEPROM.read(i);
    rtnString += rtnValue[col];
  }
  return rtnString;
}


void EepromSave(int loc, int ValueLength, String ValueString)
{
  for (int i = loc; i < (loc + ValueLength); i++) {
    char emptyValue = ' ';
    EEPROM.write(i, emptyValue);
  }
  for (int i = loc; i < (loc + ValueString.length()); i++) {
    int charLoc = (i - loc);
    char val = ValueString.charAt(charLoc);
    EEPROM.write(i, val);
  }
  EEPROM.commit();
}

void EepromSaveInt(int loc, int ValueLength, int ValueInt)
{
    EepromSave(loc, ValueLength, String(ValueInt));
}

int EepromGetInt(int loc, int locLen){
  String val = EepromGet(loc, locLen);
  if(is_number(val)){
    int i = val.toInt();
    return i;  
  }
  return 0;
}



bool is_number(String str){
   for(byte i=0;i<str.length();i++)
   {
      str.trim();
      if(isDigit(str.charAt(i)) == false) return false;
  }
   return true;
} 
