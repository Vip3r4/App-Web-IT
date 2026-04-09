
const int pinPotOuv = A0;   // Potentiomètre Ouverture
const int pinPotChauf = A1; // Potentiomètre Chauffage
const int pinTemp = A2;     // Capteur Température (Ex: LM35 ou TMP36)
const int pinLDR = A3;      // Capteur Lumière (pour vérifier l'éclairage)

const int pinBuzzer = 9;    // Alarme sonore
const int pin = 9;    // Alarme sonore
const int pin = 9;    // Alarme sonore
const int pin = 9;    // Alarme sonore

void setup() {
  Serial.begin(9600);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinLedR, OUTPUT);
  pinMode(pinLedJ, OUTPUT);
  pinMode(pinLedV, OUTPUT);
}

void loop() {
  int lectureTemp = analogRead(pinTemp);
  float voltage = lectureTemp * (5.0 / 1023.0);
  float temperature = (voltage - 0.5) * 100; // Formule pour TMP36

  int valeurPotOuv = analogRead(pinPotOuv);
  int angle = map(valeurPotOuv, 0, 1023, 5, 15); // Variable pr convertir l'analog en angle

  int lumiere = analogRead(pinLDR);
  bool alarme = (lumiere < 200); 

  if(alarme) {
    digitalWrite(pinBuzzer, HIGH);
    digitalWrite(pinLedR, HIGH);
    digitalWrite(pinLedV, LOW);
  } else {
    digitalWrite(pinBuzzer, LOW);
    digitalWrite(pinLedR, LOW);
    digitalWrite(pinLedV, HIGH);
  }

  String couleur = "Vert";
  if (alarme) couleur = "Rouge";
  else if (temperature > 28 || temperature < 14) couleur = "Jaune";


  Serial.print(temperature);
  Serial.print(",");
  Serial.print(angle);
  Serial.print(",");
  Serial.print(1);
  Serial.print(",");
  Serial.print(alarme ? 1 : 0);
  Serial.print(",");
  Serial.println(couleur);

  delay(2000);
}
