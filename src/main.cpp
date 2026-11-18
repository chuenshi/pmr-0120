#include <Arduino.h>
#include "freertos/idf_additions.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <imu.h>
#include <bar.h>
#include <servo.h>

#define SERVO_PIN 19

enum class Estado {
  Oscioso,
  Esperando_limiar,
  Controlando,
  Fault,
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

QueueHandle_t xQueue;
TaskHandle_t xTaskMaqEstados, xTaskObterEvento, xTaskDetectaFault; 

IMU imu;
Bar bar;

void setup () {
  Serial.begin(9600);
  init_servo(SERVO_PIN); 
  imu.init(); // IMU imu = IMU::init()?
  bar.init();
  xQueue =  xQueueCreate(5, sizeof(Evento));
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
  Dados data = bar.leDados();

  for (int i = 0; i < 2; i++) {
    Serial.println(data.dados[i]);
  }
  bar.atualizaDados();
  delay(200);
}
