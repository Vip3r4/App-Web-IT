#include <ArduinoJson.h>

// --- CONFIGURATION ---
char ssid[] = "A35 de Tom";
char pass[] = "07122009ToM69";
const char* host = "10.200.153.38";
const int httpPort = 8000;

// --- PINS MEGA ---
const int relais = 6;          // Relais pour la lampe
const int moteurVitesse = 9;   // PWM pour vitesse moteur
const int moteurSens = 8;      // Direction moteur
const int encA = 21;           // Encodeur A
const int encB = 20;           // Encodeur B

// --- COMMUNICATION ---
#define wifiSerial Serial1      // WiFi sur Serial1 (TX=18, RX=19 sur Mega)

// --- VARIABLES LAMPE (cycle auto + manuel) ---
unsigned long lastCycleTime = 0;
bool cycleActive = false;
bool manualControlAuthorized = false;

// --- VARIABLES MOTEUR ---
volatile long impulsionCount = 0;
const float PULSATIONS_PAR_DEGRE = 11.6;
int angleActuel = 0;
int angleCible = 0;
bool moteurEnMouvement = false;

// --- INTERRUPTION ENCODEUR ---
void onEncodeur() {
  if (moteurEnMouvement) {
    if (digitalRead(encA) == digitalRead(encB)) {
      impulsionCount++;
    } else {
      impulsionCount--;
    }

    long cibleNecessaire = abs((long)(angleCible - angleActuel) * PULSATIONS_PAR_DEGRE);
    
    if (abs(impulsionCount) >= cibleNecessaire) {
      analogWrite(moteurVitesse, 0);
      moteurEnMouvement = false;
      angleActuel = angleCible;
      Serial.print("✅ Moteur arrêté - Angle atteint : ");
      Serial.println(angleActuel);
    }
  }
}

// --- FONCTIONS LAMPE ---
void startNewCycle() {
  digitalWrite(relais, HIGH);
  cycleActive = true;
  manualControlAuthorized = false;
  lastCycleTime = millis();
  Serial.println("Cycle: Phase ALLUMEE (16s) - Lampe auto ON, moteur actif");
}

void gererCycleAuto() {
  unsigned long currentTime = millis();
  unsigned long elapsed = currentTime - lastCycleTime;
  
  if (cycleActive) {
    if (elapsed >= 16000) {
      digitalWrite(relais, LOW);
      cycleActive = false;
      manualControlAuthorized = true;
      lastCycleTime = currentTime;
      Serial.println("Cycle: Phase ETEINTE (8s) - Controle lampe manuel autorise");
    }
  } else {
    if (!moteurEnMouvement && elapsed >= 8000) {
      startNewCycle();
    }
  }
}

// --- FONCTIONS COMMUNICATION ---
void envoieAT(String commande, int delai) {
  wifiSerial.println(commande);
  unsigned long start = millis();
  while (millis() - start < delai) {
    if (wifiSerial.available()) {
      Serial.write(wifiSerial.read());
    }
  }
}

void interrogerAPI() {
  // PLUS BLOQUANT ! On interroge TOUJOURS l'API
  // Seule l'application de la commande lampe est conditionnée
  
  Serial.println("\n--- Interrogation API ---");
  
  wifiSerial.println("AT+CIPSTART=\"TCP\",\"" + String(host) + "\"," + String(httpPort));
  
  if (!wifiSerial.find("CONNECT")) {
    Serial.println("❌ Échec connexion au serveur");
    return;
  }
  
  String httpRequest = "GET /api/serre HTTP/1.1\r\nHost: " + String(host) + "\r\nConnection: close\r\n\r\n";
  
  wifiSerial.print("AT+CIPSEND=");
  wifiSerial.println(httpRequest.length());
  
  if (wifiSerial.find(">")) {
    wifiSerial.print(httpRequest);
  }
  
  if (wifiSerial.find("{")) {
    String jsonOnly = "{" + wifiSerial.readStringUntil('}');
    jsonOnly += "}";
    
    Serial.print("📦 JSON reçu : ");
    Serial.println(jsonOnly);
    
    StaticJsonDocument<256> doc;
    if (!deserializeJson(doc, jsonOnly)) {
      
      // --- GESTION DE LA LAMPE (condition modifiée) ---
      // On applique la lampe UNIQUEMENT si on n'est PAS en cycle auto
      if (!cycleActive && manualControlAuthorized) {
        bool etatLed = doc["etatled"];
        digitalWrite(relais, etatLed ? HIGH : LOW);
        Serial.print("💡 Lampe (manuel) : ");
        Serial.println(etatLed ? "ALLUMEE" : "ETEINTE");
      } else if (cycleActive) {
        Serial.println("⚠️ Lampe ignoree - Cycle auto en cours");
      }
      
      // --- GESTION DU MOTEUR (TOUJOURS EXECUTEE, MEME PENDANT CYCLE LAMPE) ---
      int angle = doc["angle_ouverture"];
      
      if (!moteurEnMouvement && angle != angleActuel) {
        angleCible = angle;
        impulsionCount = 0;
        
        // Définir la direction
        if (angleCible > angleActuel) {
          digitalWrite(moteurSens, LOW);  // Un sens
        } else {
          digitalWrite(moteurSens, HIGH); // Autre sens
        }
        
        analogWrite(moteurVitesse, 50);
        moteurEnMouvement = true;
        Serial.print("🎬 MOTEUR ACTIF -> Angle cible : ");
        Serial.println(angleCible);
      } else if (moteurEnMouvement) {
        Serial.println("⏳ Moteur deja en mouvement...");
      } else if (angle == angleActuel) {
        Serial.println("✓ Angle deja atteint");
      }
    } else {
      Serial.println("❌ Erreur parsing JSON");
    }
  }
  
  wifiSerial.println("AT+CIPCLOSE");
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
  
  analogWrite(moteurVitesse, 0);
  digitalWrite(relais, LOW);
  
  Serial.println("\n=== INITIALISATION ===");
  
  wifiSerial.println("AT");
  delay(500);
  if(wifiSerial.available()) Serial.println("1. WiFi OK");
  
  Serial.println("2. Connexion au WiFi...");
  wifiSerial.println("AT+CWJAP=\"" + String(ssid) + "\",\"" + String(pass) + "\"");
  delay(4000);
  
  Serial.println("✅ Système prêt !");
  
  startNewCycle();
}

void loop() {
  static unsigned long lastAPICall = 0;
  
  gererCycleAuto();
  
  if (millis() - lastAPICall > 200) {
    lastAPICall = millis();
    interrogerAPI();
  }
  
  delay(10);
}