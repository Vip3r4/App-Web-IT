#include <ArduinoJson.h>

// --- CONFIGURATION ---
char ssid[] = "A35 de Tom";
char pass[] = "07122009ToM69";
const char* host = "10.200.153.38";
const int httpPort = 8000;

// --- PINS MEGA ---
const int relais = 6;      
const int moteurVitesse = 9;
const int moteurSens = 8;
const int encA = 21; 
const int encB = 20;

#define wifiSerial Serial1

// --- VARIABLES ---
volatile long impulsionCount = 0;
const float PULSATIONS_PAR_DEGRE = 11.6;
int angleActuel = 0;
int angleCible = 0;
bool moteurEnMouvement = false;

// --- INTERRUPTION ENCODEUR ---
void onEncodeur() {
  if (moteurEnMouvement) {
    if (digitalRead(encA) == digitalRead(encB)) impulsionCount++;
    else impulsionCount--;

    long cible = abs(angleCible - angleActuel) * PULSATIONS_PAR_DEGRE;
    if (abs(impulsionCount) >= cible) {
      analogWrite(moteurVitesse, 0);
      moteurEnMouvement = false;
      angleActuel = angleCible;
      Serial.print("✅ STOP : "); Serial.println(angleActuel);
    }
  }
}

void setup() {
  Serial.begin(9600);
  wifiSerial.begin(9600); 
  
  pinMode(relais, OUTPUT);
  pinMode(moteurVitesse, OUTPUT);
  pinMode(moteurSens, OUTPUT);
  pinMode(encA, INPUT_PULLUP);
  pinMode(encB, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(encA), onEncodeur, CHANGE);
  
  Serial.println("\n--- BOOT SEQUENCING ---");
  
  // Test simple du module
  wifiSerial.println("AT");
  delay(500);
  if(wifiSerial.available()) Serial.println("1. WiFi OK");
  
  // Connexion forcée
  Serial.println("2. Connexion au partage...");
  wifiSerial.println("AT+CWJAP=\"" + String(ssid) + "\",\"" + String(pass) + "\"");
  delay(4000); // On laisse 4s pour se connecter
  
  Serial.println("3. Lancement de la boucle...");
}

void loop() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 3000) { // On interroge toutes les 3s
    lastUpdate = millis();
    interrogerAPI();
  }
}

void interrogerAPI() {
  Serial.println("-> Contact API...");
  
  // Timeout manuel pour ne pas bloquer l'Arduino
  wifiSerial.setTimeout(1000); 
  wifiSerial.println("AT+CIPSTART=\"TCP\",\"" + String(host) + "\"," + String(httpPort));
  
  if (wifiSerial.find("CONNECT")) {
    Serial.println("   Connecte ! Envoi requete...");
    
    String req = "GET /api/serre HTTP/1.1\r\nHost: " + String(host) + "\r\nConnection: close\r\n\r\n";
    wifiSerial.print("AT+CIPSEND=");
    wifiSerial.println(req.length());
    
    if (wifiSerial.find(">")) {
      wifiSerial.print(req);
      
      if (wifiSerial.find("{")) {
        String json = "{" + wifiSerial.readStringUntil('}');
        json += "}";
        
        StaticJsonDocument<256> doc;
        if (!deserializeJson(doc, json)) {
          digitalWrite(relais, doc["etatled"] ? HIGH : LOW);
          int angle = doc["angle_ouverture"];
          
          if (!moteurEnMouvement && angle != angleActuel) {
            angleCible = angle;
            impulsionCount = 0;
            digitalWrite(moteurSens, (angleCible > angleActuel) ? LOW : HIGH);
            analogWrite(moteurVitesse, 70); // Ta vitesse demandée
            moteurEnMouvement = true;
            Serial.print("🎬 MOTEUR ON -> "); Serial.println(angleCible);
          }
        }
      }
    }
  } else {
    Serial.println("   X Echec connection (Serveur eteint ?)");
  }
}