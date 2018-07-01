#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
 
const char* ssid ="Redmi7";
const char* password ="Bi232";

const char* ssid1 ="HOME";
const char* password1 ="0000000000";

const char* mqttServer = "";
const int mqttPort = 8883;
const char* mqttUser = "";
const char* mqttPassword = "";
static bool ManulTrigger = true;

int *rest_flag;
#define RELAY_PIN 2
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long startmillis=0;
unsigned long currentmillis=0;
WiFiServer server(8080);

void setup() {

  Serial.begin(115200);
  startmillis =millis();
  // prepare GPIO2
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite ( 2, LOW );
   if(ManulTrigger){
      digitalWrite ( 2, HIGH );
  }else{
      digitalWrite ( 2, LOW );
  }
  
  //WiFi.mode(WIFI_STA);
  WiFi.mode(WIFI_AP_STA); 
  // WiFi.mode(WIFI_AP); 
  
  WiFi.softAP(ssid1, password1);
  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.write("HotSpt IP:");
  Serial.write(myIP);
  Serial.println(myIP);

  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.write("Connecting to WiFi..");
    Serial.write("\n");
     if(((millis()-startmillis)>6000)&&(!ManulTrigger))
     {
     ESP.restart();
     }
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  void callback(char* topic, byte* payload, unsigned int length);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.write("Connecting to MQTT...");
    Serial.write("\n");
    if((!client.connected()&&(millis()-startmillis)>6000)&&(!ManulTrigger))
     {
     ESP.restart();
     }
    if (client.connect("ESP8266Client")) {
      Serial.write("connected");  
      Serial.write("\n");
 
    } else {
 
      Serial.write("failed with state ");
      Serial.write("\n");
      Serial.write(client.state());
      Serial.write("\n");
      delay(2000);
 
    }
  }
 
  client.publish("topic", "Hello Gokul");
  client.subscribe("topic");
 
}
 
void callback(char* topic, byte* payload, unsigned int length) {
 char pay[length+1];
 
  Serial.write("Message arrived in topic: ");
     Serial.write("\n");
  Serial.write(topic);
     Serial.write("\n");
 
  Serial.write("Message:");
  Serial.write("\n");
  memset((char *)pay,0,sizeof(pay));
  for (int i = 0; i < length; i++) {
    Serial.write((char)payload[i]);
    Serial.write("\n");
    pay[i]=(char)payload[i];
  }
  Serial.println("-----------------------");
  if(strcmp(((char*)pay,"on")==0)&&(!ManulTrigger)){
      digitalWrite ( 2, HIGH );
      client.publish("teststatus", "ON");
  }
  else if(strcmp((char*)pay,"M1")==0){
        ManulTrigger =true;
  }
  else if(strcmp((char*)pay,"M0")==0){
        ManulTrigger =false;
  }
  else if((strcmp((char*)pay,"off")==0)&&(!ManulTrigger)){
     digitalWrite ( 2, LOW );   
     client.publish("teststatus", "OFF");
  }
  else if(strcmp((char*)pay,"rst")==0){
         ESP.restart();
  }
  else
  {
    Serial.write((char*)pay); 
    Serial.write("\n");
}
}
 
void loop() {
  client.loop();
 }


