#include <ArduinoJson.h>
#include <SoftwareSerial.h>

char ssid[] = "A35 de Tom";
char pass[] = "07122009ToM69";
const char* host = "10.200.153.38";
const int httpPort = 8000;
const int relais = 4; 

unsigned long lastCycleTime = 0;
bool cycleActive = false;
bool manualControlAuthorized = false;

SoftwareSerial wifiSerial(2, 3); 

void setup() {
  Serial.begin(9600);
  wifiSerial.begin(9600); 
  pinMode(relais, OUTPUT);
  digitalWrite(relais, LOW);

  Serial.println("\n--- INITIALISATION ---");
  envoieAT("AT+RST", 2000);
  envoieAT("AT+CWMODE=1", 1000);
  
  String cmd = "AT+CWJAP=\"" + String(ssid) + "\",\"" + String(pass) + "\"";
  envoieAT(cmd, 8000);
  
  Serial.println("Systeme Pret !");
  
  startNewCycle();
}

void loop() {
  gererCycleAuto();
  interrogerAPI();
  delay(100);
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
      Serial.println("Cycle: Phase ETEINTE (8s) - Controle manuel autorise");
    }
  } else {
    if (elapsed >= 8000) {
      startNewCycle();
    }
  }
}

void startNewCycle() {
  digitalWrite(relais, HIGH);
  cycleActive = true;
  manualControlAuthorized = false;
  lastCycleTime = millis();
  Serial.println("Cycle: Phase ALLUMEE (16s) - Controle manuel bloque");
}

void interrogerAPI() {
  if (cycleActive) {
    return;
  }
  
  wifiSerial.println("AT+CIPSTART=\"TCP\",\"" + String(host) + "\"," + String(httpPort));
  
  if (!wifiSerial.find("CONNECT")) {
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
    
    Serial.print("JSON recu : ");
    Serial.println(jsonOnly);

    StaticJsonDocument<128> doc;
    if (!deserializeJson(doc, jsonOnly)) {
      if (manualControlAuthorized) {
        bool etatLed = doc["etatled"];
        digitalWrite(relais, etatLed ? HIGH : LOW);
        Serial.print("Controle manuel applique - Lampe : ");
        Serial.println(etatLed ? "ALLUMEE" : "ETEINTE");
      } else {
        Serial.println("Controle manuel ignore - Phase allumee automatique en cours");
      }
    }
  }
  
  wifiSerial.println("AT+CIPCLOSE");
}

void envoieAT(String commande, int delai) {
  wifiSerial.println(commande);
  unsigned long start = millis();
  while (millis() - start < delai) {
    if (wifiSerial.available()) Serial.write(wifiSerial.read());
  }
}