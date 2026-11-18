#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "imu.h" 

Adafruit_MPU6050 mpu;

void IMU::init() {
  // Retorna um novo IMU

  while (!mpu.begin()) {
    Serial.println("MPU não conctado");
  }

  sensors_event_t a, g, _;
  mpu.getEvent(&a, &g, &_);

  vals.dados[0] = a.acceleration.x;
  vals.dados[1] = a.acceleration.y;
  vals.dados[2] = a.acceleration.z;
  vals.dados[3] = g.gyro.x;
  vals.dados[4] = g.gyro.y;
  vals.dados[5] = g.gyro.z;  
}

void IMU::atualizaDados() {
  sensors_event_t a, g, _;
  mpu.getEvent(&a, &g, &_);
  vals.dados[0] = a.acceleration.x;
  vals.dados[1] = a.acceleration.y;
  vals.dados[2] = a.acceleration.z;
  vals.dados[3] = g.gyro.x;
  vals.dados[4] = g.gyro.y;
  vals.dados[5] = g.gyro.z;
}

Dados IMU::leDados() {
  return vals;
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
