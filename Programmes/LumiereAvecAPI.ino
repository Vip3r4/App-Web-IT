#include <WiFiEspAT.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

// --- Configuration WiFi ---
char ssid[] = "A35 de Tom";
char pass[] = "07122009ToM69";

// --- Configuration API ---
const char* host = "172.19.248.152";  // Votre IP
const int httpPort = 8000;

// --- Connexion module UART WiFi ---
SoftwareSerial wifiSerial(2, 3);  // RX=2, TX=3

// --- Variables existantes ---
const int button = 8;
const int relais = 3;
const int buzzer = 6;
const int capteurLum = A0;  // Changé pour une pin analogique
unsigned long Temps;
unsigned long startTime = 0;

int ancienEtat = 1;
int etatBouton;

bool mode = false;           // Mode local (bouton physique)
bool etatLedAPI = false;     // Valeur lue depuis l'API (toggle du site)
unsigned long dernierRefreshAPI = 0;
const long intervalRefreshAPI = 2000;  // Rafraîchir l'API toutes les 2 secondes

void setup() {
  Serial.begin(9600);
  wifiSerial.begin(115200);
  
  pinMode(button, INPUT_PULLUP);
  pinMode(relais, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  startTime = millis();
  
  WiFi.init(&wifiSerial);
  Serial.print("Connexion WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" OK !");
}

void loop() {
  if (millis() - dernierRefreshAPI >= intervalRefreshAPI) {
    lireEtatAPI();
    dernierRefreshAPI = millis();
  }
  
  etatBouton = digitalRead(button);
  Temps = millis() - startTime;

  Serial.print("Time: ");
  Serial.print(Temps / 1000);
  Serial.print(" | Mode local: ");
  Serial.print(mode);
  Serial.print(" | LED API: ");
  Serial.println(etatLedAPI ? "ON" : "OFF");

  if (ancienEtat == 1 && etatBouton == 0) {
    mode = !mode;
  }

  if (etatLedAPI) {
    digitalWrite(relais, HIGH);
  } 
  else if (mode || (Temps < 16000)) {
    digitalWrite(relais, HIGH);
  } 
  else if ((Temps > 8000) || !mode) {
    digitalWrite(relais, LOW);
  }

  if (Temps >= 24000) {
    startTime = millis();
  }

  ancienEtat = etatBouton;
  isTurnedOn();
  
  delay(50);
}

void lireEtatAPI() {
  WiFiClient client;
  
  if (!client.connect(host, httpPort)) {
    Serial.println("⚠️ Connexion API échouée");
    return;
  }

  client.print("GET /api/serre HTTP/1.1\r\n");
  client.print("Host: ");
  client.print(host);
  client.print(":");
  client.println(httpPort);
  client.println("Connection: close");
  client.println();

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 3000) {
      Serial.println("❌ Timeout API");
      client.stop();
      return;
    }
  }

  // AFFICHER TOUTE LA RÉPONSE (debug)
  Serial.println("=== REPONSE API ===");
  while (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
  }
  Serial.println("==================");
  
  client.stop();
}

void isTurnedOn() {
  int capteurValue = analogRead(capteurLum);

  if (mode || Temps < 16000) {
    if (capteurValue > 600) {
      digitalWrite(buzzer, HIGH);
    } else {
      digitalWrite(buzzer, LOW);
      return;
    }
  }
}