#include <ArduinoJson.h>
#include <SoftwareSerial.h>

char ssid[] = "A35 de Tom";
char pass[] = "07122009ToM69";
const char* host = "10.44.207.38";
const int httpPort = 8000;
const int relais = 4; 

SoftwareSerial wifiSerial(2, 3); 

void setup() {
  Serial.begin(9600);
  wifiSerial.begin(9600); 
  pinMode(relais, OUTPUT);
  digitalWrite(relais, LOW);

  Serial.println("\n--- INITIALISATION ---");
  envoieAT("AT+RST", 2000); // Reset propre du module
  envoieAT("AT+CWMODE=1", 1000);
  
  String cmd = "AT+CWJAP=\"" + String(ssid) + "\",\"" + String(pass) + "\"";
  envoieAT(cmd, 8000); // On laisse du temps pour le WiFi
  
  Serial.println("Systeme Pret !");
}

void loop() {
  interrogerAPI();
  delay(500); // On interroge toutes les 3 secondes
}

void interrogerAPI() {
  // 1. On s'assure que toute ancienne connexion est fermée
  wifiSerial.println("AT+CIPCLOSE"); 
  delay(200);

  // 2. Ouverture connexion TCP
  String startTCP = "AT+CIPSTART=\"TCP\",\"" + String(host) + "\"," + String(httpPort);
  wifiSerial.println(startTCP);
  
  // On attend que le module réponde "CONNECT" ou "OK"
  unsigned long start = millis();
  bool connected = false;
  while (millis() - start < 3000) {
    if (wifiSerial.find("CONNECT")) {
      connected = true;
      break;
    }
  }

  if (!connected) {
    Serial.println("Echec de connexion TCP au serveur.");
    return;
  }

  // 3. Envoi de la requête (On utilise "/" si ton API est à la racine)
  String httpRequest = "GET /api/serre HTTP/1.1\r\nHost: " + String(host) + "\r\nConnection: close\r\n\r\n";

  wifiSerial.print("AT+CIPSEND=");
  wifiSerial.println(httpRequest.length());
  delay(300); // Temps court pour recevoir le symbole '>'
  wifiSerial.print(httpRequest);

  // 4. Lecture et filtrage du JSON
  String payload = "";
  unsigned long timeout = millis();
  while (millis() - timeout < 3000) {
    if (wifiSerial.available()) {
      char c = wifiSerial.read();
      payload += c;
    }
  }

  // On extrait le JSON entre les premières et dernières accolades
  int jsonStart = payload.indexOf('{');
  int jsonEnd = payload.lastIndexOf('}');
  
  if (jsonStart != -1 && jsonEnd != -1) {
    String jsonOnly = payload.substring(jsonStart, jsonEnd + 1);
    Serial.print("JSON recu : ");
    Serial.println(jsonOnly);

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, jsonOnly);

    if (!error) {
      bool etatLed = doc["etatled"]; 
      digitalWrite(relais, etatLed ? HIGH : LOW);
      Serial.println(etatLed ? ">>> LED ALLUMÉE" : ">>> LED ÉTEINTE");
    } else {
      Serial.print("Erreur JSON : ");
      Serial.println(error.c_str());
    }
  } else {
    Serial.println("Pas de JSON valide dans la reponse.");
  }
}

void envoieAT(String commande, int delai) {
  wifiSerial.println(commande);
  unsigned long start = millis();
  while (millis() - start < delai) {
    if (wifiSerial.available()) Serial.write(wifiSerial.read());
  }
}
