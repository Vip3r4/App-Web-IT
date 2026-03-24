/*const int button = 8;
const int relais = 3;
const int buzzer = 6;
const int capteurLum = 3;

int ancienEtat = 1;
int etatBouton;

bool mode = false;

void setup() {

  Serial.begin(9600);
  pinMode(button, INPUT_PULLUP);
  pinMode(relais, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(capteurLum, INPUT);
}

void loop() {
  etatBouton = digitalRead(button);

  if (ancienEtat == 1 && etatBouton == 0) {
    mode = !mode;
  }

  if (mode) {
    digitalWrite(relais, HIGH);
  } else {
    digitalWrite(relais , LOW);
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
}*/

const int button = 8;
const int relais = 3;
const int buzzer = 6;
const int capteurLum = 3;
unsigned long Temps;

int ancienEtat = 1;
int etatBouton;

bool mode = false;

void setup() {

  Serial.begin(9600);
  pinMode(button, INPUT_PULLUP);
  pinMode(relais, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(capteurLum, INPUT);
}

void loop() {
  etatBouton = digitalRead(button);

  Serial.print("Time: ");
  Temps = millis();
  Serial.println(Temps/1000);

  if (ancienEtat == 1 && etatBouton == 0) {
    mode = !mode;
  }

  if (mode || (Temps < 57600000)) {
    digitalWrite(relais, HIGH);
  } else if ((Temps > 57600000) || !mode) {
    digitalWrite(relais , LOW);
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