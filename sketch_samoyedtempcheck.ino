#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "DHTesp.h"
#define LED_BUILTIN  2
#define SDA 13 //Define SDA pins
#define SCL 14 //Define SCL pins

DHTesp dht; //Define the DHT object
int dhtPin = 13;//Define the dht pin


LiquidCrystal_I2C lcd(0x27,16,3);

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
  Wire.begin(SDA, SCL);
  lcd.init();
  lcd.backlight();
  dht.setup(dhtPin, DHTesp::DHT11);//Initialize the dht pin and dht object
  Serial.begin(115200); //Set the baud rate to 115200
  
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

 mqttClient.publish(topic, "Keeping track of room temperature for dog");
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
 flag:TempAndHumidity newValues = dht.getTempAndHumidity();//Get the Temperature and humidity
 if (dht.getStatus() != 0) { //Judge if the correct value is read
    goto flag; //If there is an error, go back to the flag and re-read the data
 }
 Serial.println("Temperature:" + String((newValues.temperature*1.8)+32) + " Humidity:" + String(newValues.humidity));

 lcd.setCursor(0,0);
 lcd.print("T:" + String((newValues.temperature*1.8)+32) + " H:" + String(newValues.humidity));
 lcd.setCursor(0,1);
 if((newValues.temperature*1.8)+32 >= 76){
  digitalWrite(LED_BUILTIN,HIGH);
  lcd.print("Room is too hot");
  mqttClient.publish(topic, "Temperature is too high for dog");
  mqttClient.subscribe(topic);
 }
 if((newValues.temperature*1.8)+32 < 76){
  digitalWrite(LED_BUILTIN,LOW);
  lcd.print("Room cool enough");
  mqttClient.publish(topic, "Temperature is fine for dog");
  mqttClient.subscribe(topic);
 }
 delay(1000);

}
