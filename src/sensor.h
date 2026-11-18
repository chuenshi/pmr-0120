#ifndef SENSOR_H
#define SENSOR_H

#define TAM_DADOS 6

struct Dados{
  float dados[TAM_DADOS]; 
};

/*
  Aqui vai o codigo da classe sensor, que eh herdada
  pelo IMU e por Sensor.
  Os dados sao envelopados no struct Dados, para
  que eles possam ser passados como um array
  de 6 elementos.
*/

class Sensor {
  private:
    Dados vals;
  public:
    Sensor init();
    void atualizaDados(Sensor s);
    Dados le_dados(Sensor s);
};

#endif
