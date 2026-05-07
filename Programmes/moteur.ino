#include <ArduinoJson.h>
#include <SoftwareSerial.h>

char ssid[] = "A35 de Tom";
char pass[] = "07122009ToM69";
const char* host = "10.200.153.38";
const int httpPort = 8000;
const int relais = 4;

// Moteur
const int moteurVitesse = 9;
const int moteurSens = 8;

// Variables angle
int angleActuel = 0;           // 0° = fermé
int angleCible = 0;
bool moteurEnMouvement = false;
unsigned long debutMouvement = 0;
int dureeMouvement = 0;

SoftwareSerial wifiSerial(2, 3);

// Durées pour chaque angle DEPUIS 0° (fermé) - Vitesse 50
const unsigned long dureeDepuisFerme[] = {
    0,     // 0°
    0, 0, 0, 0,
    50,    // 5°  -> 0.05s
    60,    // 6°  -> 0.06s
    70,    // 7°  -> 0.07s
    80,    // 8°  -> 0.08s
    90,    // 9°  -> 0.09s
    100,   // 10° -> 0.10s
    110,   // 11° -> 0.11s
    120,   // 12° -> 0.12s
    130,   // 13° -> 0.13s
    140,   // 14° -> 0.14s
    150,   // 15° -> 0.15s
    720    // 72° -> 0.72s
};

unsigned long getDureeDepuisFerme(int angle) {
    if (angle == 72) return 720;
    if (angle >= 5 && angle <= 15) return dureeDepuisFerme[angle];
    return 0;
}

void setup() {
    Serial.begin(9600);
    wifiSerial.begin(9600);
    
    pinMode(relais, OUTPUT);
    pinMode(moteurVitesse, OUTPUT);
    pinMode(moteurSens, OUTPUT);
    
    digitalWrite(relais, LOW);
    analogWrite(moteurVitesse, 0);
    digitalWrite(moteurSens, LOW);
    
    Serial.println("\n--- INITIALISATION ---");
    envoieAT("AT+RST", 2000);
    envoieAT("AT+CWMODE=1", 1000);
    
    String cmd = "AT+CWJAP=\"" + String(ssid) + "\",\"" + String(pass) + "\"";
    envoieAT(cmd, 8000);
    
    Serial.println("Systeme Pret - Vitesse moteur 50");
}

void loop() {
    gererMoteur();
    interrogerAPI();
    delay(10);
}

void gererMoteur() {
    if (moteurEnMouvement) {
        unsigned long elapsed = millis() - debutMouvement;
        if (elapsed >= dureeMouvement) {
            analogWrite(moteurVitesse, 0);
            digitalWrite(moteurSens, LOW);
            moteurEnMouvement = false;
            angleActuel = angleCible;
            
            Serial.print("✅ Moteur arrete - Position: ");
            Serial.print(angleActuel);
            Serial.println("°");
        }
    }
}

void demarrerMoteur(int angleDemande) {
    if (moteurEnMouvement) {
        Serial.println("⏳ Moteur deja en mouvement");
        return;
    }
    
    if (angleDemande == angleActuel) {
        Serial.println("ℹ️ Deja a cet angle");
        return;
    }
    
    // Calculer le temps depuis fermé pour chaque position
    unsigned long dureeActuelle = getDureeDepuisFerme(angleActuel);
    unsigned long dureeDemandee = getDureeDepuisFerme(angleDemande);
    
    // La différence = temps a parcourir
    if (angleDemande > angleActuel) {
        // OUVRIR - sens LOW
        digitalWrite(moteurSens, LOW);
        dureeMouvement = dureeDemandee - dureeActuelle;
        Serial.print("🔓 OUVERTURE de ");
    } else {
        // FERMER - sens HIGH
        digitalWrite(moteurSens, HIGH);
        dureeMouvement = dureeActuelle - dureeDemandee;
        Serial.print("🔒 FERMETURE de ");
    }
    
    analogWrite(moteurVitesse, 50);  // Vitesse 50
    
    debutMouvement = millis();
    moteurEnMouvement = true;
    angleCible = angleDemande;
    
    Serial.print(abs(angleDemande - angleActuel));
    Serial.print("° (");
    Serial.print(dureeMouvement);
    Serial.println(" ms)");
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
        
        StaticJsonDocument<256> doc;
        if (!deserializeJson(doc, jsonOnly)) {
            // LED
            bool etatLed = doc["etatled"];
            digitalWrite(relais, etatLed ? HIGH : LOW);
            Serial.print("💡 Lampe: ");
            Serial.println(etatLed ? "ALLUMEE" : "ETEINTE");
            
            // Moteur
            int nouvelAngle = doc["angle_ouverture"];
            if (nouvelAngle >= 5 && nouvelAngle <= 72) {
                demarrerMoteur(nouvelAngle);
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
