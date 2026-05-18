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

// --- NOUVEAUX PINS ---
const int buzzer = 22;         // Buzzer
const int capteurLum = A0;     // Capteur de lumière analogique

// --- COMMUNICATION ---
#define wifiSerial Serial1

// --- VARIABLES LAMPE ---
unsigned long lastCycleTime = 0;
bool cycleActive = false;
bool manualControlAuthorized = false;

// --- VARIABLES MOTEUR ---
volatile long impulsionCount = 0;
const float PULSATIONS_PAR_DEGRE = 11.6;
int angleActuel = 0;
int angleCible = 0;
bool moteurEnMouvement = false;

// --- VARIABLES CAPTEUR LUMIERE & BUZZER ---
const int SEUIL_LUMIERE = 300;        // Seuil d'obscurité (à ajuster)
unsigned long lastLightCheckTime = 0;
const unsigned long INTERVAL_LIGHT_CHECK = 500;  // Vérifie toutes les 500ms
bool lampeDevraitEtreAllumee = false;  // La lampe DOIT être allumée selon consigne

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
void setLampe(bool allumee) {
  digitalWrite(relais, allumee ? HIGH : LOW);
  Serial.print("💡 Lampe : ");
  Serial.println(allumee ? "ALLUMEE" : "ETEINTE");
}

void startNewCycle() {
  setLampe(true);
  cycleActive = true;
  manualControlAuthorized = false;
  lampeDevraitEtreAllumee = true;   // Pendant cycle auto, lampe DOIT être allumée
  lastCycleTime = millis();
  Serial.println("Cycle: Phase ALLUMEE (16s)");
}

void gererCycleAuto() {
  unsigned long currentTime = millis();
  unsigned long elapsed = currentTime - lastCycleTime;
  
  if (cycleActive) {
    if (elapsed >= 16000) {
      setLampe(false);
      cycleActive = false;
      manualControlAuthorized = true;
      lampeDevraitEtreAllumee = false;  // Lampe DOIT être éteinte
      lastCycleTime = currentTime;
      Serial.println("Cycle: Phase ETEINTE (8s) - Controle manuel autorise");
    }
  } else {
    if (!moteurEnMouvement && elapsed >= 8000) {
      startNewCycle();
    }
  }
}

// --- FONCTION GESTION CAPTEUR + BUZZER ---
void gererCapteurEtBuzzer() {
  static unsigned long lastPrint = 0;
  unsigned long currentTime = millis();
  
  int valeurLumiere = analogRead(capteurLum);
  
  // Affiche la valeur toutes les secondes pour le debug
  if (currentTime - lastPrint >= 1000) {
    lastPrint = currentTime;
    Serial.print("📊 Capteur = ");
    Serial.print(valeurLumiere);
    Serial.print(" | Seuil = ");
    Serial.print(SEUIL_LUMIERE);
    Serial.print(" | Lampe doit être = ");
    Serial.print(lampeDevraitEtreAllumee ? "ALLUMEE" : "ETEINTE");
    Serial.print(" | Détection = ");
    Serial.println(valeurLumiere > SEUIL_LUMIERE ? "SOMBRE" : "CLAIR");
  }
  
  // LOGIQUE BUZZER CORRIGÉE :
  // SI lampe DOIT être allumée ET capteur > seuil (obscurité) → buzzer ON
  // SINON → buzzer OFF
  if (lampeDevraitEtreAllumee && valeurLumiere > SEUIL_LUMIERE) {
    digitalWrite(buzzer, HIGH);
    Serial.println("🔊 ALERTE - Obscurité détectée alors que lampe devrait être allumée !");
  } else {
    digitalWrite(buzzer, LOW);
  }
}

// --- FONCTIONS COMMUNICATION ---
void interrogerAPI() {
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
      
      // --- GESTION DE LA LAMPE (commande manuelle) ---
      if (!cycleActive && manualControlAuthorized) {
        bool etatLed = doc["etatled"];
        setLampe(etatLed);
        lampeDevraitEtreAllumee = etatLed;  // Met à jour ce que la lampe DOIT être
        Serial.print("🎮 Commande manuelle lampe : ");
        Serial.println(etatLed ? "ALLUMEE" : "ETEINTE");
      } else if (cycleActive) {
        Serial.println("⚠️ Commande lampe ignorée - Cycle auto en cours");
      }
      
      // --- GESTION DU MOTEUR ---
      int angle = doc["angle_ouverture"];
      
      if (!moteurEnMouvement && angle != angleActuel) {
        angleCible = angle;
        impulsionCount = 0;
        
        if (angleCible > angleActuel) {
          digitalWrite(moteurSens, HIGH);
        } else {
          digitalWrite(moteurSens, LOW);
        }
        
        analogWrite(moteurVitesse, 50);
        moteurEnMouvement = true;
        Serial.print("🎬 MOTEUR -> Angle cible : ");
        Serial.println(angleCible);
      } else if (moteurEnMouvement) {
        Serial.println("⏳ Moteur déjà en mouvement...");
      } else if (angle == angleActuel) {
        Serial.println("✓ Angle déjà atteint");
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
  pinMode(buzzer, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(encA), onEncodeur, CHANGE);
  
  analogWrite(moteurVitesse, 0);
  setLampe(false);
  digitalWrite(buzzer, LOW);
  
  Serial.println("\n=== INITIALISATION ===");
  Serial.println("🔧 Détection lampe défaillante");
  Serial.println("   Le buzzer sonne en CONTINU si :");
  Serial.println("   - La lampe DOIT être allumée (cycle ou commande)");
  Serial.println("   - Mais le capteur ne détecte PAS de lumière");
  
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
  gererCapteurEtBuzzer();  // Vérifie lumière + buzzer continu
  
  if (millis() - lastAPICall > 200) {
    lastAPICall = millis();
    interrogerAPI();
  }
  
  delay(10);
}