#include <ESP32Servo.h>  // ersetzt <Servo.h> beim Arduino-Board

// Pin-Definitionen (anpassen bei Bedarf)
#define S0 26
#define S1 25
#define S2 33
#define S3 32
#define OUT 27
#define LED_PIN 14
#define SERVO_PIN 13

Servo myServo;  // Bleibt gleich, da API kompatibel

// Kalibrierbare Variablen
int posLeft = 0;    // Winkel für linke Bahn (anpassen, z.B. 0-180)
int posRight = 90;  // Winkel für rechte Bahn (anpassen, z.B. 0-180)
int colorThreshold = 100;  // Schwellwert für Farberkennung (höher = strenger; kalibrieren!)
int clearThreshold = 50;   // Schwellwert für farblos (niedrig = farblos; kalibrieren!)

// Variablen für RGB-Messungen
unsigned long redFreq = 0;
unsigned long greenFreq = 0;
unsigned long blueFreq = 0;

void setup() {
  // Sensor-Pins initialisieren
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Skalierung auf 100% setzen
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);

  // LED an
  digitalWrite(LED_PIN, HIGH);

  // Servo initialisieren und auf Default-Position setzen
  myServo.attach(SERVO_PIN);
  myServo.write(posLeft);  // Starte mit linker Bahn

  // Serielle Konsole für Debugging/Kalibrierung
  Serial.begin(115200);
  Serial.println("System gestartet. Kalibrierung starten? (Drücke 'c' für Kalibrierung)");

  // Optionale Kalibrierung aufrufen (manuell über Serial)
  calibrateIfNeeded();
}

void loop() {
  // RGB-Werte messen
  redFreq = readColor(LOW, LOW);    // Rot-Filter
  greenFreq = readColor(HIGH, HIGH); // Grün-Filter
  blueFreq = readColor(LOW, HIGH);   // Blau-Filter

  // Farbe klassifizieren
  if (isColored(redFreq, greenFreq, blueFreq)) {
    myServo.write(posLeft);  // Links für farbig
    Serial.println("Farbige Kugel erkannt -> Links");
  } else {
    myServo.write(posRight);  // Rechts für farblos
    Serial.println("Farblose Kugel erkannt -> Rechts");
  }

  // Wartezeit für Kugelpassage
  delay(500);
}

// Funktion zum Messen einer Farbfrequenz
unsigned long readColor(int s2, int s3) {
  digitalWrite(S2, s2);
  digitalWrite(S3, s3);
  return pulseIn(OUT, LOW);  // Frequenz messen (niedriger Wert = höhere Intensität)
}

// Funktion zur Farbklassifikation
bool isColored(unsigned long r, unsigned long g, unsigned long b) {
  // Farbig, wenn mindestens ein Kanal über Threshold
  if (r < colorThreshold || g < colorThreshold || b < colorThreshold) {
    return true;  // Farbig (niedrige Freq = hohe Intensität)
  }
  // Farblos, wenn alle Kanäle hoch (wenig Reflexion) und über clearThreshold
  if (r > clearThreshold && g > clearThreshold && b > clearThreshold) {
    return false;
  }
  return true;  // Default: Farbig
}

// Kalibrierungsfunktion (über Serial aufrufbar)
void calibrateIfNeeded() {
  if (Serial.available() > 0) {
    char input = Serial.read();
    if (input == 'c') {
      Serial.println("Kalibrierung startet. Lege rote Kugel unter Sensor und drücke Enter.");
      while (!Serial.available()) {}
      Serial.read();  // Enter abfangen
      redFreq = readColor(LOW, LOW);
      Serial.print("Roter Wert: "); Serial.println(redFreq);

      // Ähnlich für Blau, Grün, Farblos...
      // Beispiel: Schwellwerte anpassen
      colorThreshold = (redFreq + 50);  // Beispielanpassung
      Serial.println("Kalibrierung abgeschlossen. Neue Thresholds: ");
      Serial.print("Color: "); Serial.println(colorThreshold);
      Serial.print("Clear: "); Serial.println(clearThreshold);

      // Servo-Kalibrierung: Manuell anpassen
      Serial.println("Teste Servo: Drücke 'l' für links, 'r' für rechts.");
      while (true) {
        if (Serial.available()) {
          char cmd = Serial.read();
          if (cmd == 'l') myServo.write(posLeft);
          if (cmd == 'r') myServo.write(posRight);
          if (cmd == 's') break;  // Stop
        }
      }
      // Hier posLeft/posRight manuell im Code anpassen basierend auf Test
    }
  }
}