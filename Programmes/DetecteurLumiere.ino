const int button = 8;
const int relais = 3;
const int buzzer = 6;
const int capteurLum = 3;
unsigned long Temps;
unsigned long startTime = 0;

int ancienEtat = 1;
int etatBouton;

bool mode = false;

void setup() {

  Serial.begin(9600);
  pinMode(button, INPUT_PULLUP);
  pinMode(relais, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(capteurLum, INPUT);

  startTime = millis();
}

void loop() {
  etatBouton = digitalRead(button);
  Temps = millis() - startTime;

  Serial.print("Time: ");
  Serial.println(Temps/1000);

  if (ancienEtat == 1 && etatBouton == 0) {
    mode = !mode;
  }

  if (mode || (Temps < /*57600000*/ 16000)) {
    digitalWrite(relais, HIGH);
  } else if ((Temps > /*57600000*/ 8000) || !mode) {
    digitalWrite(relais , LOW);
  }

  if (Temps >= /*86400000*/ 24000)
  {
    startTime = millis();
  }

  ancienEtat = etatBouton;

  IsTurnedOn();
}

void IsTurnedOn() {

  int capteurValue = analogRead(capteurLum);

  if (mode) {
    if (capteurValue > 600) {
      digitalWrite(buzzer, HIGH);
    } else {
      digitalWrite(buzzer, LOW);
      return;
    }
  } else {
    return;
  }
  loop();
}
