#include <Arduino.h> 

const int PIN_CAPTEUR_TEMP = A2;        // Broche du capteur TMP36
const int PIN_RELAIS_CHAUFFAGE = 2;     // Broche du relais
const int PIN_LED_TEMOIN = 13;          // LED témoin (optionnel)

float TEMPERATURE_CONSIGNE = 22.5;      // Température souhaitée en °C
float MARGE_HYSTERESIS = 1.0;           // Marge avant réallumage (ex: 1°C)

float temperatureActuelle = 0.0;
bool chauffageAllume = false;
unsigned long dernierAffichage = 0;
const unsigned long INTERVALLE_AFFICHAGE = 1000;  // Affiche toutes les 1 seconde

void setup() {
  Serial.begin(9600);
  
  pinMode(PIN_RELAIS_CHAUFFAGE, OUTPUT);
  pinMode(PIN_LED_TEMOIN, OUTPUT);
  
  digitalWrite(PIN_RELAIS_CHAUFFAGE, LOW);
  digitalWrite(PIN_LED_TEMOIN, LOW);
  
  Serial.println("======================================");
  Serial.println("SYSTEME DE CONTROLE DE TEMPERATURE");
  Serial.println("Capteur: TMP36");
  Serial.println("======================================");
  Serial.print("Temperature cible: ");
  Serial.print(TEMPERATURE_CONSIGNE);
  Serial.println(" °C");
  Serial.print("Hysteresis: +/- ");
  Serial.print(MARGE_HYSTERESIS);
  Serial.println(" °C");
  Serial.println("--------------------------------------");
}

void loop() {
  lireCapteurTemperature();
  
  controlerTemperature();
  
  if (millis() - dernierAffichage >= INTERVALLE_AFFICHAGE) {
    afficherEtat();
    dernierAffichage = millis();
  }
  
  delay(100);
}

void lireCapteurTemperature() {
  int lectureBrute = analogRead(PIN_CAPTEUR_TEMP);
  
  // Conversion en tension (0-5V)
  float tension = lectureBrute * (5.0 / 1023.0);
  
  // Conversion en température pour TMP36
  // TMP36 donne 0.5V à 0°C, +10mV par °C
  temperatureActuelle = (tension - 0.5) * 100.0;
  
}

// ===== CONTRÔLE DU CHAUFFAGE (avec hystérésis) =====
void controlerTemperature() {
  if (chauffageAllume) {
    // Si chauffage allumé, on éteint quand on dépasse la consigne
    if (temperatureActuelle >= TEMPERATURE_CONSIGNE) {
      eteindreChauffage();
    }
  } else {
    // Si chauffage éteint, on allume quand on est en dessous de (consigne - marge)
    if (temperatureActuelle <= (TEMPERATURE_CONSIGNE - MARGE_HYSTERESIS)) {
      allumerChauffage();
    }
  }
}

void allumerChauffage() {
  digitalWrite(PIN_RELAIS_CHAUFFAGE, HIGH);
  digitalWrite(PIN_LED_TEMOIN, HIGH);
  chauffageAllume = true;
  Serial.print("[ACTION] ");
  Serial.print(millis() / 1000);
  Serial.println("s - CHAUFFAGE ALLUME");
}

void eteindreChauffage() {
  digitalWrite(PIN_RELAIS_CHAUFFAGE, LOW);
  digitalWrite(PIN_LED_TEMOIN, LOW);
  chauffageAllume = false;
  Serial.print("[ACTION] ");
  Serial.print(millis() / 1000);
  Serial.println("s - CHAUFFAGE ETEINT");
}

void afficherEtat() {
  Serial.print("Temp: ");
  Serial.print(temperatureActuelle, 1);  // 1 décimale
  Serial.print("°C | Consigne: ");
  Serial.print(TEMPERATURE_CONSIGNE, 1);
  Serial.print("°C | Chauffage: ");
  Serial.println(chauffageAllume ? "ON " : "OFF");
}

void changerConsigne(float nouvelleConsigne) {
  if (nouvelleConsigne >= 0 && nouvelleConsigne <= 50) {
    TEMPERATURE_CONSIGNE = nouvelleConsigne;
    Serial.print("[CONSIGNE] Nouvelle cible: ");
    Serial.print(TEMPERATURE_CONSIGNE);
    Serial.println(" °C");
  }
}

float getTemperature() {
  return temperatureActuelle;
}

bool getEtatChauffage() {
  return chauffageAllume;
}
