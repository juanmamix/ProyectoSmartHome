#include <ESP8266WiFi.h>

#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTTYPE DHT11

ESP8266WebServer server(80);

LiquidCrystal_I2C lcd(0x27,16,2);

// Wifi Setup
const char *WIFI_SSID[] = {"Tenda_3B2DF8","Mechatronic"}; // Nombres de las redes Wi-Fi conocidas
const char *WIFI_PASSWORD[] = {"9KBHqiTr","m3ch4tr0n1c"}; // Claves de las redes Wi-Fi conocidas
const int NUM_NETWORKS = 2;                                // Número de redes Wi-Fi conocidas

const char *mqtt_server = "192.168.0.115";

WiFiClient espClient;
PubSubClient client(espClient);

// Pines Utilizados:
// Pines usados:

const int DHTPin = D4;   // 16 digital input
const int RedLED = D5;   // 14 pwm
const int GreenLED = D6; // 12 pwm
const int BlueLED = D7;  // 13 pwm
const int Relay_1 = D0;  // 2 digital out
const int Pir = D8;      // 15 digital input
//  SCL = D1
//  SDA = D2

// Variables globales:

bool last_Motion = false;
bool Motion = false;
bool Pir_Sens = true;
bool RGB = false;
int motion = 0;
int mr, mb, mg;
long r = 80;
long g = 0;
long b = 100;
long ar,ag,ab = 0;
bool rythm = false;
int rango = 400;
int volumen;
int Volume = 120;
int Delay = 10;
int Delaycycle = 20;
bool digvol;
byte Cycle_R,Cycle_G,Cycle_B,Actual_R,Actual_G,Actual_B = 0;
bool LED = false;
bool AutoLCD = true;
bool Do_Cycle = false;
int Brillo = 0;
int DELAYRC = 20;
int timer_LCD = 0;
int tempo, tiempooff = 0;
bool started = false;
bool Turning = false;


int brightness,percent_brightness,delta_max = 0;



// Initialize DHT sensor.

DHT DHT(DHTPin, DHTTYPE);

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;
// Variables globales:



void connectToWifi()
{
    int i = 0;
    WiFi.mode(WIFI_STA); // Configura la NodeMCU para que actúe como cliente
    while (i < NUM_NETWORKS)
    {
        Serial.printf("Conectando a la red %s...\n", WIFI_SSID[i]);
        WiFi.begin(WIFI_SSID[i], WIFI_PASSWORD[i]); // Intenta conectarse a la red Wi-Fi
        unsigned long startTime = millis();
        while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < 10000)
        { // Espera hasta que se conecte o hasta 10 segundos
            delay(500);
            Serial.print(".");
        }
        if (WiFi.status() == WL_CONNECTED)
        { // Si se ha conectado a la red Wi-Fi
            Serial.printf("\nConectado a la red %s. Dirección IP: %s\n", WIFI_SSID[i], WiFi.localIP().toString().c_str());
            return;
        }
        else
        { // Si no se ha conectado a la red Wi-Fi
            Serial.printf("\nNo se pudo conectar a la red %s.\n", WIFI_SSID[i]);
            i++; // Intenta conectarse a la siguiente red Wi-Fi conocida
        }
    }
    Serial.println("No se pudo conectar a ninguna red Wi-Fi conocida.");
}

void callback(String Topic, byte *message, unsigned int length)
{
    Serial.print("Message arrived on topic: ");
    Serial.print(Topic);
    Serial.print(". Message: ");
    String messageTemp;

    for (int i = 0; i < length; i++)
    {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println();
    if (Topic == "RoomNodeMCU/Relay_1")
    {
        Serial.print("Changing Relay 1 to ");
        if (messageTemp == "on")
        {
            digitalWrite(Relay_1, LOW);
            Serial.print("On");
        }
        else if (messageTemp == "off")
        {
            digitalWrite(Relay_1, HIGH);
            Serial.print("Off");
        }
    }
    else if (Topic == "RoomNodeMCU/LED/RGB")
    {
        if (messageTemp == "audon")
        {
            rythm = true;
        }
        else if (messageTemp == "audoff")
        {
            rythm = false;
            analogWrite(RedLED, r);
            analogWrite(GreenLED, g);
            analogWrite(BlueLED, b);
        }
        else if (messageTemp == "off")
        {

            if (rythm)
            {
                mr = 0;
                mg = 0;
                mb = 0;
            }
            
            
            Serial.println("Turning off the led strip");
            Turning = true;
            last_Motion = false;
            Cycle_R = 0;
            Cycle_G = 0;
            Cycle_B = 0;
        }
        else if (messageTemp == "on")
        {
            if (rythm)
            {
                Actual_R = r;
                Actual_G = g;
                Actual_B = b;
            }
                    
            RGB = true;
            Cycle_R = r;
            Cycle_G = g;
            Cycle_B = b;
        }
        else if (messageTemp == "cycle_on")
        {
            Do_Cycle = true;
        }
        else if (messageTemp == "cycle_off")
        {
            Do_Cycle = false;
            Cycle_R = r;
            Cycle_G = g;
            Cycle_B = b;
        }
        else
        {
            // messageTemp = '#' + messageTemp;
            
                     
            // Convertir valor hexadecimal a valores RGB
            
            long number = (long)strtol(&messageTemp[0], NULL, 16);
            // Split them up into r, g, b values
            r = map((number >> 16), 0, 255, 0, 1023);
            g = map(((number >> 8) & 0xFF), 0, 255, 0, 1023);
            b = map((number & 0xFF), 0, 255, 0, 1023);
            
            Serial.println("");
            Serial.print("R = ");
            Serial.println(r);
            Serial.print("G = ");
            Serial.println(g);
            Serial.print("B = ");
            Serial.println(b);
            Serial.println("");
    

            if (rythm)
            {
                Actual_R = r;
                Actual_G = g;
                Actual_B = b;
            }
            if (!Turning)
            {
                Cycle_R = r;
                Cycle_G = g;
                Cycle_B = b;
            }
        }
    }
    else if (Topic == "RoomNodeMCU/Pir")
    {
        Serial.print("Changing last motion 1 to ");

        if (messageTemp == "no")
        {
            Pir_Sens = false;
        }
        else if (messageTemp == "yes")
        {
            Pir_Sens = true;
  
        }
    }
    else if (Topic == "RoomNodeMCU/Pir/STATE")
    {
        if (messageTemp == "1")
        {
            last_Motion = true;
            Serial.print("state 1");
        }
        else if (messageTemp == "0")
        {
            last_Motion = false;
            Serial.print("state 0");
        }
    }
    else if (Topic == "RoomNodeMCU/LED/RGB/BRIGHTNESS")
    {
        Brillo = messageTemp.toInt();
        if (!Turning)
        {
            Cycle_R = r * Brillo / 255;
            Cycle_G = g * Brillo / 255;
            Cycle_B = b * Brillo / 255;
        }
    }
    else if (Topic == "RoomNodeMCU/LED/RGB/VOLUME")
    {
        Volume = messageTemp.toInt();
    }
    else if (Topic == "RoomNodeMCU/LED/RGB/DELAY")
    {
        Delay = messageTemp.toInt();
    }
    else if (Topic == "RoomNodeMCU/LED/RGB/DELAYCYCLE")
    {
        Delaycycle = messageTemp.toInt();
    }
    else if (Topic == "RoomNodeMCU/LCD/BackLight")
    {
        if (messageTemp == "on")
        {
            lcd.backlight();
        }
        if (messageTemp == "off")
        {
            lcd.noBacklight();
        }
    }
    else if (Topic == "RoomNodeMCU/LCD")
    {
        if (messageTemp == "on")
        {
            AutoLCD = true;
        }
        if (messageTemp == "off")
        {
            AutoLCD = false;
        }
    }
}



void Cycle()
{
    if (Actual_R == Cycle_R && Actual_G == Cycle_G && Actual_B == Cycle_B)
    {
        Cycle_R = random(Brillo);
        Cycle_G = random(Brillo);
        Cycle_B = random(Brillo);
    }
}
void LightGrad()
{
    if (Actual_R > Cycle_R)
    {

        if (!rythm and RGB)
        {
            Actual_R--;
            analogWrite(RedLED, Actual_R);
        }
    }
    else if (Actual_R < Cycle_R)
    {

        if (!rythm and RGB)
        {
            Actual_R++;
            analogWrite(RedLED, Actual_R);
        }
    }

    if (Actual_G > Cycle_G)
    {

        if (!rythm and RGB)
        {
            Actual_G--;
            analogWrite(GreenLED, Actual_G);
        }
    }
    else if (Actual_G < Cycle_G)
    {

        if (!rythm and RGB)
        {
            Actual_G++;
            analogWrite(GreenLED, Actual_G);
        }
    }

    if (Actual_B > Cycle_B)
    {

        if (!rythm and RGB)
        {
            Actual_B--;
            analogWrite(BlueLED, Actual_B);
        }
    }
    else if (Actual_B < Cycle_B)
    {

        if (!rythm and RGB)
        {
            Actual_B++;
            analogWrite(BlueLED, Actual_B);
        }
    }
    if (Actual_B == 0 and Actual_R == 0 and Actual_G == 0)
    {
        RGB = false;
        Turning = false;
    }
    delay(Delaycycle);
    if (Do_Cycle and RGB and !rythm)
    {
        Cycle();
    }
}









void reconnect()
{
    // offline();
    Serial.print("Attempting MQTT connection...");
    
    if (!client.connected())
    {
      Serial.println("dentro loop1");
        if (client.connect("RoomNodeMCU"))
        {
            Serial.println("connected");
            client.publish("RoomNodeMCU", "Connected");
            client.subscribe("RoomNodeMCU/LED/RGB");
            client.subscribe("RoomNodeMCU/LED/RGB/VOLUME");
            client.subscribe("RoomNodeMCU/LED/RGB/DELAY");
            client.subscribe("RoomNodeMCU/LED/RGB/DELAYCYCLE");
            client.subscribe("RoomNodeMCU/LED/RGB/BRIGHTNESS");
            client.subscribe("RoomNodeMCU/Relay_1");
            client.subscribe("RoomNodeMCU/Pir/STATE");
            client.subscribe("RoomNodeMCU/Pir");
            client.subscribe("RoomNodeMCU/LCD/BackLight");
            client.subscribe("RoomNodeMCU/LCD");
        }
        else
        {
           
                Serial.print("failed, rc=");
                Serial.print(client.state());
                Serial.println(" try again in 5 seconds");
                // Wait 5 seconds before retrying
                delay(5000);
                Serial.println("Attempting MQTT connection...");
                
           
            
            // delay(50);
            Serial.println("dentro loop2");
        }
    }
    if (Pir_Sens)
    {
      Motion = digitalRead(Pir);
      if (Motion)
      {
        Cycle_R = r;
        Cycle_G = g;
        Cycle_B = b; 
        
      }
      else
      {
        Cycle_R = 0;
        Cycle_G = 0;
        Cycle_B = 0;  
      }            
    }
    
}

void setup()
{
    pinMode(RedLED, OUTPUT);
    pinMode(GreenLED, OUTPUT);
    pinMode(BlueLED, OUTPUT);
    pinMode(Relay_1, OUTPUT);
    pinMode(Pir, INPUT);

    DHT.begin();

    Serial.begin(115200);

    connectToWifi();

    Serial.println("trying to connect to mqtt");
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    Serial.println("connected to mqtt");

    now = 60000 * 20 + lastMeasure + 1;
    lcd.init();

    // Encender la luz de fondo.relay
    lcd.backlight();

    // Escribimos el Mensaje en el LCD.
    lcd.print("Started");
    delay(1000);
    lcd.clear();
    digitalWrite(Relay_1,true);    
    analogWrite(RedLED,1023);
    analogWrite(GreenLED,1023);
    analogWrite(BlueLED,1023);
    Actual_R = analogRead(RedLED);
    Actual_G = analogRead(GreenLED);
    Actual_B = analogRead(BlueLED);    
}

void loop()
{

    if (!client.connected())
    {
        Serial.println("Lost connection with mqtt trying again..");
        reconnect();
        //server.handleClient();
        LightGrad();

    }
    else
    {
        now = millis();
        // Publishes new temperature and humidity every 20 minutes
        if ((now - lastMeasure > 60000 * 1) or !started)
        {
            lastMeasure = now;
            started = true;
            // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
            float h = DHT.readHumidity();
            // Read temperature as Celsius (the default)
            float t = DHT.readTemperature();

            // Check if any reads failed and exit early (to try again).
            if (isnan(h) || isnan(t))
            {
                Serial.println("Failed to read from DHT sensor!");
                lcd.setCursor(0, 1);
                lcd.print("Failed DHT sensor");
                // started = false;
                delay(1000);
                return;
            }

            // Computes temperature values in Celsius
            int hic = DHT.computeHeatIndex(t, h, false);
            static char temperatureTemp[7];
            dtostrf(hic, 6, 2, temperatureTemp);
            static char temperatureT[7];
            dtostrf(t, 6, 2, temperatureT);
            static char humidityTemp[7];
            dtostrf(h, 6, 2, humidityTemp);

            client.publish("RoomNodeMCU/DHT/temperature", temperatureT, true);
            client.publish("RoomNodeMCU/DHT/temperatureindex", temperatureTemp, true);
            client.publish("RoomNodeMCU/DHT/humidity", humidityTemp, true);

            Serial.print("Humidity: ");
            Serial.print(h);
            Serial.print(" %\t Temperature: ");
            Serial.print(t);
            Serial.print(" *C ");

            Serial.print(" *F\t Heat index: ");
            Serial.print(hic);
            Serial.println(" *C ");
            lcd.setCursor(0, 1);
            lcd.print("Temp:");
            lcd.print(t, 0);      // 1 decimal
            lcd.print((char)223); // "337" -> "°"
            lcd.print("C    ");   // "337" -> "°"
            lcd.print("Sens:");
            lcd.print(hic, 1);    // 1 decimal
            lcd.print((char)223); // "337" -> "°"
            lcd.print("C    ");   // "337" -> "°"
            lcd.setCursor(0, 0);
            lcd.print("Hum:");
            lcd.print(h, 0);    // 1 decimal
            lcd.print("%    "); // "337" -> "°"
            lcd.autoscroll();
        }
        if (!client.loop())
            client.connect("ESP8266Client");
        now = millis();
        // Publishes state to mantain connection every 10 mins
        if (now - lastMeasure > 60000 * 10)
        {
            lastMeasure = now;
            client.publish("RoomNodeMCU/", "im alive");
        }
    }
    if (Pir_Sens)
    {
        Motion = digitalRead(Pir);
        if (AutoLCD)
        {
            if (Motion)
            {
                lcd.backlight();
            }
            else
            {
                lcd.noBacklight();
            }
        }
        if (Motion != last_Motion)
        {

            if (Motion)
            {
                Serial.println("Motion detected");
                last_Motion = true;

                client.publish("RoomNodeMCU/Pir/STATE", "ye", true);
            }
            else
            {
                Serial.println("motion not detected");
                client.publish("RoomNodeMCU/Pir/STATE", "ne", true);
                last_Motion = false;
            }
        }
    }

    client.loop();
       
    if (rythm)
    {
        volumen = analogRead(A0);
        Serial.println(volumen);
        // int vol = volumen - 50;

        if (volumen > Volume) // si el bajo es muy alto
        {
            if (Do_Cycle)
            {
                mr = random(Brillo);
                mg = random(Brillo);
                mb = random(Brillo);
            }
            else
            {
                mr = Actual_R; // damos lo valores del color seleccionado
                mg = Actual_G;
                mb = Actual_B;
            }
        }

        analogWrite(RedLED, mr); // actualizamos la tira
        analogWrite(GreenLED, mg);
        analogWrite(BlueLED, mb);
        if (mr > 32)
        {
            mr = mr - 16;
            // mr--;
        }
        if (mg > 32)
        {
            mg = mg - 16;
            // mg--;
        }
        if (mb > 32)
        {
            mb = mb - 16;
            // mb--;
        }
        delay(Delay);
    }
    else
    {
        LightGrad();
      //(smoothTransition();        
    }
    if (timer_LCD >= 40)
    {
        timer_LCD = 0;
        lcd.scrollDisplayLeft();
        // lcd.autoscroll();
    }
    timer_LCD++;
}
/*
void handleButton(int pin, String state) {
  digitalWrite(pin, state == "ON" ? HIGH : LOW);
}

// Generate web page
void generatePage() {
  String page = "<html><body><h1>Control de Pines</h1>";
  page += "<p><a href=\"/?pin1=ON\"><button>Techo ON</button></a>&nbsp;<a href=\"/?pin1=OFF\"><button>Techo OFF</button></a></p>";
  page += "<p><a href=\"/?pin2=ON\"><button>PIR ON</button></a>&nbsp;<a href=\"/?pin2=OFF\"><button>PIR OFF</button></a></p>";
  page += "<p><a href=\"/?pin3=ON\"><button>RGB ON</button></a>&nbsp;<a href=\"/?pin3=OFF\"><button>RGB OFF</button></a></p>";
  page += "<p><a href=\"/?pin4=ON\"><button>Pin 4 ON</button></a>&nbsp;<a href=\"/?pin4=OFF\"><button>Pin 4 OFF</button></a></p>";
  page += "</body></html>";
  server.send(200, "text/html", page);
}

// Handle incoming client requests
void handleRequest() {
  String pin1State = server.arg("pin1");
  String pin2State = server.arg("pin2");
  String pin3State = server.arg("pin3");
  String pin4State = server.arg("pin4");

  if (pin1State != "") {
    handleButton(Relay_1, pin1State);
  }

  if (pin2State != "") {
    if(pin2State == "ON")
    {
      Pir_Sens = true;        
    }
    else
    {
      Pir_Sens = false;      
    }    
  }

  if (pin3State != "") {
    if(pin3State == "ON")
    {
        Cycle_R = r;
        Cycle_G = g;
        Cycle_B = b; 
      }
      else
      {
        Cycle_R = 0;
        Cycle_G = 0;
        Cycle_B = 0;  
      }
  }

  if (pin4State != "") {
    if(pin4State == "ON")
    {
      lcd.backlight();
    }
    else
    {
      lcd.noBacklight();
    }
  }

  generatePage();
}
*/