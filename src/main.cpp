#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <IMU.h>
#include <servo.h>

#define SERVO_PIN 19

void init_servo(int pin);
void init_adc();
void init_imu();
void init_bar();


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

void setup () {
  init_servo(SERVO_PIN); 
}

void loop () {}
