#include <DFRobot_MLX90614.h>
#include <Wire.h>
#include <LCD.h>			// libreria de control del LCD
#include <LiquidCrystal_I2C.h>		// libreria para LCD por I2C

DFRobot_MLX90614_IIC sensor;   // instantiate an object to drive our sensor

void setup(){
  Serial.begin(9600);

  // initialize the sensor
  while( NO_ERR != sensor.begin() ){
    Serial.println("Communication with device failed, please check connection");
    delay(3000);
  }
  Serial.println("Begin ok!");

  sensor.enterSleepMode();
  delay(50);
  sensor.enterSleepMode(false);
  delay(200);

  sensor.setEmissivityCorrectionCoefficient(.95);
  float ambientTemp = sensor.getAmbientTempCelsius();
  float objectTemp = sensor.getObjectTempCelsius();

}

void loop(){
  float ambientTemp = sensor.getAmbientTempCelsius();
  float objectTemp = sensor.getObjectTempCelsius();
  
  Serial.print("Ambiente: ");
  Serial.println(ambientTemp);

  Serial.print("Objeto: ");
  Serial.println(objectTemp);
  
  delay(1000);
}
