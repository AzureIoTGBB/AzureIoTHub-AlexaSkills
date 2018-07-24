//#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
//#include <WiFiUdp.h>
#include <PubSubClient.h>

WiFiClientSecure espClient;
PubSubClient client(espClient);

/* WiFi Settings */

const char* ssid      = "<SSID>";
const char* password  = "<Password for WIFI Access>";
const char* mqtt_server = "<IoT Hub URL>";
const char* deviceName  = "<IoT Device Name>";
const char* deviceSAS   = "<IoT SAS token>"; 
long rssi;
long lastMsg = 0;
int BuiltIn = 4;
String message="";


void setup() {

  pinMode(BuiltIn, OUTPUT);
  Serial.begin(115200);
  setup_wifi(); 
  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  rssi = WiFi.RSSI();
  Serial.print("Received Signal Strength Indicator (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void LEDon() {
  digitalWrite(BuiltIn, HIGH); 
}

void LEDoff() {
  digitalWrite(BuiltIn, LOW);  
}

void callback(char* topic, byte* payload, unsigned int length) {
  LEDon();
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]  ");
  
  for (int i = 0; i < length; i++) {
    message+=(char)payload[i];
    }
  Serial.println(message);
  Serial.print("--");
    if(message == "OFF"){
    digitalWrite(BuiltIn, LOW);
    }
  if(message == "ON") {
    digitalWrite(BuiltIn, HIGH);
    }
  message="";  
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String hubUserString=mqtt_server;
    hubUserString+="/";
    hubUserString+=deviceName;
    Serial.println(deviceSAS);
   
    //if (client.connect(deviceName, hubUserString.c_str(), deviceSAS)) {
    if (client.connect(deviceName, hubUserString.c_str(), deviceSAS)) {
      Serial.println("got here");
      String subscribestring="devices/";
      subscribestring+=deviceName;
      subscribestring+="/messages/devicebound/#";
      
      client.subscribe(subscribestring.c_str());
      Serial.println("MQTT subscribed successfully.");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 
}




