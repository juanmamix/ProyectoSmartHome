#include <WiFi.h>
#include <PubSubClient.h>

/*
const char* ssid = "Mechatronic Network";
const char* password = "m3ch4tr0n1c";
*/

const char* ssid = "Tenda_3B2DF8";
const char* password = "9KBHqiTr";

const char* mqtt_server = "192.168.1.115";


WiFiClient espClient;
PubSubClient client(espClient);


long now = millis();
long lastMeasure = 0;


// Pines
const byte Relay1 = 12;
const byte Relay2 = 13;

void setup_wifi() {
  delay(1000);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(String topic, byte* message, unsigned int length) 
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) 
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  if (topic == "ESP32CAM/Relay_1") 
  {
    Serial.print("Changing Relay 1 to ");
    if (messageTemp == "on") 
    {
      Serial.print("On");
      digitalWrite(Relay1, LOW);
    } 
    else 
    if (messageTemp == "off") 
    {
      Serial.print("Off");
      digitalWrite(Relay1, HIGH);
    }
  }
  else
  if (topic == "ESP32CAM/Relay_2") 
  {
    Serial.print("Changing Relay 2 to ");
    if (messageTemp == "on") 
    {
      Serial.print("On");
      digitalWrite(Relay2, LOW);
    } 
    else 
    if (messageTemp == "off") 
    {
      Serial.print("Off");
      digitalWrite(Relay2, HIGH);
    }
  }
}

void reconnect() {

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32CAM")) {
      Serial.println("connected");

      client.subscribe("ESP32CAM/Relay_1");
      client.subscribe("ESP32CAM/Relay_2");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
    delay(100);
  }
}

void setup() {

  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if (!client.loop())
    client.connect("ESP32Client");

  now = millis();
  // Publishes state to mantain the connection every 10 mins
  if (now - lastMeasure > 60000 * 10) {
    lastMeasure = now;
    client.publish("ESP32CAM/", "im alive");
  }
  delay(50);
}
