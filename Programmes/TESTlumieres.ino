// Entrées
int potentio = **;

// Sorties
int moteur = **;

void setup() {

  pinMode(moteur, OUTPUT);

  pinMode(potentio, INPUT);

  // Console en Série pour TEST
  Serial.begin(9600);
  Serial.println("Systeme Moteur demarré. Simulation en cours.");
}

void loop() {
  int EtatMoteur = DigitalRead(potentio);
  int potentioPIN = analogRead(potentio);

  if (potPin >= 400 && potPin <= 600) {

    digitalWrite(potentioPIN, HIGH);
    delay(250);
  }

     // Affichage du statut (pour TEST)
      if (potentioPIN >=0 ) {
        Serial.print("Potentio: ");
        Serial.println(potentioPIN);
      }
    }
  }
}