#include <WiFi.h>
#include <PubSubClient.h>
#define LED_BUILTIN  2

const char *ssid = "NotTemp"; 
const char *password = "Saul4231";  

const char *mqtt_broker = "broker.emqx.io";
const char *topic = "esp32/message";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;


WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("press 1 or 2 to turn on or off the light");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    }
  Serial.println("Connected to the WiFi network!");

 mqttClient.setServer(mqtt_broker, mqtt_port);
 mqttClient.setCallback(callback);
 while (!mqttClient.connected()) {
   String client_id = String(WiFi.macAddress());
 
     if (mqttClient.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("Connected to Public MQTT Broker");
     } else {
         Serial.print("Failed to connect with MQTT Broker");
         Serial.print(mqttClient.state());
         delay(2000);
     }
 }

 mqttClient.publish(topic, "Hello! I am ESP32");
 mqttClient.publish(topic, "The following is a run of turning the LED on and off using 1 and 2 inputs");
 mqttClient.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
 }
 Serial.println();
 Serial.println("************");
}

void loop() {
  
 mqttClient.loop();
 Serial.available() == 0;
 while (Serial.available() == 0) {
 }
 int output = Serial.parseInt();
 if(output == 1){
   digitalWrite(LED_BUILTIN,HIGH);
   mqttClient.publish(topic, "The LED was turned on");
   mqttClient.subscribe(topic);
  }
 if(output == 2){
   digitalWrite(LED_BUILTIN,HIGH);
   mqttClient.publish(topic, "The LED was turned on");
   mqttClient.subscribe(topic);
 }
 output = 0;
 Serial.available() == 0;
 while (Serial.available() == 0) {
 }
 output = Serial.parseInt();
 if(output == 1){
   digitalWrite(LED_BUILTIN,LOW);
   mqttClient.publish(topic, "The LED was turned off");
   mqttClient.subscribe(topic);
  }
 if(output == 2){
   digitalWrite(LED_BUILTIN,LOW);
   mqttClient.publish(topic, "The LED was turned off");
   mqttClient.subscribe(topic);
   Serial.println("Only input 1 or 2 to turn LED on and off");
 }
}
