#include <Arduino.h>
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "portmacro.h"
#include <imu.h>
#include <bar.h>
#include <servo.h>

#define SERVO_PIN 19

#define pi 3.14159265359
#define k_p 0.5

enum class Estado {
  Oscioso,
  EsperandoLimiar,
  Controlando,
  Fault,
  AquisicaoPura
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

void controlaVoo();

void taskMaqDeEstados(void *pvParameters);
void taskObterEvento(void *pvParameters);
void taskAtualizaVelocidade(void *pvParameters);

Estado state = Estado::Oscioso;
Evento evento = Evento::Nenhum;
Acao acao = Acao::A00;

QueueHandle_t xQueue;
TaskHandle_t xTaskMaqEstados, xTaskObterEvento, xTaskDetectaFault; 


SemaphoreHandle_t mutexV = NULL;

IMU imu;
Bar bar;

void setup () {
  Serial.begin(9600);


  init_servo(SERVO_PIN); 
  imu.init(); // IMU imu = IMU::init()?
  bar.init();

  mutexV = xSemaphoreCreateMutex();
  xQueue =  xQueueCreate(5, sizeof(Evento));

  while (xQueue == NULL || mutexV == NULL) {}
  /*
  xTaskCreate(
              taskMaqDeEstados,
              "controlador pid",
              500,
              NULL,
              200,
              NULL
            );
  */
  xTaskCreate(
              taskAtualizaVelocidade,
              "integracao velocidade",
              8192,
              (void*) &imu,
              20, //prioridade alta
              NULL
            );

  vTaskStartScheduler();
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
    case Estado::EsperandoLimiar:
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
    case Estado::AquisicaoPura:
      break;
    }
  }
}

void taskAtualizaVelocidade(void *pvParameters) {
  float velocidade[3] = {0};
  IMU* imu = (IMU*) pvParameters;

  int ultimoTickIMU = xTaskGetTickCount();

  for (;;) {
    if (xSemaphoreTake(mutexV, portMAX_DELAY)) { 
      float dt =
          (xTaskGetTickCount() - ultimoTickIMU) * portTICK_PERIOD_MS / 1e3f;
      Dados dados = imu->leDados();
      for (int i = 0; i < 3; i++) {
        velocidade[i] += dados.dados[i] * dt;
      }
      ultimoTickIMU = xTaskGetTickCount();
      for (float v : velocidade) {
        Serial.println(v);
      }
      
      xSemaphoreGive(mutexV);
    }

    vTaskDelay(pdMS_TO_TICKS(20));
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
     controlaVoo();
     break;

   case Acao::A03:

     break;

   case Acao::A04:

     break;
  
   }

   return out;
}

void controlaVoo() {
  // controle proporciol
  if (xSemaphoreTake(mutexV, portMAX_DELAY)) {
    float omega = imu.leDados().dados[5]; // no eixo z
    set_pos(-omega / pi * 180 * k_p);
    xSemaphoreGive(mutexV);
  }
}

void loop () {
  /* debugar data getters
 
  Serial.println("IMU:");
  Dados data = imu.leDados();

  for (int i = 0; i < 6; i++) {
  Serial.println(data.dados[i]);
  }
  imu.atualizaDados();  
  Serial.println("Bar:");
  Dados data = bar.leDados();

  for (int i = 0; i < 2; i++) {
    Serial.println(data.dados[i]);
  }
  bar.atualizaDados();
  delay(200);
  */
}
