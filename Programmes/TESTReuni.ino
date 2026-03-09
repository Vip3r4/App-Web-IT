
//////////////////////////////////////////////////////////
//  TESTReuni.ino                                       //
//  PROGRAMME DE TEST, UTILISER POUR TEST UNIQUEMENT    //
//  NE PAS UTILISER POUR PRESENTATION                   //
//  Progs: Chauffage et Moteur                          //
//////////////////////////////////////////////////////////




// Entrées
int potentioC = *PIN*;
int potentioM = *PIN*;

// Sorties
int chauffage = *PIN*;
int moteur = *PIN*;

void setup() {

  pinMode(chauffage, OUTPUT);
  pinMode(potentioC, INPUT);

  pinMode(moteur, OUTPUT);
  pinMode(potentioM, INPUT);

  // Console en Série pour TEST
  Serial.begin(9600);
  Serial.println("Marche");
}

void loop() {
  int EtatChauffage = DigitalRead(potentioC);
  int potentioPINC = analogRead(potentioC);

  int EtatMoteur = DigitalRead(potentioM);
  int potentioPINM = analogRead(potentioM);

  if (potPinC >= 400 && potPinC <= 600) {

    digitalWrite(potentioPINC, HIGH);
    delay(250);
  }

  if (potPinM >= 400 && potPinM <= 600) {

    digitalWrite(potentioPINM, HIGH);
    delay(250);
  }

     // Affichage du statut (pour TEST)
      if (potentioPINC >=0 ) {
        Serial.print("Potentio: ");
        Serial.println(potentioPINC);
      }

      if (potentioPINM >=0 ) {
        Serial.print("Potentio: ");
        Serial.println(potentioPINM);
      }
    }
  }
}