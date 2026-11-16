#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

void init_IMU() {
  while (!mpu.begin()) {
    Serial.println("MPU não conctado");
  }
}

int* leDadosIMU() {

  // a função retorna 
  int *dados = (int*)malloc(sizeof(int[6]));
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
