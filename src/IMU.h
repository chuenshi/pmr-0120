#ifndef IMU_H
#define IMU_H

#include "sensor.h"

class IMU: public Sensor {
  public:
    IMU init(); // Retorna um novo IMU
    Dados leDados(IMU s); // Le os dados do IMU
    void atualizaDados(IMU s); // Modifica os dados de um IMU
  private:
    Dados vals;
};


void init_IMU();

float* leDadosIMU();

#endif
