#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Wifi Setup
const char *WIFI_SSID[] = {"Tenda_3B2DF8","ALVARO"}; // Nombres de las redes Wi-Fi conocidas
const char *WIFI_PASSWORD[] = {"9KBHqiTr","valentino1"}; // Claves de las redes Wi-Fi conocidas
const int NUM_NETWORKS = 2;                                // Número de redes Wi-Fi conocidas

// Agrega más redes Wi-Fi si es necesario

const char* host = "esp32-cam";
const int ledPin1 = 2; // GPIO para el primer LED
const int ledPin2 = 14; // GPIO para el segundo LED

bool ledState1 = HIGH; // Estado del LED 1 (inicialmente apagado)
bool ledState2 = HIGH; // Estado del LED 2 (inicialmente apagado)


AsyncWebServer server(80);

void connectToWifi()
{
    int i = 0;
    //WiFi.mode(WIFI_STA); // Configura la NodeMCU para que actúe como cliente
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


void setup() {
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin1,HIGH);
  digitalWrite(ledPin2,HIGH);
  Serial.begin(115200);

  // Conéctate a una red Wi-Fi conocida
  /*WiFi.begin(ssid1, password1);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red Wi-Fi 1...");
  }

  Serial.println("Conexión exitosa a la red Wi-Fi 1");
  */
  // Configura la segunda red Wi-Fi (si se necesita)
  // WiFi.begin(ssid2, password2);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Conectando a la red Wi-Fi 2...");
  // }

  // Serial.println("Conexión exitosa a la red Wi-Fi 2");
  connectToWifi();
  // Configura rutas para controlar los LEDs
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body>";
    html += "<h1>Control de LEDs</h1>";
    // Botón para el LED 1 (toggle)
    html += "<p>LED 1: <a href='/toggle1'><button>" + String(ledState1 ? "Encender" : "Apagar") + "</button></a></p>";
    // Botón para el LED 2 (toggle)
    html += "<p>LED 2: <a href='/toggle2'><button>" + String(ledState2 ? "Encender" : "Apagar") + "</button></a></p>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/toggle1", HTTP_GET, [](AsyncWebServerRequest *request){
    ledState1 = !ledState1; // Cambia el estado del LED 1
    digitalWrite(ledPin1, ledState1);
    request->redirect("/");
  });

  server.on("/toggle2", HTTP_GET, [](AsyncWebServerRequest *request){
    ledState2 = !ledState2; // Cambia el estado del LED 2
    digitalWrite(ledPin2, ledState2);
    request->redirect("/");
  });

  server.begin();
}

void loop() {
  // Tu código de loop aquí
  delay(100);
}