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


byte Relay_1_ON[] = {0xA0, 0x01, 0x01, 0xA2};
byte Relay_2_ON[] = {0xA0, 0x02, 0x01, 0xA3};
byte Relay_3_ON[] = {0xA0, 0x03, 0x01, 0xA4};
byte Relay_4_ON[] = {0xA0, 0x04, 0x01, 0xA5};

byte Relay_1_OFF[] = {0xA0, 0x01, 0x00, 0xA1};
byte Relay_2_OFF[] = {0xA0, 0x02, 0x00, 0xA2};
byte Relay_3_OFF[] = {0xA0, 0x03, 0x00, 0xA3};
byte Relay_4_OFF[] = {0xA0, 0x04, 0x00, 0xA4};

long now = millis();
long lastMeasure = 0;
byte brillo = 64;
byte actbrillo, lastbrillo = 0;
bool LED = true;
bool Turning = false;

// Pines 
const byte PWM_Led = 2;

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
    if(topic=="ESPRelay/Relay_1")
    {
        Serial.print("Changing Relay 1 to ");
        if(messageTemp == "on")
        {
          Serial.write(Relay_1_ON, sizeof(Relay_1_ON));
          Serial.print("On");
        }
        else
        if(messageTemp == "off")
        {
          Serial.write(Relay_1_OFF, sizeof(Relay_1_OFF));
          Serial.print("Off");
        }
    }
    if(topic=="ESPRelay/Relay_2")
    {
        Serial.print("Changing Relay 2 to ");
        if(messageTemp == "on")
        {
          Serial.write(Relay_2_ON, sizeof(Relay_2_ON));
          Serial.print("On");
        }
        else
        if(messageTemp == "off")
        {
          Serial.write(Relay_2_OFF, sizeof(Relay_2_OFF));
          Serial.print("Off");
        }
    }
    if(topic=="ESPRelay/Relay_3")
    {
        Serial.print("Changing Relay 3 to ");
        if(messageTemp == "on")
        {
          Serial.write(Relay_3_ON, sizeof(Relay_3_ON));
          Serial.print("On");
        }
        else
        if(messageTemp == "off")
        {
          Serial.write(Relay_3_OFF, sizeof(Relay_3_OFF));
          Serial.print("Off");
        }
    }
    if(topic=="ESPRelay/Relay_4")
    {
        Serial.print("Changing Relay 4 to ");
        if(messageTemp == "on")
        {
          Serial.write(Relay_4_ON, sizeof(Relay_4_ON));
          Serial.print("On");
        }
        else
        if(messageTemp == "off")
        {
          Serial.write(Relay_4_OFF, sizeof(Relay_4_OFF));
          Serial.print("Off");
        }
    }
    if(topic=="ESPRelay/LED")
    {
       
        if(messageTemp == "on")
        {
          //analogWrite(PWM_Led, brillo);
          LED = true;
          
          brillo = lastbrillo;
          
        }
        else
        if(messageTemp == "off")
        {
          //analogWrite(PWM_Led, 0);
          LED = false;
          
          lastbrillo = brillo;
          brillo = 0;

        }
        else
        {
          if(LED)
          {
            brillo = messageTemp.toInt();
            
          }
          else
          {
            lastbrillo = messageTemp.toInt();
          }
          
        }
        
    }
}

void Gradiente()
{
  if(actbrillo > brillo)
  {
    actbrillo--;
    analogWrite(PWM_Led, actbrillo);
  }  
  else
  if(actbrillo < brillo)
  {
    actbrillo++;
    analogWrite(PWM_Led, actbrillo);
  }
  
  
}

void reconnect() 
{
  
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESPRelay")) 
    {
      Serial.println("connected");  

      client.subscribe("ESPRelay/Relay_1");
      client.subscribe("ESPRelay/Relay_2");
      client.subscribe("ESPRelay/Relay_3");
      client.subscribe("ESPRelay/Relay_4");
      client.subscribe("ESPRelay/LED");
    } 
    else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
    delay(100);
  }
}

void setup()
{

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client");
  
  now = millis();
  // Publishes state to mantain the connection every 10 mins
  if (now - lastMeasure > 60000*10)
  {
    lastMeasure = now;
    client.publish("NodeMCUMain/", "im alive");
  }
  Gradiente();
  delay(50);  
    
}
