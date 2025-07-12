#include <Servo.h>
#include <SoftwareSerial.h>

SoftwareSerial Serial1(2, 3); // RX, TX

Servo sg90;
int servoPin = 9;
int position = 115;      // Position initiale du servo
int seuil = 150;        // Seuil pour déclencher mouvement
int zoneMorte = 10;     // Petite marge d'inactivité autour du seuil

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  sg90.attach(servoPin);
  sg90.write(position);  // Position initiale

  Serial.println("Systeme prêt.");
}

void loop() {
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    data.trim();

    int sep = data.indexOf(',');
    if (sep > 0 && sep < data.length() - 1) {
      int r1 = data.substring(0, sep).toInt();
      int r2 = data.substring(sep + 1).toInt();

      
      if (r1 == 0 && r2 == 0) return;

      int diff = abs(r1 - r2);

      
      if (diff > seuil + zoneMorte) {
        if (r1 > r2) position -= 2;
        else position += 2;

        position = constrain(position, 75, 145);
        sg90.write(position);

      } else {
        
      }

      
    }
  }
}
