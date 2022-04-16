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
      // Init temperature sensor
    if (!mlx.begin()) {
        Serial.println("Error connecting to MLX sensor. Check wiring.");
    }
}

void initPulseSensor(){
    // Init HeartPulse sensor
    interruptSetup(); 
}