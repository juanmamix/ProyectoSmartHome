#include <ESP8266WiFi.h>
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


// Pines Utilizados:
const int Relay_1 = 0;   //D3
const int Relay_2 = 14;  //D5
const int Relay_3 = 12;  //D6
const int Relay_4 = 2;   //D4
                         // Pines de Sensores        :
const int Pir_1 = 16;    //D0
const int Pir_2 = 5;     //D1
const int Pir_3 = 4;     //D2

// Variables globales:
char motion_1 = 0;
char motion_2 = 0;
char motion_3 = 0;
bool Motion_1 = false;
bool Motion_2 = false;
bool Motion_3 = false;
bool Started  = false;
bool last_Motion_1 = false;
bool last_Motion_2 = false;
bool last_Motion_3 = false;
bool Pir_01_Sens = true;
bool Pir_02_Sens = true;
bool Pir_03_Sens = true;
long now = millis();
long lastMeasure = 0;
int tempo1, tempo2, tempo3, tiempooff = 0;

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
  if (topic == "NodeMCUMain/Relay_1") {
    Serial.print("Changing Relay 1 to ");
    if (messageTemp == "on") {
      digitalWrite(Relay_1, LOW);
      Serial.print("On");
    } else if (messageTemp == "off") {
      digitalWrite(Relay_1, HIGH);
      Serial.print("Off");
    }
  }
  if (topic == "NodeMCUMain/Relay_2") {
    Serial.print("Changing Relay 2 to ");
    if (messageTemp == "on") {
      digitalWrite(Relay_2, LOW);
      Serial.print("On");
    } else if (messageTemp == "off") {
      digitalWrite(Relay_2, HIGH);
      Serial.print("Off");
    }
  }
  if (topic == "NodeMCUMain/Relay_3") {
    Serial.print("Changing Relay 3 to ");
    if (messageTemp == "on") {
      digitalWrite(Relay_3, LOW);
      Serial.print("On");
    } else if (messageTemp == "off") {
      digitalWrite(Relay_3, HIGH);
      Serial.print("Off");
    }
  }
  if (topic == "NodeMCUMain/Relay_4") {
    Serial.print("Changing Relay 4 to ");
    if (messageTemp == "on") {
      digitalWrite(Relay_4, LOW);
      Serial.print("On");
    } else if (messageTemp == "off") {
      digitalWrite(Relay_4, HIGH);
      Serial.print("Off");
    }
  }
  if (topic == "NodeMCUMain/Pir_1") {
    Serial.print("Changing last motion 1 to ");
    if (messageTemp == "no") {
      Pir_01_Sens = false;
    } else if (messageTemp == "yes") {
      Pir_01_Sens = true;
    }
  }
  if (topic == "NodeMCUMain/Pir_2") {
    Serial.print("Changing last motion 2 to ");
    if (messageTemp == "no") {
      Pir_02_Sens = false;
    } else if (messageTemp == "yes") {
      Pir_02_Sens = true;
    }
  }
  if (topic == "NodeMCUMain/Pir_3") {
    Serial.print("Changing last motion 3 to ");
    if (messageTemp == "no") {
      Pir_03_Sens = false;
    } else if (messageTemp == "yes") {
      Pir_03_Sens = true;
    }
  }
  if (topic == "NodeMCUMain/Pir_1/STATE") {
    if (messageTemp == "1") {
      last_Motion_1 = true;
      Serial.print("1");
    } else if (messageTemp == "0") {
      last_Motion_1 = false;
      Serial.print("0");
    }
  }
  if (topic == "NodeMCUMain/Pir_2/STATE") {
    if (messageTemp == "1") {
      last_Motion_2 = true;
      Serial.print("1");
    } else if (messageTemp == "0") {
      last_Motion_2 = false;
      Serial.print("0");
    }
  }
  if (topic == "NodeMCUMain/Pir_3/STATE") {
    if (messageTemp == "1") {
      last_Motion_3 = true;
      Serial.print("1");
    } else if (messageTemp == "0") {
      last_Motion_3 = false;
      Serial.print("0");
    }
  }
}

void Automatico() {
  if (Pir_01_Sens) {
    if (digitalRead(Pir_1)) {
      tempo1 = 0;
      digitalWrite(Relay_1, false);
      digitalWrite(Relay_2, false);
    }
    if (tempo1 >= 3 * 12 or ((digitalRead(Pir_2) or digitalRead(Pir_3)) and (tempo1 >= 1 * 12))) {
      digitalWrite(Relay_1, true);
      digitalWrite(Relay_2, true);
      tempo1 = 0;
    }
    Serial.println(tempo1);
    tempo1++;
  }
  if (Pir_02_Sens) {
    if (digitalRead(Pir_2)) {
      tempo2 = 0;
      digitalWrite(Relay_4, false);
    }
    if (tempo2 >= 1 * 12) {
      digitalWrite(Relay_4, true);
      tempo2 = 0;
    }
    tempo2++;
  }
  if (Pir_03_Sens) {
    if (digitalRead(Pir_3)) {
      tempo3 = 0;
      digitalWrite(Relay_3, false);
    }
    if (tempo3 >=  12 * 3 or ((digitalRead(Pir_1) or digitalRead(Pir_2)) and (tempo3 >= 1 * 12))) {
      digitalWrite(Relay_3, true);
      tempo3 = 0;
    }
    tempo3++;
  }
}

void reconnect() {
  //offline();
  // Loop until we're reconnected
  if (!client.connected()) {


    if (client.connect("NodeMCUMain")) {
      Serial.println("connected");
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)

      client.subscribe("NodeMCUMain/Relay_1");
      client.subscribe("NodeMCUMain/Relay_2");
      client.subscribe("NodeMCUMain/Relay_3");
      client.subscribe("NodeMCUMain/Relay_4");
      client.subscribe("NodeMCUMain/Pir_1");
      client.subscribe("NodeMCUMain/Pir_1/STATE");
      client.subscribe("NodeMCUMain/Pir_2");
      client.subscribe("NodeMCUMain/Pir_2/STATE");
      client.subscribe("NodeMCUMain/Pir_3");
      client.subscribe("NodeMCUMain/Pir_3/STATE");
    } else {
      if (tiempooff >= 2) {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        tiempooff = 0;
        Serial.println("Attempting MQTT connection...");
      }
      tiempooff++;
      //delay(50);
    }
  }
   
}


void setup() {
  pinMode(Pir_1, INPUT);
  pinMode(Pir_2, INPUT);
  pinMode(Pir_3, INPUT);

  pinMode(Relay_1, OUTPUT);
  pinMode(Relay_2, OUTPUT);
  pinMode(Relay_3, OUTPUT);
  pinMode(Relay_4, OUTPUT);

  Serial.begin(115200);
  setup_wifi();
  Serial.println("trying to connect to mqtt");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println("connected to mqtt");
}


void loop() {

  if(!Started)
  {
    digitalWrite(Relay_1, true);
    digitalWrite(Relay_2, true);
    digitalWrite(Relay_3, true);
    digitalWrite(Relay_4, true);
    Started = true;
  }
  if (!client.connected()) {
    Serial.println("Lost connection with mqtt trying again..");
    reconnect();
    Serial.println("Automatic Offline mode on");
    Automatico();
  } else {
    if (!client.loop())
      client.connect("ESP8266Client");
    now = millis();
    // Publishes state to mantain connection every 10 mins
    if (now - lastMeasure > 60000 * 10) {
      lastMeasure = now;
      client.publish("NodeMCUMain/", "im alive");
    }
    if (Pir_01_Sens) {
      Motion_1 = digitalRead(Pir_1);
      if (Motion_1 != last_Motion_1) {

        last_Motion_1 = Motion_1;
        if (Motion_1) {
          motion_1 = '1';
          client.publish("NodeMCUMain/Pir_1/STATE", "ye", true);
        } else {
          motion_1 = '0';
          client.publish("NodeMCUMain/Pir_1/STATE", "ne", true);
        }
      }
    }
    if (Pir_02_Sens) {
      Motion_2 = digitalRead(Pir_2);
      if (Motion_2 != last_Motion_2) {

        last_Motion_2 = Motion_2;
        if (Motion_2) {
          motion_2 = '1';
          client.publish("NodeMCUMain/Pir_2/STATE", "ye", true);
        } else {
          motion_2 = '0';
          client.publish("NodeMCUMain/Pir_2/STATE", "ne", true);
        }
      }
    }
    if (Pir_03_Sens) {
      Motion_3 = digitalRead(Pir_3);
      if (Motion_3 != last_Motion_3) {

        last_Motion_3 = Motion_3;
        if (Motion_3) {
          motion_3 = '1';
          client.publish("NodeMCUMain/Pir_3/STATE", "ye", true);
        } else {
          motion_3 = '0';
          client.publish("NodeMCUMain/Pir_3/STATE", "ne", true);
        }
      }
    }
    delay(50);
  }




  
}