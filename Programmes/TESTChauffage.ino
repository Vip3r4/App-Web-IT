
//////////////////////////////////////////////////////////
//  TESTChauffage.ino                                   //
//  PROGRAMME DE TEST, UTILISER POUR TEST UNIQUEMENT    //
//  NE PAS UTILISER POUR PRESENTATION                   //
//  Prog: Chauffage                                     //
//////////////////////////////////////////////////////////




// Entrées
int potentioC = *PIN*;

// Sorties
int chauffage = *PIN*;

void setup() {

  pinMode(chauffage, OUTPUT);

  pinMode(potentioC, INPUT);

  Serial.begin(9600);
}

void loop() {
  int EtatChauffage = DigitalRead(potentioC);
  int potentioPINC = analogRead(potentioC);

  if (potPinC >= 400 && potPinC <= 600) {

    digitalWrite(potentioPINC, HIGH);
    delay(250);
  }

      if (potentioPINC >=0 ) {
        Serial.print("Potentio: ");
        Serial.println(potentioPINC);
      }
    }
  }
}