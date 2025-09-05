#include <Servo.h>

// TCS3200 Pin-Definitionen
#define S0 18
#define S1 19
#define S2 21
#define S3 22
#define OUT 23

// SG90 Servo
Servo weichenServo;
#define SERVO_PIN 13

// Variablen für Farbmessung
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;

void setup() {
  // TCS3200 Setup
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  
  // Frequenzskalierung auf 20% setzen
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // Servo Setup
  weichenServo.attach(SERVO_PIN);
  weichenServo.write(90); // Startposition der Weiche

  Serial.begin(115200);
}

void loop() {
  // Rot messen
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  delay(100);
  redFrequency = pulseIn(OUT, LOW);

  // Grün messen
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  delay(100);
  greenFrequency = pulseIn(OUT, LOW);

  // Blau messen
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  delay(100);
  blueFrequency = pulseIn(OUT, LOW);

  // Farbe bestimmen (einfache Schwellenwerte, Kalibrierung nötig)
  if (redFrequency < greenFrequency && redFrequency < blueFrequency) {
    Serial.println("Rot erkannt");
    weichenServo.write(45); // Weiche auf Position 1 (z. B. links)
  } else if (greenFrequency < redFrequency && greenFrequency < blueFrequency) {
    Serial.println("Grün erkannt");
    weichenServo.write(90); // Weiche auf Position 2 (z. B. Mitte)
  } else {
    Serial.println("Blau erkannt");
    weichenServo.write(135); // Weiche auf Position 3 (z. B. rechts)
  }

  delay(500); // Wartezeit für nächste Messung
}