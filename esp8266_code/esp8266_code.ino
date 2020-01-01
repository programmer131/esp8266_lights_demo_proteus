#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

const char* ssid = "Home2";
const char* password = "helloworld";
const char* mqtt_server = "mqtt.gbridge.io";

WiFiClientSecure espClient;
PubSubClient client(espClient);

StaticJsonDocument<200> JSON_Packet;
void serialEvent();
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
static const char *fingerprint PROGMEM = "9F B6 69 93 2B C3 E6 31 48 F7 BC 60 FB A8 79 FB DA FB 2A EA";

int red_led_state=0,blue_led_state=0,green_led_state=0,yello_led_state=0;

void callback(char* topic, byte* payload, unsigned int length) {
  // Switch on the LED if 1 was received 
  if(strcmp(topic,"gBridge/u4537/d15521/onoff")==0)//blue
  {  
    if ((char)payload[0] == '0'){
     client.publish("gBridge/u4537/d15521/onoff/set", "0");
     JSON_Packet["blue_led"]=0;
    }
    else{
     client.publish("gBridge/u4537/d15521/onoff/set", "1");
     JSON_Packet["blue_led"]=1;
    }
  }
  if(strcmp(topic,"gBridge/u4537/d15519/onoff")==0)//green
  {
      if ((char)payload[0] == '0'){
     client.publish("gBridge/u4537/d15519/onoff/set", "0");
     JSON_Packet["green_led"]=0;
    }
    else{
     client.publish("gBridge/u4537/d15519/onoff/set", "1");
     JSON_Packet["green_led"]=1;
    }
  }
  if(strcmp(topic,"gBridge/u4537/d15520/onoff")==0)//red
  {
      if ((char)payload[0] == '0'){
     client.publish("gBridge/u4537/d15520/onoff/set", "0");
     JSON_Packet["red_led"]=0;
    }
    else{
     client.publish("gBridge/u4537/d15520/onoff/set", "1");
     JSON_Packet["red_led"]=1;
    }   
  }
  if(strcmp(topic,"gBridge/u4537/d15522/onoff")==0)//yello
  {
     if ((char)payload[0] == '0'){
     client.publish("gBridge/u4537/d15522/onoff/set", "0");
     JSON_Packet["yello_led"]=0;
    }
    else{
     client.publish("gBridge/u4537/d15522/onoff/set", "1");
     JSON_Packet["yello_led"]=1;
    } 
  }
  serializeJson(JSON_Packet, Serial);
  Serial.println();
  JSON_Packet.clear();
  delay(10);//add some delay after each packet transmission to allow UNO to process. 
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),"gbridge-username","gbridgeMQTTpassword")) {
     // Serial.println("connected");
      client.subscribe("gBridge/u4537/d15522/onoff");
      client.subscribe("gBridge/u4537/d15519/onoff");
      client.subscribe("gBridge/u4537/d15520/onoff");
      client.subscribe("gBridge/u4537/d15521/onoff");
    } else {
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
 /* Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
*/
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  //  Serial.print(".");
  }

  randomSeed(micros());

  /*Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());*/
}


void setup() {
  Serial.begin(9600);
  setup_wifi();
  inputString.reserve(100);
  espClient.setFingerprint(fingerprint);
  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);
}

uint32_t x=0;

void loop() {
  serialEvent();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (stringComplete) {
    DeserializationError error = deserializeJson(JSON_Packet, inputString);
    if (!error){
      if(JSON_Packet.containsKey("blue_led"))
       {
        if(JSON_Packet["blue_led"]==0)
          client.publish("gBridge/u4537/d15521/onoff/set", "0");
        else
          client.publish("gBridge/u4537/d15521/onoff/set", "1");
       }
      if(JSON_Packet.containsKey("red_led"))
        {
        if(JSON_Packet["red_led"]==0)
          client.publish("gBridge/u4537/d15520/onoff/set", "0");
        else
          client.publish("gBridge/u4537/d15520/onoff/set", "1");
       }
      if(JSON_Packet.containsKey("green_led"))
        {
        if(JSON_Packet["green_led"]==0)
          client.publish("gBridge/u4537/d15519/onoff/set", "0");
        else
          client.publish("gBridge/u4537/d15519/onoff/set", "1");
       }
      if(JSON_Packet.containsKey("yello_led"))
      {
        if(JSON_Packet["yello_led"]==0)
          client.publish("gBridge/u4537/d15522/onoff/set", "0");
        else
          client.publish("gBridge/u4537/d15522/onoff/set", "1");
       }  
       JSON_Packet.clear();
    }
    //Serial.println(inputString);
    inputString = "";
    stringComplete = false;
  }  
}


void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
     // Serial.println("New String received");
    }
  }
}
