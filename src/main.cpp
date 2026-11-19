#include <Arduino.h>
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include <imu.h>
#include <bar.h>
#include <servo.h>
#include <buzzer.h>

#define SERVO_PIN 19

constexpr float pi = 3.14159265359f;
constexpr float k_p = 0.5f;
constexpr float k_i = 0.05f;
constexpr float k_d = 0.01f;

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
  Nenhum
};

enum class Acao {
  A00, // Nada
  A01, // Transição para lançamento
  A02, // Início voo controlado
  A03, // Apogeu
  A04  // Pouso
};

struct Transicao {
  Estado proximoEstado;
  Acao acao;
};

const Transicao matrizTransicao[5][4] = {
       // Eventos:        Inicio                                  Altitude                               Apogeu                                 Aterrissagem
      /* Oscioso */      {{Estado::EsperandoLimiar, Acao::A01},   {Estado::Oscioso, Acao::A00},          {Estado::Oscioso, Acao::A00},         {Estado::Oscioso, Acao::A00}},
/* EsperandoLimiar */    {{Estado::EsperandoLimiar, Acao::A00},   {Estado::Controlando, Acao::A02},      {Estado::Fault, Acao::A00},           {Estado::Oscioso, Acao::A00}},
      /* Controlando */  {{Estado::Controlando, Acao::A00},       {Estado::Controlando, Acao::A00},      {Estado::AquisicaoPura, Acao::A03},   {Estado::Fault, Acao::A00}},
      /* Fault */        {{Estado::Fault, Acao::A00},             {Estado::Fault, Acao::A00},            {Estado::AquisicaoPura, Acao::A03},   {Estado::Oscioso, Acao::A04}},
      /* AquisicaoPura */{{Estado::AquisicaoPura, Acao::A00},     {Estado::AquisicaoPura, Acao::A00},    {Estado::AquisicaoPura, Acao::A00},   {Estado::Oscioso, Acao::A04}}
};

// Protótipos
void controlaVoo();
void taskMaqDeEstados(void *pvParameters);
void taskObterEvento(void *pvParameters);
void taskAtualizaVelocidade(void *pvParameters);
Evento executarAcao(Acao codigoAcao);

// Variáveis globais de estado e sincronização
Estado state = Estado::Oscioso;

QueueHandle_t xQueue = NULL;
TaskHandle_t xTaskMaqEstados = NULL;
TaskHandle_t xTaskObterEvento = NULL;
TaskHandle_t xTaskVelocidade = NULL;
SemaphoreHandle_t mutexV = NULL;

float velocidade[3] = {0.0f, 0.0f, 0.0f};
float offsetAcel[3] = {0.0f, 0.0f, 0.0f};

// Instâncias dos periféricos
IMU imu;
Bar bar;
Buzzer buzzer;

void setup() {
  Serial.begin(115200);
  buzzer.init();
  // teste do buzzer
  buzzer.ligar();
  delay(1000);
  buzzer.desligar();
  init_servo(SERVO_PIN);
  Serial.println("Servo ok");
  imu.init();
  Serial.println("imu ok");
  bar.init();
  Serial.println("bar ok"); 

  mutexV = xSemaphoreCreateMutex();
  xQueue = xQueueCreate(5, sizeof(Evento));

  while (xQueue == NULL || mutexV == NULL) {
    delay(10);
  }

  xTaskCreate(
    taskAtualizaVelocidade,
    "IntegracaoVelocidade",
    4096,
    (void*) &imu,
    20, // Prioridade alta para leitura do IMU
    &xTaskVelocidade
  );

  xTaskCreate(
    taskMaqDeEstados,
    "MaquinaEstadosPID",
    4096,
    NULL,
    15,
    &xTaskMaqEstados
  );

  xTaskCreate(
    taskObterEvento,
    "ObterEvento",
    4096,
    NULL,
    10,
    &xTaskObterEvento
  );
  Serial.println("Tasks criadas, sistema pronto");
}

void taskMaqDeEstados(void *pvParameters) {
  Evento evRecebido;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequencia = pdMS_TO_TICKS(5); // 200 Hz

  for (;;) {
    if (xQueueReceive(xQueue, &evRecebido, 0) == pdTRUE) {
      if (evRecebido != Evento::Nenhum) {
        Transicao t = matrizTransicao[static_cast<int>(state)][static_cast<int>(evRecebido)];
        state = t.proximoEstado;
        executarAcao(t.acao);
      }
    }

    if (state == Estado::Controlando) {
      controlaVoo();
    } else if (state == Estado::Fault) {
      set_pos(0);
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequencia);
  }
}

void taskAtualizaVelocidade(void *pvParameters) {
  IMU* sensorImu = (IMU*) pvParameters;

  // 1. Calibração inicial na rampa (desconta gravidade e viés do sensor parado)
  const int amostrasCalib = 50;
  for (int k = 0; k < amostrasCalib; k++) {
    if (xSemaphoreTake(mutexV, portMAX_DELAY)) {
      sensorImu->atualizaDados();
      Dados d = sensorImu->leDados();
      for (int i = 0; i < 3; i++) {
        offsetAcel[i] += d.dados[i];
      }
      xSemaphoreGive(mutexV);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  for (int i = 0; i < 3; i++) {
    offsetAcel[i] /= amostrasCalib;
  }

  // 2. Loop determinístico de integração (50 Hz = 20 ms)
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequencia = pdMS_TO_TICKS(20);
  TickType_t ultimoTickIMU = xLastWakeTime;

  for (;;) {
    if (xSemaphoreTake(mutexV, portMAX_DELAY)) {
      TickType_t agora = xTaskGetTickCount();
      float dt = (agora - ultimoTickIMU) * portTICK_PERIOD_MS / 1000.0f;
      ultimoTickIMU = agora;

      sensorImu->atualizaDados();
      Dados dados = sensorImu->leDados();

      if (state != Estado::Oscioso) {
        for (int i = 0; i < 3; i++) {
          float acelReal_ms2 = (dados.dados[i] - offsetAcel[i]) * 9.80665f; // gravidade em m/s^2
          velocidade[i] += acelReal_ms2 * dt;
        }
      } else {
        velocidade[0] = 0.0f;
        velocidade[1] = 0.0f;
        velocidade[2] = 0.0f;
      }

      Serial.println(velocidade[2]);

      xSemaphoreGive(mutexV);
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequencia);
  }
}
void taskObterEvento(void *pvParameters) {
  //aguarda 600ms para o sistema estabilizar
  vTaskDelay(pdMS_TO_TICKS(600));

  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequencia = pdMS_TO_TICKS(20);

  float altitudeSolo = 0.0f;
  float altitudeMax = 0.0f;
  bool soloCalibrado = false;
  int contadorConfirmacao = 0; // Evita disparo falso por ruído

  for (;;) {
    Evento codigoEvento = Evento::Nenhum;
    float velZ = 0.0f, acelZ = 0.0f, altitudeAtual = 0.0f;

    if (xSemaphoreTake(mutexV, portMAX_DELAY)) {
      bar.atualizaDados();
      altitudeAtual = bar.leDados().dados[1];
      acelZ = imu.leDados().dados[2] - offsetAcel[2];
      velZ = velocidade[2];
      xSemaphoreGive(mutexV);
    }

    if (!soloCalibrado) {
      altitudeSolo = altitudeAtual;
      soloCalibrado = true;
    }

    float altRel = altitudeAtual - altitudeSolo;
    if (altRel > altitudeMax) altitudeMax = altRel;

    switch (state) {
      case Estado::Oscioso:
        // Exige 3 leituras seguidas (> 60 ms) com aceleração alta para confirmar ignição
        if (acelZ > 1.5f) {
          if (++contadorConfirmacao >= 3) {
            codigoEvento = Evento::Inicio;
            contadorConfirmacao = 0;
          }
        } else {
          contadorConfirmacao = 0;
        }
        break;

      case Estado::EsperandoLimiar:
        // Libera controle quando atingir altitude mínima OU velocidade vertical alta
        if (altRel >= 30.0f || velZ >= 15.0f) {
          codigoEvento = Evento::Altitude;
        }
        break;

      case Estado::Controlando:
        // Confirma apogeu se a altitude caiu 3m do pico por 3 ciclos seguidos
        if ((altitudeMax - altRel) > 3.0f) {
          if (++contadorConfirmacao >= 3) {
            codigoEvento = Evento::Apogeu;
            contadorConfirmacao = 0;
          } else {
            contadorConfirmacao += 1;
          }
        } else {
          contadorConfirmacao = 0;
        }
        break;

      case Estado::AquisicaoPura:
      case Estado::Fault:
        // Pouso confirmado se estiver perto do solo e praticamente sem aceleração/velocidade
        if (altRel <= 5.0f && abs(acelZ) < 0.15f) {
          if (++contadorConfirmacao >= 10) { // ~200 ms parado no solo
            codigoEvento = Evento::Aterrissagem;
            contadorConfirmacao = 0;
          } else {
            contadorConfirmacao += 1;
          }
        } else {
          contadorConfirmacao = 0;
        }
        break;
    }

    if (codigoEvento != Evento::Nenhum) {
      xQueueSend(xQueue, &codigoEvento, 0);
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequencia);
  }
}

Evento executarAcao(Acao codigoAcao) {
  Evento out = Evento::Nenhum;

  switch (codigoAcao) {
    case Acao::A00:
      Serial.println("Nada ocorreu");
      break;

    case Acao::A01:
      Serial.println("Transição para lançamento");
      // Ação de transição para lançamento (ex: bip de armado)
      break;

    case Acao::A02:
      Serial.println("Início do voo controlado");
      controlaVoo();
      break;

    case Acao::A03:
      Serial.println("Apogeu");
      // Ação de apogeu (ex: centralizar aletas)
      set_pos(0);
      break;

    case Acao::A04:
      Serial.println("Pouso");
      // Ação de pouso (ex: acionar buzzer de resgate)
      buzzer.ligar();
      break;
  }

  return out;
}

void controlaVoo() {
  static float integral = 0.0f;
  static float erroAnterior = 0.0f;
  static TickType_t ultimoTick = xTaskGetTickCount();

  if (xSemaphoreTake(mutexV, portMAX_DELAY)) {
    TickType_t agora = xTaskGetTickCount();
    float dt = (agora - ultimoTick) * portTICK_PERIOD_MS / 1000.0f;
    if (dt <= 0.0f) dt = 0.005f;
    ultimoTick = agora;

    float omegaGraus = imu.leDados().dados[5] * (180.0f / pi);
    float erro = 0.0f - omegaGraus;

    integral += erro * dt;
    integral = constrain(integral, -45.0f, 45.0f);

    float derivada = (erro - erroAnterior) / dt;
    erroAnterior = erro;

    float saidaServo = (k_p * erro) + (k_i * integral) + (k_d * derivada);
    saidaServo = constrain(saidaServo, -30.0f, 30.0f);
    set_pos(saidaServo);

    xSemaphoreGive(mutexV);
  }
}

void loop() {
  // O controle roda inteiramente nas tasks do FreeRTOS
  vTaskDelay(pdMS_TO_TICKS(1000));
}
