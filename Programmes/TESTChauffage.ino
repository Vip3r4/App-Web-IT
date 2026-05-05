// Entrées
int capteurTemp = *PIN*;

// Sorties
int chauffage = *PIN*;

void setup() {
  pinMode(chauffage, OUTPUT);
  pinMode(potentioC, INPUT);
}

void loop() {
  int EtatChauffage = digitalRead(chauffage);
  int Temp = analogRead(capteurTemp);

  int lectureTemp = analogRead(capteurTemp);
  float voltage = lectureTemp * (5.0 / 1023.0);
  float temperature = (voltage - 0.5) * 100; // Formule pour TMP36
}
