#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "imu.h" 

void IMU::init() {
  // Retorna um novo IMU

  while (!unit.begin()) {
    Serial.println("MPU não conctado");
  }

  sensors_event_t a, g, _;
  unit.getEvent(&a, &g, &_);

  vals.dados[0] = a.acceleration.x;
  vals.dados[1] = a.acceleration.y;
  vals.dados[2] = a.acceleration.z;
  vals.dados[3] = g.gyro.x;
  vals.dados[4] = g.gyro.y;
  vals.dados[5] = g.gyro.z;  
}

void IMU::atualizaDados() {
  sensors_event_t a, g, _;
  unit.getEvent(&a, &g, &_);
  vals.dados[0] = a.acceleration.x;
  vals.dados[1] = a.acceleration.y;
  vals.dados[2] = a.acceleration.z;
  vals.dados[3] = g.gyro.x;
  vals.dados[4] = g.gyro.y;
  vals.dados[5] = g.gyro.z;
}

Dados IMU::leDados() const{
  return vals;
}
