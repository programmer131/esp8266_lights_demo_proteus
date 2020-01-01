#include <ArduinoJson.h>
StaticJsonDocument<200> JSON_Packet;
const int buttonPinRed = 2;
const int buttonPinBlue = 3;
const int buttonPinGreen = 4;
const int buttonPinYello = 5;
const int LED_PIN_Red  = 6;
const int LED_PIN_Blue = 7;
const int LED_PIN_Green= 8;
const int LED_PIN_Yello= 9;
int red_led_state=0,blue_led_state=0,green_led_state=0,yello_led_state=0;
bool led_state_update_flag=false;
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
int debounceDelay=1000;
unsigned long lastDebounceTime = 0;

void led_state_update()
{
  digitalWrite(LED_PIN_Red,red_led_state);
  digitalWrite(LED_PIN_Blue,blue_led_state);
  digitalWrite(LED_PIN_Green,green_led_state);
  digitalWrite(LED_PIN_Yello,yello_led_state);
}
void transmit_led_state()
{
  if(!JSON_Packet.isNull()){
  serializeJson(JSON_Packet, Serial);Serial.println();
  }
  JSON_Packet.clear();
}
void check_buttons()
{
  if(digitalRead(buttonPinRed))
  {
    red_led_state=!red_led_state;
    JSON_Packet["red_led"]=red_led_state;
    led_state_update_flag=true;
  }
  if(digitalRead(buttonPinGreen))
  {
    green_led_state=!green_led_state;
    JSON_Packet["green_led"]=green_led_state;
    led_state_update_flag=true;
  }
  if(digitalRead(buttonPinBlue))
  {
    blue_led_state=!blue_led_state;
    JSON_Packet["blue_led"]=blue_led_state;
    led_state_update_flag=true;
  }
  if(digitalRead(buttonPinYello))
  {
    yello_led_state=!yello_led_state;
    JSON_Packet["yello_led"]=yello_led_state;
    led_state_update_flag=true;
  }
}

void setup() {
  Serial.begin(9600);
  inputString.reserve(200);
  pinMode(buttonPinRed, INPUT);
  pinMode(buttonPinBlue, INPUT);  
  pinMode(buttonPinGreen, INPUT);
  pinMode(buttonPinYello, INPUT);
  pinMode(LED_PIN_Red, OUTPUT);
  pinMode(LED_PIN_Blue, OUTPUT);
  pinMode(LED_PIN_Green, OUTPUT);
  pinMode(LED_PIN_Yello, OUTPUT);
  led_state_update_flag=true;
  JSON_Packet["blue_led"]=blue_led_state;
  JSON_Packet["red_led"]=red_led_state;
  JSON_Packet["green_led"]=green_led_state;
  JSON_Packet["yello_led"]=yello_led_state;
  
}

void loop() {
  if(led_state_update_flag)
  {
    led_state_update();
    transmit_led_state();
    led_state_update();
    led_state_update_flag=false;
    lastDebounceTime=millis();
  }
    
  if(stringComplete) {
    DeserializationError error = deserializeJson(JSON_Packet, inputString);
    inputString = "";
    stringComplete = false;
    if (!error) {
      if(JSON_Packet.containsKey("blue_led"))
        blue_led_state=JSON_Packet["blue_led"];
      if(JSON_Packet.containsKey("red_led"))
        red_led_state=JSON_Packet["red_led"];
      if(JSON_Packet.containsKey("green_led"))
        green_led_state=JSON_Packet["green_led"];
      if(JSON_Packet.containsKey("yello_led"))
        yello_led_state=JSON_Packet["yello_led"];
      //set flag to update LED GPIOs
      led_state_update_flag=true;
      
      JSON_Packet.clear();
      inputString = "";
      stringComplete = false;
    }
  }

  if ((millis() - lastDebounceTime) < debounceDelay) {}
  else
  check_buttons();

}


void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    if(inChar!=0x0A) 
      inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == 0x0D) {
      stringComplete = true;
    }
  }
}
