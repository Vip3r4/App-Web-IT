// Entrées
int potentioC = *PIN*;

// Sorties
int chauffage = *PIN*;

void setup() {

  pinMode(chauffage, OUTPUT);

  pinMode(potentioC, INPUT);

  // Console en Série pour TEST
  Serial.begin(9600);
  Serial.println("Systeme Chauffage demarré. Simulation en cours.");
}

void loop() {
  int EtatChauffage = DigitalRead(potentioC);
  int potentioPINC = analogRead(potentioC);

  if (potPinC >= 400 && potPinC <= 600) {

    digitalWrite(potentioPINC, HIGH);
    delay(250);
  }

     // Affichage du statut (pour TEST)
      if (potentioPINC >=0 ) {
        Serial.print("Potentio: ");
        Serial.println(potentioPINC);
      }
    }
  }
}