#include <ArduinoJson.h>
#include <SoftwareSerial.h>

char ssid[] = "A35 de Tom";
char pass[] = "07122009ToM69";
const char* host = "10.200.153.38";
const int httpPort = 8000;
const int relais = 4; 

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
}

void loop() {
  interrogerAPI();
}

void interrogerAPI() {

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
    
    Serial.print("JSON : ");
    Serial.println(jsonOnly);

    StaticJsonDocument<128> doc;
    if (!deserializeJson(doc, jsonOnly)) {
      bool etatLed = doc["etatled"]; 
      digitalWrite(relais, etatLed ? HIGH : LOW);
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