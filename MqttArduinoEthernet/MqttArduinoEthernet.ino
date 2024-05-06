/*

  En esta version cambie el if del reconnect para que no compruebe hasta 
  que termine la rgb en apagar o prender

*/


#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define DHTTYPE DHT11   // DHT 11

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
//IPAddress ip(192, 168, 1, 118);
//IPAddress gateway(192,168,1,1);
//IPAddress dns(8,8,8,8);
IPAddress server(192, 168, 1, 115);
LiquidCrystal_I2C lcd(0x27,16,2);  //

//void callback(String topic, byte* message, unsigned int length);


EthernetClient ethClient;
PubSubClient client(ethClient);


//Pines usados:

    const int DHTPin   = 47;
    const int RedLED   = 44;
    const int GreenLED = 45;
    const int BlueLED  = 46;
    const int Relay_1  = 40; 
    const int Relay_2  = 39; 
    const int Relay_3  = 42; 
    const int Relay_4  = 43; 
    const int Pir      = 48;
    //const int PowerPin = ;

//Variables globales:

   
    bool last_Motion = false;
    bool Motion      = false;
    bool Pir_Sens    = true;
    bool RGB         = true;
    int motion = 0;
    int mr, mb, mg;
    int r = 70;
    int g = 30;
    int b = 196;
    int ar = 0;
    int ag = 0;
    int ab = 0;
    bool rythm = false;
    int rango = 400; 
    int volumen;
    int Volume = 120;
    int Delay = 10;
    int Delaycycle = 20;
    bool digvol;
    byte Cycle_R = 0;
    byte Cycle_G = 0;
    byte Cycle_B = 0;
    byte Actual_R = 0;
    byte Actual_G = 0;
    byte Actual_B = 0;
    bool LED = false;
    bool AutoLCD = true;
    bool Do_Cycle = false;
    int Brillo = 0;
    int DELAYRC = 20;
    int timer_LCD = 0;
    int tempo, tiempooff = 0;
    bool started = false;
    bool Cliente, Turning = false;
    String hora = "00:00";
    
        

    
// Initialize DHT sensor.

    DHT dht(DHTPin, DHTTYPE);

// Timers auxiliar variables
    long now = millis();
    long lastMeasure, lastvol = 0;


void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.println("");
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.print(". Message: ");
  String messageTemp;
  String Topic = String(topic);
  Serial.println("");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println();
  Serial.print("Topic = ");
  Serial.print(topic);
  Serial.print("  message = ");
  Serial.print(messageTemp);
  Serial.println();

    if(Topic=="ArduinoEthernet/Relay_1")
    {
        Serial.print("Changing Relay 1 to ");
        if(messageTemp == "on")
        {
          digitalWrite(Relay_1, LOW);
          Serial.print("On");
        }
        else
        if(messageTemp == "off")
        {
          digitalWrite(Relay_1, HIGH);
          Serial.print("Off");
        }
    }
    else
    if(Topic=="ArduinoEthernet/Relay_2")
    {
        Serial.print("Changing Relay 2 to ");
        if(messageTemp == "on")
        {
          digitalWrite(Relay_2, LOW);
          Serial.print("On");
        }
        else
        if(messageTemp == "off")
        {
          digitalWrite(Relay_2, HIGH);
          Serial.print("Off");
        }
    }
    else
    if(Topic=="ArduinoEthernet/Relay_3")
    {
        Serial.print("Changing Relay 3 to ");
        if(messageTemp == "on")
        {
          digitalWrite(Relay_3, LOW);
          Serial.print("On");
        }
        else
        if(messageTemp == "off")
        {
          digitalWrite(Relay_3, HIGH);
          Serial.print("Off");
        }
    }
    else
    if(Topic=="ArduinoEthernet/Relay_4")
    {
        Serial.print("Changing Relay 4 to ");
        if(messageTemp == "on")
        {
          digitalWrite(Relay_4, LOW);
          Serial.print("On");
        }
        else
        if(messageTemp == "off")
        {
          digitalWrite(Relay_4, HIGH);
          Serial.print("Off");
        }
    }
    else
    if(Topic=="ArduinoEthernet/LED/RGB")
    {
      if(messageTemp == "audon")
      {
        rythm = true;
      }
      else
      if(messageTemp == "audoff")
      {
        rythm = false;
        analogWrite(RedLED,r);
        analogWrite(GreenLED,g);
        analogWrite(BlueLED,b);
      }
      else
      if(messageTemp == "off")
      {
        
        if(rythm)
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
      else
      if(messageTemp == "on")
      {
        if(rythm)
        {
          Actual_R = r;
          Actual_G = g;
          Actual_B = b;
        }
        RGB =true;
        Cycle_R = r;
        Cycle_G = g;
        Cycle_B = b;
      }
      else
      if(messageTemp == "cycle_on")
      {
        Do_Cycle = true;
      }
      else
      if(messageTemp == "cycle_off")
      {
        Do_Cycle = false;
        Cycle_R = r;
        Cycle_G = g;
        Cycle_B = b;
      }
    }
    if(Topic=="ArduinoEthernet/LED/RGB/COLOR")
    {
      //messageTemp = '#' + messageTemp;
        Serial.println("Changing color to "); 
        long number = (long) strtol( &messageTemp[0], NULL, 16);
        //Split them up into r, g, b values
        r = number >> 16 ;
        g = (number >> 8) & 0xFF;
        b = number & 0xFF;
        Serial.println("R = ");
        Serial.println(r);
        Serial.print("G = ");
        Serial.println(g);
        Serial.print("B = ");
        Serial.println(b);
        Serial.println("");
        if (g == b && r == g)
        {
          r = r - 64;
        }
        if(rythm)
        {
          Actual_R = r;
          Actual_G = g;
          Actual_B = b;
        }
        if(!Turning)
        {
          Cycle_R = r;
          Cycle_G = g;
          Cycle_B = b;
        }
    }
    else
    if(Topic=="ArduinoEthernet/Pir")
      {
        Serial.print("Changing last motion 1 to ");
        
        if(messageTemp == "no")
          {
            Pir_Sens = false;
          }
        else
        if(messageTemp == "yes")
          {
            Pir_Sens = true;
          }
    }
    else
    if(Topic=="ArduinoEthernet/Pir/STATE")
    {
      if(messageTemp == "1")
          {
            last_Motion = true;
            Serial.print("state 1");
          }
        else
        if(messageTemp == "0")
          {
            last_Motion = false;
            Serial.print("state 0");
          }
    }
    else
    if(Topic=="ArduinoEthernet/LED/RGB/BRIGHTNESS")
    {
      Brillo = messageTemp.toInt(); 
      if(!Turning)
        {
          Cycle_R = r*Brillo/255;
          Cycle_G = g*Brillo/255;
          Cycle_B = b*Brillo/255;
        }
       
    }
    else
    if(Topic=="ArduinoEthernet/LED/RGB/VOLUME")
    {
      Volume = messageTemp.toInt();
      
    }
    else
    if(Topic=="ArduinoEthernet/LED/RGB/DELAY")
    {
      Delay = messageTemp.toInt();
       
    }
    else
    if(Topic=="ArduinoEthernet/LED/RGB/DELAYCYCLE")
    {
      Delaycycle = messageTemp.toInt();
       
    }
    else
    if(Topic=="ArduinoEthernet/LCD/BackLight")
    {
      if(messageTemp == "on")
      {
        lcd.backlight();
      }
      if(messageTemp == "off")      
      {
        lcd.noBacklight();
      }
    }
    else
    if(Topic=="ArduinoEthernet/LCD")
    {
      if(messageTemp == "on")
      {
        AutoLCD = true;
      }
      if(messageTemp == "off")      
      {
        AutoLCD = false;
      }
    }
    if (Topic=="ArduinoEthernet/Hora") {
      hora = messageTemp;
    }
}
void Cycle ()
{
  if(Actual_R == Cycle_R && Actual_G == Cycle_G && Actual_B == Cycle_B)
  {
    Cycle_R = random(Brillo);
    Cycle_G = random(Brillo);
    Cycle_B = random(Brillo);
  }
}
void LightGrad()
  {
    if(Actual_R > Cycle_R)
    {
      
      if(!rythm and RGB)
      {
        Actual_R--;
        analogWrite(RedLED  ,Actual_R);
      }
    }
    else 
    if(Actual_R < Cycle_R)
    {
      
      if(!rythm and RGB)
      {
        Actual_R++;
        analogWrite(RedLED  ,Actual_R);
      }
    }

    if(Actual_G > Cycle_G)
    {
      
      if(!rythm and RGB)
      {
        Actual_G--;
        analogWrite(GreenLED,Actual_G);
      }
    }
    else 
    if(Actual_G < Cycle_G)
    {
      
      if(!rythm and RGB)
      {
        Actual_G++;
        analogWrite(GreenLED,Actual_G);
      }
    }

    if(Actual_B > Cycle_B)
    {
      
      if(!rythm and RGB)
      {
        Actual_B--;
        analogWrite(BlueLED ,Actual_B);
      }
    }
    else 
    if(Actual_B < Cycle_B)
    {
      
      if(!rythm and RGB)
      {
        Actual_B++;
        analogWrite(BlueLED ,Actual_B);
      }
    }
    //Serial.println("Dentro LightGrad");
    if(Actual_B == 0 and Actual_R == 0 and Actual_G == 0)
    {
      RGB = false;
      Turning = false;
      //Serial.println("RGB OFF");
    }
    if(Cliente)
    {
      delay(Delaycycle);

    }
    if(Do_Cycle and RGB and !rythm)
    {
      Cycle();
    }    
  }

void reconnect()
{
  // Loop until we're reconnected
  //while (!client.connected()) {
    //Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if ((tiempooff >= 50) and (Actual_R == Cycle_R and Actual_G == Cycle_G and Actual_B == Cycle_B))
    {
      Cliente = client.connect("arduinoClient");

    }

    if (Cliente) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("ArduinoEthernet","Connected");
      client.subscribe("ArduinoEthernet/LED/RGB");
      client.subscribe("ArduinoEthernet/LED/RGB/COLOR");
      client.subscribe("ArduinoEthernet/LED/RGB/VOLUME");
      client.subscribe("ArduinoEthernet/LED/RGB/DELAY");
      client.subscribe("ArduinoEthernet/LED/RGB/DELAYCYCLE");
      client.subscribe("ArduinoEthernet/LED/RGB/BRIGHTNESS");
      client.subscribe("ArduinoEthernet/Relay_1");
      client.subscribe("ArduinoEthernet/Relay_2");
      client.subscribe("ArduinoEthernet/Relay_3");
      client.subscribe("ArduinoEthernet/Relay_4");
      client.subscribe("ArduinoEthernet/Pir/STATE");
      client.subscribe("ArduinoEthernet/Pir");
      client.subscribe("ArduinoEthernet/LCD/BackLight");
      client.subscribe("ArduinoEthernet/LCD");
      client.subscribe("ArduinoEthernet/Hora");
    } else {
      if ((tiempooff >= 50) and (Actual_R == Cycle_R and Actual_G == Cycle_G and Actual_B == Cycle_B))
      {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        tiempooff = 0;
        Serial.println("Attempting MQTT connection...");        
      }
      
      tiempooff++;
    }
  
}

void Automatico()
{
  if(Pir_Sens)
  {
    Motion = digitalRead(Pir); 
    if (Motion != last_Motion)
      {
        if(Motion)
        {
          Serial.println("Motion detected");
          last_Motion = true;
          digitalWrite(Relay_2, LOW); 
          RGB = true;
          //Turning = true;
          tempo = 0;
          Cycle_R = r;
          Cycle_G = g;
          Cycle_B = b;
          if(rythm)
          {
            Actual_R = r;
            Actual_G = g;
            Actual_B = b;
          }
          
        }
        else
        {
          Serial.println("motion not detected");
          
          last_Motion = false;
        }
      }
    else
    {
      //Serial.println("Motion not detected");

    }
    if(tempo >= 10*60*4)
    {
      Cycle_R = 0;
      Cycle_G = 0;
      Cycle_B = 0;
      if(rythm)
        {
          mr = 0;
          mg = 0;
          mb = 0;
        }
      //tempo = 0;
    }
    if(tempo >= 10*60*5)
    {
      tempo = 0;
      digitalWrite(Relay_2, HIGH); 
    }
    tempo++;
    //Serial.println(tempo);
  }
   if(rythm)
  {
    volumen = analogRead(A0);
    //Serial.println(volumen);
    //int vol = volumen - 50;
    
    if(volumen > Volume) //si el bajo es muy alto 
    {
      if(Do_Cycle)
      {
        mr = random(Brillo);
        mg = random(Brillo);
        mb = random(Brillo);
      }
      else
      {
        mr = Actual_R;       //damos lo valores del color seleccionado 
        mg = Actual_G;
        mb = Actual_B;
      }
      
    }
    
    analogWrite(RedLED, mr);    //actualizamos la tira
    analogWrite(GreenLED, mg);
    analogWrite(BlueLED, mb);
    if(mr > 5)
    {
      mr = mr - 5;
      //mr--;
    }
    if(mg > 5)
    {
      mg = mg - 5;
      //mg--;
    }
    if(mb > 5)
    {
      mb = mb - 5;
      //mb--;
    }
    delay(Delay);
  }
  else
  {
    LightGrad();
    /*
    Serial.println("");
    Serial.print("Actual R: ");
    Serial.print(Actual_R);
    Serial.print("  Actual G: ");
    Serial.print(Actual_G);
    Serial.print("  Actual B: ");
    Serial.print(Actual_B);
    Serial.println("RGB  ");
    Serial.println(RGB);
    Serial.println("");
    */
  }
  if(Actual_R == Cycle_R and Actual_G == Cycle_G and Actual_B == Cycle_B)
  {
    delay(100);
  }
  else
  {
    delay(15);
  }
  //Serial.println("Automatico");
}

void setup() {
  
    pinMode(RedLED   ,OUTPUT);
    pinMode(GreenLED ,OUTPUT);
    pinMode(BlueLED  ,OUTPUT);
    pinMode(Relay_1  ,OUTPUT);
    pinMode(Relay_2  ,OUTPUT);
    pinMode(Relay_3  ,OUTPUT);
    pinMode(Relay_4  ,OUTPUT);
    pinMode(Pir      ,INPUT );

    dht.begin();
  
    Serial.begin(115200);
    lcd.init();
  
    //Encender la luz de fondo.
    lcd.backlight();
    lcd.println("Conectando a la red");
    client.setServer(server, 1883);
    client.setCallback(callback);
    Serial.println("trying to get ip");
     if (Ethernet.begin(mac) == 0) {
      Serial.println("Error al configurar la conexión Ethernet mediante DHCP");
      lcd.println("Error DHCP");

      while (1);  // Si hay un error, detener la ejecución
    }
    lcd.clear();
    lcd.print("Conexion exitosa");
    lcd.clear();
    lcd.print("Servidor en:");
    lcd.setCursor(0,1);
    lcd.print(Ethernet.localIP());
    
    // Imprimir la dirección IP asignada al Arduino
    Serial.print("Dirección IP asignada: ");
    Serial.println(Ethernet.localIP());
    delay(1500);
    digitalWrite(Relay_1,HIGH);
    digitalWrite(Relay_2,LOW);
    digitalWrite(Relay_3,HIGH);
    digitalWrite(Relay_4,HIGH);
    now = 60000*20 + lastMeasure + 1;
   
  
    //Encender la luz de fondo.
    
  
    // Escribimos el Mensaje en el LCD.
    
    
    lcd.clear();
    analogWrite(RedLED,255);
    delay(1000);
    analogWrite(RedLED,0);
    delay(100);
    analogWrite(GreenLED,255);
    delay(1000);
    analogWrite(GreenLED,0);
    delay(100);
    analogWrite(BlueLED,255);
    delay(1000);
    analogWrite(BlueLED,0);
    delay(100);
    analogWrite(RedLED,70);
    analogWrite(GreenLED,30);
    analogWrite(BlueLED,196);
    Actual_R = 70;
    Actual_G = 30;
    Actual_B = 196;    
    lcd.print("Started");
    delay(1000);
    lcd.clear();
    lcd.noBacklight();
}


void loop()
{
  
    
    


  if (!client.connected()) {
    reconnect();
    Automatico();
  }
  else
  {
    now = millis();
    // Publishes new temperature and humidity every 20 minutes
    if ((now - lastMeasure > 60000*1) or !started) {
      lastMeasure = now;
      started = true;
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();



      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t) ) {
        Serial.println("Failed to read from DHT sensor!");
        lcd.setCursor(0,1);
        lcd.print("Failed DHT sensor");
        //started = false;  
        delay(1000); 
        return;
      }

      // Computes temperature values in Celsius
      int hic = dht.computeHeatIndex(t, h, false);
      static char temperatureTemp[7];
      dtostrf(hic, 6, 2, temperatureTemp);
      static char temperatureT[7];
      dtostrf(t, 6, 2, temperatureT);
      static char humidityTemp[7];
      dtostrf(h, 6, 2, humidityTemp);

      client.publish("ArduinoEthernet/DHT/temperature", temperatureT,true);
      client.publish("ArduinoEthernet/DHT/temperatureindex", temperatureTemp,true);
      client.publish("ArduinoEthernet/DHT/humidity", humidityTemp,true);

      Serial.print("Humidity: ");
      Serial.print(h);
      Serial.print(" %\t Temperature: ");
      Serial.print(t);
      Serial.print(" *C ");

      Serial.print(" *F\t Heat index: ");
      Serial.print(hic);
      Serial.println(" *C ");
      lcd.clear();
      lcd.home();
      lcd.setCursor(0,1);
      lcd.print("Temp:");
      lcd.print(t,0); //1 decimal
      lcd.print((char) 223); // "337" -> "°"
      lcd.print("C  "); 
      lcd.print(hora); 
      lcd.setCursor(0,0);
      lcd.print("Sens:");
      lcd.print(hic,1); //1 decimal
      lcd.print((char) 223); // "337" -> "°"
      lcd.print("C  "); // "337" -> "°"
      lcd.print("H:");
      lcd.print(h,0); //1 decimal
      lcd.print("%  "); // "337" -> "°"
      //lcd.autoscroll(); 
      
    }
    if(Pir_Sens)
    {
      Motion = digitalRead(Pir);
      if(AutoLCD)
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
        
        if(Motion)
        {
          Serial.println("Motion detected");
          last_Motion = true;
          
          client.publish("ArduinoEthernet/Pir/STATE","ye",true);
        }
        else
        {
          Serial.println("motion not detected");
          client.publish("ArduinoEthernet/Pir/STATE","ne",true);
          last_Motion = false;
        }
      
      }
    }
  }
  client.loop();

  if(rythm)
  {
    volumen = analogRead(A0);
    //
    Serial.println(volumen);
    //int vol = volumen - 50;
    
      //Serial.println(lastvol);
      
      
    if(volumen > Volume) //si el bajo es muy alto 
    {
      lastvol = millis();
      if(Do_Cycle and lastvol + 200 >= now )
      {
        mr = random(Brillo);
        mg = random(Brillo);
        mb = random(Brillo);
         
      }
      else
      {
        mr = Actual_R;       //damos lo valores del color seleccionado 
        mg = Actual_G;
        mb = Actual_B;
      }
      
    }
    
    analogWrite(RedLED, mr);    //actualizamos la tira
    analogWrite(GreenLED, mg);
    analogWrite(BlueLED, mb);
    if(mr > 2)
    {
      mr = mr - 2;
      //mr--;
    }
    if(mg > 2)
    {
      mg = mg - 2;
      //mg--;
    }
    if(mb > 2)
    {
      mb = mb - 2;
      //mb--;
    }
    delay(Delay);
  }
  else
  {
    LightGrad();
  }
  /*if(timer_LCD >= 40)
  {
    timer_LCD = 0;
    lcd.scrollDisplayLeft();
    //lcd.autoscroll();        
  }
  timer_LCD++;
  */
}

