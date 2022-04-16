#include <Wire.h>
#include <Adafruit_MLX90614.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
  Serial.begin(9600);
  
  pinMode(A4,OUTPUT);
  pinMode(A5,OUTPUT);
  digitalWrite(A4,LOW);
  digitalWrite(A5,LOW);
  delay(100);

  if(mlx.begin()){
    Serial.println("sensor OK");
  }
}

void loop() {
  Serial.print("Ambiente = "); Serial.print(mlx.readAmbientTempC()); Serial.print("*C");
  Serial.print(" Objeto= "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  // Serial.println(mlx.readObjectTempC());
  delay(2000);
}
