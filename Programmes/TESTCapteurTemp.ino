// Entrées
int CapteurTmp = A1;

void setup() {

  pinMode(CapteurTmp, INPUT);

  Serial.begin(9600);
}

void loop() {
  int CapteurTmp1 = analogRead(CapteurTmp);

  if (CapteurTmp1 >= 0) {
    Serial.print("Temp: ");
    Serial.println(CapteurTmp1);
  }

  uint16_t tmp; // valeur codée en 16bits
    double data; // valeur codée en 32bits
    tmp=analogRead(CapteurTmp);
    data = (double) tmp * (5/10.24);
    Serial.print("Temp:");
    Serial.print(data);
    Serial.println("C");
}
