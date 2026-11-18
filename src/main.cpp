#include <Arduino.h>
#include "freertos/task.h"
#include <imu.h>
#include <bar.h>
#include <servo.h>

#define SERVO_PIN 19

void init_servo(int pin);
/*
void init_IMU();
void init_bar();
void init_sd();
*/

float* leDadosIMU();
float* leDadosBar();

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
  float* dados = leDadosIMU();
  for (int i = 0; i < 6; i++) {
    Serial.println(dados[i]);
  }
  */
  for (int i = 0; i < 6; i++) {
    Serial.println("IMU:");
    printf("%f", (imu.leDados(imu)).dados[i]);
    
  }

  for (int i = 0; i < 2; i++) {
    Serial.println("Bar:");
    printf("%f", (bar.leDados(bar)).dados[i]);
  }
  imu.atualizaDados(imu);
  bar.atualizaDados(bar);
  
  delay(200);
}
