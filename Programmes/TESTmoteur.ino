
//////////////////////////////////////////////////////////
//  TESTMoteur.ino                                      //
//  PROGRAMME DE TEST, UTILISER POUR TEST UNIQUEMENT    //
//  NE PAS UTILISER POUR PRESENTATION                   //
//  Prog: Moteur                                        //
//////////////////////////////////////////////////////////




// Entrées
int potentioM = *PIN*;

// Sorties
int moteur = *PIN*;

void setup() {

  pinMode(moteur, OUTPUT);

  pinMode(potentioM, INPUT);

  // Console en Série pour TEST
  Serial.begin(9600);
  Serial.println("Systeme Moteur demarré. Simulation en cours.");
}

void loop() {
  int EtatMoteur = DigitalRead(potentioM);
  int potentioPINM = analogRead(potentioM);

  if (potPinM >= 400 && potPinM <= 600) {

    digitalWrite(potentioPINM, HIGH);
    delay(250);
  }

     // Affichage du statut (pour TEST)
      if (potentioPINM >=0 ) {
        Serial.print("Potentio: ");
        Serial.println(potentioPINM);
      }
    }
  }
}