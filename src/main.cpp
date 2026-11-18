#include <Arduino.h>
#include "freertos/task.h"
#include <imu.h>
#include <bar.h>
#include <servo.h>

#define SERVO_PIN 19

enum class Estado {
  Oscioso,
  Esperando_limiar,
  Controlando,
  Aquisição_pura,
};

enum class Evento {
  Inicio,
  Altitude,
  Apogeu,
  Aterrissagem,
};

void maqDeEstados(void *pvParameters);
void obterEvento(void *pvParameters);

IMU imu;
Bar bar;

void setup () {
  Serial.begin(9600);
  init_servo(SERVO_PIN); 
  imu.init(); // IMU imu = IMU::init()?
  bar.init();
}


void loop () {
  /* debugar data getters
 
  Serial.println("IMU:");
  Dados data = imu.leDados();

  for (int i = 0; i < 6; i++) {
  Serial.println(data.dados[i]);
  }
  imu.atualizaDados();  
  */
  Serial.println("Bar:");
  Dados data = imu.leDados();

  for (int i = 0; i < 2; i++) {
    Serial.println(data.dados[i]);
  }
  delay(200);
}
