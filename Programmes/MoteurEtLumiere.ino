#include <ArduinoJson.h>

char ssid[] = "A35 de Tom";
char pass[] = "07122009ToM69";
const char* host = "10.200.153.38";
const int httpPort = 8000;

const int relais = 6;
const int moteurVitesse = 9;
const int moteurSens = 8;
const int encA = 21;
const int encB = 20;

const int buzzer = 22;
const int capteurLum = A0;

#define wifiSerial Serial1

unsigned long lastCycleTime = 0;
bool cycleActive = false;
bool manualControlAuthorized = false;

volatile long impulsionCount = 0;
const float PULSATIONS_PAR_DEGRE = 11.6;
int angleActuel = 0;
int angleCible = 0;
bool moteurEnMouvement = false;

const int SEUIL_LUMIERE = 300;
unsigned long lastLightCheckTime = 0;
const unsigned long INTERVAL_LIGHT_CHECK = 500;
bool lampeDevraitEtreAllumee = false;

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

void setLampe(bool allumee) {
  digitalWrite(relais, allumee ? HIGH : LOW);
  Serial.print("💡 Lampe : ");
  Serial.println(allumee ? "ALLUMEE" : "ETEINTE");
}

void startNewCycle() {
  setLampe(true);
  cycleActive = true;
  manualControlAuthorized = false;
  lampeDevraitEtreAllumee = true;
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
      lampeDevraitEtreAllumee = false;
      lastCycleTime = currentTime;
      Serial.println("Cycle: Phase ETEINTE (8s) - Controle manuel autorise");
    }
  } else {
    if (!moteurEnMouvement && elapsed >= 8000) {
      startNewCycle();
    }
  }
}

void gererCapteurEtBuzzer() {
  static unsigned long lastPrint = 0;
  unsigned long currentTime = millis();
  
  int valeurLumiere = analogRead(capteurLum);

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
  
  if (lampeDevraitEtreAllumee && valeurLumiere > SEUIL_LUMIERE) {
    digitalWrite(buzzer, HIGH);
    Serial.println("🔊 ALERTE - Obscurité détectée alors que lampe devrait être allumée !");
  } else {
    digitalWrite(buzzer, LOW);
  }
}

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

      if (!cycleActive && manualControlAuthorized) {
        bool etatLed = doc["etatled"];
        setLampe(etatLed);
        lampeDevraitEtreAllumee = etatLed;
        Serial.print("🎮 Commande manuelle lampe : ");
        Serial.println(etatLed ? "ALLUMEE" : "ETEINTE");
      } else if (cycleActive) {
        Serial.println("⚠️ Commande lampe ignorée - Cycle auto en cours");
      }

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
  gererCapteurEtBuzzer();
  
  if (millis() - lastAPICall > 200) {
    lastAPICall = millis();
    interrogerAPI();
  }
  
  delay(10);
}
