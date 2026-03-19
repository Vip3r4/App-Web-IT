const int button = 8;
const int ledG = 11;
const int ledR = 13;
const int buzzer = 6;
const int capteurLum = 3;

int ancienEtat = 1;
int etatBouton;

bool mode = false;

void setup() {

  Serial.begin(9600);
  pinMode(button, INPUT_PULLUP);
  pinMode(ledG, OUTPUT);
  pinMode(ledR, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(capteurLum, INPUT);
}

void loop() {
  etatBouton = digitalRead(button);

  if (ancienEtat == 1 && etatBouton == 0) {
    mode = !mode;
    delay(50);
  }

  if (mode) {
    digitalWrite(ledG, HIGH);
    digitalWrite(ledR, LOW);
  } else {
    digitalWrite(ledG, LOW);
    digitalWrite(ledR, HIGH);
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