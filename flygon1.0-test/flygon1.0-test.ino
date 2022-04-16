#include <Wire.h>			// libreria de comunicacion por I2C
#include <LCD.h>			// libreria para funciones de LCD
#include <LiquidCrystal_I2C.h>		// libreria para LCD por I2C

#include "MAX30100_PulseOximeter.h"

#include <Keypad.h>

#include <Adafruit_MLX90614.h>

// HeartPusle sensor config  
int pulsePin = 0;                   // Sensor de Pulso conectado al puerto A0
volatile int BPM;                   // Pulsaciones por minuto
volatile int Signal;                // Entrada de datos del sensor de pulsos
volatile int IBI = 600;             // tiempo entre pulsaciones
volatile boolean Pulse = false;     // Verdadero cuando la onda de pulsos es alta, falso cuando es Baja
volatile boolean QS = false;        // Verdadero cuando el Arduino Busca un pulso del Corazon


// Oximeter sensor config
#define REPORTING_PERIOD_MS 1000
uint32_t tsLastReport = 0;
PulseOximeter pox;

uint8_t ox = 0;
uint8_t bias = 0;
float IC2heartRate = 0;

// Temp sensor config
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
double curentTemperature = 0.0;

// Display config
LiquidCrystal_I2C lcd (0x27, 2, 1, 0, 4, 5, 6, 7); // DIR, E, RW, RS, D4, D5, D6, D7

// Keypad config
const byte rows = 4;
const byte cols = 4;
char keys[rows][cols] = { 
    {'1',  '2', '3', 'A'},
    {'4',  '5', '6', 'B'},
    {'7',  '8', '9', 'C'},
    {'*',  '0', '#', 'D'}
 };

byte rowPins[rows] = {9,8,7,6};
byte colPins[cols] = {5,4,3,2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

char keyPressed;
String text = "";

void setup() {
    Serial.begin(9600);

    // Init Display
    lcd.setBacklightPin(3,POSITIVE);	// puerto P3 de PCF8574 como positivo
    lcd.setBacklight(HIGH);		// habilita iluminacion posterior de LCD
    lcd.begin(16, 2);			// 16 columnas por 2 lineas para LCD 1602A
    lcd.clear();			// limpia pantalla

    // Init oximeter sensor
    if (!pox.begin())
    {
        Serial.println("- Oximeter FAILED -");
    }
    else
    {
        Serial.println("- Oximeter SUCCESS -");
    }

    // Init temperature sensor
    if (!mlx.begin()) {
        Serial.println("Error connecting to MLX sensor. Check wiring.");
    }
    
    // Init HeartPulse sensor
    interruptSetup();    
}

void initDisplay() {
    
}

void initOximeter() {

}

void initTemperature() {

}

void loop() {
    keyPressed = keypad.getKey();

    curentTemperature = mlx.readObjectTempC();


    pox.update();

    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
        ox = pox.getSpO2();
        IC2heartRate = pox.getHeartRate();
        bias = pox.getRedLedCurrentBias();

        tsLastReport = millis();
    }

    if (QS == true){                       // Bandera del Quantified Self es verdadera cuando el Arduino busca un pulso del corazon
        QS = false;                          // Reset a la bandera del Quantified Self 
    }

        // lcd.setCursor(0, 0);		// ubica cursor en columna 0 y linea 0
        // lcd.print("BPM:");	// escribe el texto
        // lcd.print(pox.getHeartRate());	// escribe el texto
        // lcd.print(" Ox:");	// escribe el texto
        // lcd.print(String(pox.getSpO2())+"     ");	// escribe el texto

    // 
    lcd.setCursor(0, 0);
    lcd.print("P:");
    lcd.print(String(IC2heartRate));
    lcd.print(" O:");
    lcd.print(String(ox)+"   ");
    
    lcd.setCursor(0, 1);
    lcd.print("P2:");
    lcd.print(BPM);
    lcd.print(" T:");
    lcd.print(String(curentTemperature)+"   ");

}
