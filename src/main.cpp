#include <Arduino.h>
#include "freertos/idf_additions.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "portmacro.h"
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
  Nenhum,
};

enum class Acao {
  A00, // Nada
  A01, // Transição para lançamento
  A02, // Inicio voo controlado
  A03, // Apogeu
  A04, // Pouso
};

void taskMaqDeEstados(void *pvParameters);
void taskObterEvento(void *pvParameters);

Estado state = Estado::Oscioso;
Evento evento = Evento::Nenhum;
Acao acao = Acao::A00;

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
  while (xQueue == nullptr) {}
  xTaskCreate(
              taskMaqDeEstados,
              "controlador pid",
              500,
              NULL,
              200,
              NULL
            );
  // taskCreate(taskObterEvento)
  // taskCreate(taskDetectFault)

}

void taskMaqDeEstados(void *_) { //pvParameters
  for (;;) {
    switch (state) {
    case Estado::Oscioso:
      // buzzer::bipbip()
      // button::verifica-inicio()
      // executar_acao(A01) 
      break;
    case Estado::Esperando_limiar:
      // if velociade < v_lim
      // 
      break;
    case Estado::Controlando:
      // algoritmo de controle
      
      break;
    case Estado::Fault:
      set_pos(0);
      // transicionar para aquisiçao
      break;
    case Estado::Aquisição_pura:
      break;
    }
  }
}

void taskObterEvento(void *pvParameters) {
  Evento codigoEvento;
  for (;;) {
    codigoEvento = Evento::Nenhum;
    
  
  }
}

 Evento executarAcao(Acao codigoAcao) {
   Evento out = Evento::Nenhum;

   switch (codigoAcao) {
   case Acao::A00:
     return out;

   case Acao::A01:

     break;

   case Acao::A02:

     break;

   case Acao::A03:

     break;

   case Acao::A04:

     break;
  
   }

   return out;
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
