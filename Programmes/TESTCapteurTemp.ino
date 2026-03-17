// Entrées
int CapteurTmp = A1;

void setup() {

  pinMode(CapteurTmp, INPUT);

  Serial.begin(9600);
}

void loop() {
  int CapteurTmp1 = analogRead(CapteurTmp);

      if (CapteurTmp1 >=0 ) {
        Serial.print("Temp: ");
        Serial.println(CapteurTmp1);
      }
  }
}