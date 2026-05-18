const int PIN_CAPTEUR_TEMP = A0;    // Capteur TMP36 (ANALOGIQUE)
const int PIN_RELAIS = 4;            // Relais (DIGITAL)

const float CONSIGNE = 22.5;
const float HYSTERESIS = 1.0;


float temperatureActuelle = 0.0;
bool chauffageAllume = false;
unsigned long dernierAffichage = 0;
unsigned long dernierAllumage = 0;
const unsigned long DELAI_MINIMUM = 5000;

void setup() {
  Serial.begin(9600);
  
  pinMode(PIN_RELAIS, OUTPUT);
  digitalWrite(PIN_RELAIS, LOW);
  
  Serial.println("==========================================");
  Serial.println("CONTROLE TEMPERATURE - SYSTEME PRET");
  Serial.println("==========================================");
  Serial.print("Consigne: ");
  Serial.print(CONSIGNE);
  Serial.println(" °C");
  Serial.print("Hysteresis: +/- ");
  Serial.print(HYSTERESIS);
  Serial.println(" °C");
  Serial.println("---");
  Serial.println("Test relais...");
  
  digitalWrite(PIN_RELAIS, HIGH);
  delay(500);
  digitalWrite(PIN_RELAIS, LOW);
  delay(500);
  
  Serial.println("Test termine. Debut du controle.");
  Serial.println("==========================================");
}

void loop() {
  lireTemperature();
  
  controlerTemperature();
  
  afficherInfos();
  
  delay(100);
}

void lireTemperature() {
  int lectureBrute = analogRead(PIN_CAPTEUR_TEMP);
  float tension = lectureBrute * (5.0 / 1023.0);
  temperatureActuelle = (tension - 0.5) * 100.0;
}

void controlerTemperature() {
  if (millis() - dernierAllumage < DELAI_MINIMUM && chauffageAllume == false) {
    return;
  }
  
  if (chauffageAllume) {
    if (temperatureActuelle >= CONSIGNE) {
      eteindreChauffage();
    }
  } else {
    if (temperatureActuelle <= (CONSIGNE - HYSTERESIS)) {
      allumerChauffage();
    }
  }
}

void allumerChauffage() {
  digitalWrite(PIN_RELAIS, HIGH);
  chauffageAllume = true;
  dernierAllumage = millis();
  
  Serial.print("[");
  Serial.print(millis() / 1000);
  Serial.println("s] 🔥🔥🔥 CHAUFFAGE ALLUME 🔥🔥🔥");
}

void eteindreChauffage() {
  digitalWrite(PIN_RELAIS, LOW);
  chauffageAllume = false;
  
  Serial.print("[");
  Serial.print(millis() / 1000);
  Serial.println("s] ❄️❄️❄️ CHAUFFAGE ETEINT ❄️❄️❄️");
}

void afficherInfos() {
  if (millis() - dernierAffichage >= 1000) {
    Serial.print("Temp: ");
    Serial.print(temperatureActuelle, 1);
    Serial.print("°C | Consigne: ");
    Serial.print(CONSIGNE, 1);
    Serial.print("°C | Chauffage: ");
    Serial.print(chauffageAllume ? "ON " : "OFF");
    
    Serial.print(" | Relais: ");
    Serial.print(digitalRead(PIN_RELAIS) ? "HIGH" : "LOW");
    
    float tension = analogRead(PIN_CAPTEUR_TEMP) * (5.0 / 1023.0);
    Serial.print(" | Tension: ");
    Serial.print(tension, 2);
    Serial.println("V");
    
    dernierAffichage = millis();
  }
}

void changerConsigne(float nouvelleConsigne) {
  if (nouvelleConsigne >= 10.0 && nouvelleConsigne <= 40.0) {
    Serial.print("Nouvelle consigne demandee: ");
    Serial.print(nouvelleConsigne);
    Serial.println(" °C");
  }
}

float getTemperature() {
  return temperatureActuelle;
}

bool getEtatChauffage() {
  return chauffageAllume;
}
