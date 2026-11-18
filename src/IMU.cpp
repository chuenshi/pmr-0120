#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "imu.h" 

Adafruit_MPU6050 mpu;

IMU IMU::init() {
  // Retorna um novo IMU
  IMU out;

  while (!mpu.begin()) {
    Serial.println("MPU não conctado");
  }

  Dados d;
  sensors_event_t a, g, _;
  mpu.getEvent(&a, &g, &_);
  d.dados[0] = a.acceleration.x;
  d.dados[1] = a.acceleration.y;
  d.dados[2] = a.acceleration.z;
  d.dados[3] = g.gyro.x;
  d.dados[4] = g.gyro.y;
  d.dados[5] = g.gyro.z;
  
  out.vals = d;
    
  return out;
}

void IMU::atualizaDados(IMU s) {
  sensors_event_t a, g, _;
  mpu.getEvent(&a, &g, &_);
  s.vals.dados[0] = a.acceleration.x;
  s.vals.dados[1] = a.acceleration.y;
  s.vals.dados[2] = a.acceleration.z;
  s.vals.dados[3] = g.gyro.x;
  s.vals.dados[4] = g.gyro.y;
  s.vals.dados[5] = g.gyro.z;
}

Dados IMU::leDados(IMU s) {
  return s.vals;
}


void init_IMU() {
  while (!mpu.begin()) {
    Serial.println("MPU não conctado");
  }
}

float* leDadosIMU() {
  // a função retorna 
  float *dados = (float*)malloc(sizeof(float[6]));
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  dados[0] = a.acceleration.x;
  dados[1] = a.acceleration.y;
  dados[2] = a.acceleration.z;
  dados[3] = g.gyro.x;
  dados[4] = g.gyro.y;
  dados[5] = g.gyro.z;
  
  return dados;
}
