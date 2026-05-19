#include <ArduinoJson.h>

// --- CONFIGURATION ---
char ssid[] = "A35 de Tom";
char pass[] = "07122009ToM69";
const char* host = "10.200.153.38";
const int httpPort = 8000;

// --- PINS MEGA ---
const int relaisLampe = 6;        // Relais LAMPE (existant)
const int relaisChauffage = 5;    // Relais CHAUFFAGE (NOUVEAU - à ajouter)
const int moteurVitesse = 9;
const int moteurSens = 8;
const int encA = 21;
const int encB = 20;
const int buzzer = 22;
const int capteurLum = A0;        // Capteur lumière
const int capteurTemp = A1;       // Capteur TMP36

// --- PARAMÈTRES TEMPÉRATURE ---
float temperatureConsigne = 22.5;
const float HYSTERESIS = 1.0;
float temperatureActuelle = 0.0;
bool chauffageAllume = false;

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

// --- VARIABLES CAPTEUR LUMIERE ---
const int SEUIL_LUMIERE = 300;
bool lampeDevraitEtreAllumee = false;

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
      Serial.print("✅ Moteur arrêté - Angle : ");
      Serial.println(angleActuel);
    }
  }
}

// ===== TEMPÉRATURE =====
void lireTemperature() {
  int brute = analogRead(capteurTemp);
  float tension = brute * (5.0 / 1023.0);
  temperatureActuelle = (tension - 0.5) * 100.0;
}

void controlerTemperature() {
  lireTemperature();
  if (chauffageAllume) {
    if (temperatureActuelle >= temperatureConsigne) {
      digitalWrite(relaisChauffage, LOW);
      chauffageAllume = false;
      Serial.println("🌡️❄️ CHAUFFAGE ETEINT");
    }
  } else {
    if (temperatureActuelle <= (temperatureConsigne - HYSTERESIS)) {
      digitalWrite(relaisChauffage, HIGH);
      chauffageAllume = true;
      Serial.println("🌡️🔥 CHAUFFAGE ALLUME");
    }
  }
}

// ===== LAMPE =====
void setLampe(bool allumee) {
  digitalWrite(relaisLampe, allumee ? HIGH : LOW);
  Serial.print("💡 Lampe : ");
  Serial.println(allumee ? "ALLUMEE" : "ETEINTE");
}

void startNewCycle() {
  setLampe(true);
  cycleActive = true;
  manualControlAuthorized = false;
  lampeDevraitEtreAllumee = true;
  lastCycleTime = millis();
}

void gererCycleAuto() {
  if (cycleActive && (millis() - lastCycleTime >= 16000)) {
    setLampe(false);
    cycleActive = false;
    manualControlAuthorized = true;
    lampeDevraitEtreAllumee = false;
    lastCycleTime = millis();
  } else if (!cycleActive && !moteurEnMouvement && (millis() - lastCycleTime >= 8000)) {
    startNewCycle();
  }
}

// ===== BUZZER =====
void gererCapteurEtBuzzer() {
  int valeurLumiere = analogRead(capteurLum);
  if (lampeDevraitEtreAllumee && valeurLumiere > SEUIL_LUMIERE) {
    digitalWrite(buzzer, HIGH);
  } else {
    digitalWrite(buzzer, LOW);
  }
}

// ===== API =====
void interrogerAPI() {
  wifiSerial.println("AT+CIPSTART=\"TCP\",\"" + String(host) + "\"," + String(httpPort));
  if (!wifiSerial.find("CONNECT")) return;
  
  String httpRequest = "GET /api/serre HTTP/1.1\r\nHost: " + String(host) + "\r\nConnection: close\r\n\r\n";
  wifiSerial.print("AT+CIPSEND=");
  wifiSerial.println(httpRequest.length());
  if (wifiSerial.find(">")) wifiSerial.print(httpRequest);
  
  if (wifiSerial.find("{")) {
    String jsonOnly = "{" + wifiSerial.readStringUntil('}') + "}";
    StaticJsonDocument<512> doc;
    if (!deserializeJson(doc, jsonOnly)) {
      
      // Température
      if (doc.containsKey("consigne")) {
        temperatureConsigne = doc["consigne"];
      }
      
      // Lampe
      if (!cycleActive && manualControlAuthorized && doc.containsKey("etatled")) {
        setLampe(doc["etatled"]);
        lampeDevraitEtreAllumee = doc["etatled"];
      }
      
      // Moteur
      int angle = doc["angle_ouverture"];
      if (!moteurEnMouvement && angle != angleActuel) {
        angleCible = angle;
        impulsionCount = 0;
        digitalWrite(moteurSens, angleCible > angleActuel ? HIGH : LOW);
        analogWrite(moteurVitesse, 50);
        moteurEnMouvement = true;
      }
    }
  }
  wifiSerial.println("AT+CIPCLOSE");
}

// ===== SETUP =====
void setup() {
  Serial.begin(9600);
  wifiSerial.begin(9600);
  
  pinMode(relaisLampe, OUTPUT);
  pinMode(relaisChauffage, OUTPUT);
  pinMode(moteurVitesse, OUTPUT);
  pinMode(moteurSens, OUTPUT);
  pinMode(encA, INPUT_PULLUP);
  pinMode(encB, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(encA), onEncodeur, CHANGE);
  
  digitalWrite(relaisLampe, LOW);
  digitalWrite(relaisChauffage, LOW);
  analogWrite(moteurVitesse, 0);
  digitalWrite(buzzer, LOW);
  
  Serial.println("\n=== SYSTEME PRET ===");
  Serial.println("💡 Lampe sur pin 6 | 🌡️ Chauffage sur pin 5");
  
  wifiSerial.println("AT+CWJAP=\"" + String(ssid) + "\",\"" + String(pass) + "\"");
  delay(4000);
  
  startNewCycle();
}

// ===== LOOP =====
void loop() {
  static unsigned long lastAPI = 0, lastTemp = 0;
  
  gererCycleAuto();
  gererCapteurEtBuzzer();
  
  if (millis() - lastTemp >= 1000) {
    controlerTemperature();
    lastTemp = millis();
  }
  
  if (millis() - lastAPI >= 200) {
    lastAPI = millis();
    interrogerAPI();
  }
  
  delay(10);
}
