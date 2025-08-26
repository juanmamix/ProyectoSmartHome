#include "WiFi.h"
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

// Pines de Sensores
const int Pir_1 = 14; //D5

bool Pir_01_Sens = true;
bool last_Motion_1 = false; 
bool Motion_1 = false;
char motion_1 = 0;
long now = millis();
long lastMeasure = 0;

void setup_wifi() {
  delay(10);
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

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  
  if(topic=="NodeMCUScnd/Pir/STATE")
  {
    Serial.print("Changing last motion 1 to ");
    if(messageTemp == "1")
      {
        last_Motion_1 = true;
        Serial.print("1");
      }
    else
    if(messageTemp == "0")
      {
        last_Motion_1 = false;
        Serial.print("0");
      }
  }
  if(topic=="NodeMCUScnd/Pir")
  {        
    if(messageTemp == "no")
      {
        Pir_01_Sens = false;
      }
    else
    if(messageTemp == "yes")
      {
        Pir_01_Sens = true;
      }
    }

  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("NodeMCUScnd")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      
      
      client.subscribe("NodeMCUScnd/Pir");
      client.subscribe("NodeMCUScnd/Pir/STATE");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() 
{
  pinMode(Pir_1, INPUT);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //Serial.println("connected");
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("NodeMcu2Client");
  
  now = millis();
  // Publishes state to mantain connection everty 10 mins
  if (now - lastMeasure > 60000*10)
  {
    lastMeasure = now;
    client.publish("NodeMCUScnd", "im alive");
  }
  if(Pir_01_Sens)
  {
    Motion_1 = digitalRead(Pir_1);
    if (Motion_1 != last_Motion_1)
    {
      
      last_Motion_1 = Motion_1;
      if(Motion_1)
      {
        motion_1 = '1';
        client.publish("NodeMCUScnd/Pir/STATE","ye",true);
        Serial.println("motion detected");
      }
      else
      {
        motion_1 = '0';
        client.publish("NodeMCUScnd/Pir/STATE","ne",true);
        Serial.println("motion not detected");  
      }
      
    }

  delay(50);  
    
    }
}
