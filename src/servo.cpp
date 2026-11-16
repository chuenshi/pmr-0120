#include <ESP32Servo.h>

Servo servo;

void init_servo(int pin) {
  servo.attach(pin); // escolhe a porta do servo

  for (int pos = 0; pos < 180; pos += 1) {
    servo.write(pos);
    delay(15);
  }
  for (int pos = 180; pos >= 90; pos -= 2) {
    servo.write(pos);
    delay(15);
  }
  // Sequência de inicialização/debug
}

void set_pos(int deg) {
  servo.write(deg);
}
