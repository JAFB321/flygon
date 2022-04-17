void initDisplay() {
    // Init Display
    lcd.setBacklightPin(3,POSITIVE);	// puerto P3 de PCF8574 como positivo
    lcd.setBacklight(HIGH);		// habilita iluminacion posterior de LCD
    lcd.begin(16, 2);			// 16 columnas por 2 lineas para LCD 1602A
    lcd.clear();			// limpia pantalla
}

void initOximeter() {
    // Init oximeter sensor
    if (!pox.begin())
    {
        Serial.println("- Oximeter FAILED -");
    }
    else
    {
        Serial.println("- Oximeter SUCCESS -");
    }
}

void initTemperature() {
    // initialize the sensor
    while( NO_ERR != sensor_temp.begin() ){
    Serial.println("Communication with device failed, please check connection");
    delay(3000);
    }
    Serial.println("Temperature Ok!");

  sensor_temp.enterSleepMode();
  delay(50);
  sensor_temp.enterSleepMode(false);
  delay(200);

  sensor_temp.setEmissivityCorrectionCoefficient(.95);
  float ambientTemp = sensor_temp.getAmbientTempCelsius();
  float objectTemp = sensor_temp.getObjectTempCelsius();
}

void initPulseSensor(){
    // Init HeartPulse sensor
    interruptSetup(); 
}