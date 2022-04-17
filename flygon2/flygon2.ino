#include <Wire.h>			// libreria de comunicacion por I2C
#include <LCD.h>			// libreria de control del LCD
#include <LiquidCrystal_I2C.h>		// libreria para LCD por I2C
#include "MAX30100_PulseOximeter.h"
#include <Keypad.h>
#include <DFRobot_MLX90614.h>


void(* resetFunc) (void) = 0;

// HeartPusle sensor config  
int pulsePin = 0;                   // Sensor de Pulso conectado al puerto A0
volatile int BPM;                   // Pulsaciones por minuto
volatile int Signal;                // Entrada de datos del sensor de pulsos
volatile int IBI = 600;             // tiempo entre pulsaciones
volatile boolean Pulse = false;     // Verdadero cuando la onda de pulsos es alta, falso cuando es Baja
volatile boolean QS = false;        // Verdadero cuando el Arduino Busca un pulso del Corazon


// Oximeter sensor config
PulseOximeter pox;

// uint8_t ox = 0;
// uint8_t bias = 0;
// float IC2heartRate = 0;

// Temp sensor config
DFRobot_MLX90614_IIC sensor_temp;   // instantiate an object to drive our sensor
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

byte rowPins[rows] = {6,7,8,9};
byte colPins[cols] = {2,3,4,5};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);
char keyPressed;

// ------------ Aplication State ------------
String _INIT = "1";
String _IDLE = "2";
String _REQUEST_ID = "3";
String _GET_READS = "4";
String _SEND_DATA = "5";
String _SUCCESS = "6";
String _RESET = "7";
String _ERROR = "8";

String STATE = _INIT;

// REQUEST_ID State
String ID = "";

// GET_READS State
long read_time = 10; // Tiempo de lectura 
long remaining_time = 0;

// SEND_DATA State
#define REPORTING_PERIOD_MS 1000
uint32_t tsLastReport = 0;

int sensor_val_position = 0;

double temp_values[10];
uint8_t ox_values[10];
int bpm_values[10];
int bpm2_values[10];

double temp_gap[] = {33.5,36.5};
uint8_t ox_gap[] = {80,100};
int bpm_gap[] = {70,111};

// SUCCESS State
double ox_sent = -1;
double temp_sent = -1;
double bpm_sent = -1;


void setup() {
    STATE = _INIT;

    Serial.begin(9600);
    Serial.println("Hola mundo!");
    initDisplay();
    initOximeter();
    initTemperature();
    initPulseSensor();

    randomSeed(analogRead(3));

    STATE = _IDLE;
}

void loop() {
    // Get sensor values
    keyPressed = keypad.getKey();

    pox.update();


    if (QS == true){   // Bandera del Quantified Self es verdadera cuando el Arduino busca un pulso del corazon
        QS = false;    // Reset a la bandera del Quantified Self 
    }




    if(STATE == _IDLE){
        onIDLE();
    }

    else if(STATE == _REQUEST_ID){
        onRequestID();
    }

    else if(STATE == _GET_READS){
        onGET_READS();
    }

    else if(STATE == _SEND_DATA){
        onSEND_DATA();
    }

    else if(STATE == _SUCCESS){
        onSUCCESS();
    }

    else if(STATE == _ERROR){
        onERROR();
    }


    if(STATE == _IDLE && keyPressed=='A'){ 
        STATE=_REQUEST_ID;
        lcd.clear();
    }

}

void onIDLE(){
    lcd.setCursor(0,0);
    lcd.print("Bienvenido");
    lcd.setCursor(0,1);
    lcd.print("Alumno!");
}

void onRequestID(){
    if(keyPressed && isNumber(keyPressed)){
        ID += keyPressed;
    }

    lcd.setCursor(0,0);
    lcd.print("Ingresa tu ID");
    lcd.setCursor(0,1);

    if(ID.length() > 0){
        lcd.print(ID);
    }

    if(keyPressed && ID.length() > 3 && keyPressed=='A'){
        initGET_READS();
    }
}

void initGET_READS(){
      remaining_time = millis()+read_time*1000;
      STATE=_GET_READS;
      lcd.clear();
}

void onGET_READS(){
    lcd.setCursor(0,0);
    lcd.print("Coloca tus dedos");
    lcd.setCursor(0,1);
    lcd.print(String((remaining_time-millis())/1000)+"s - ");
    // lcd.print(sensor_val_position);


    if (millis() - tsLastReport > REPORTING_PERIOD_MS && millis() <= remaining_time ){
        const int pos = sensor_val_position;
        
        if(pos < 9){
            double temp = sensor_temp.getObjectTempCelsius();
            if(temp > temp_gap[0] && temp < temp_gap[1]){
                temp_values[pos] = temp;
            } else temp_values[pos] = 0;

            uint8_t ox = pox.getSpO2();
            // uint8_t ox = random(90, 100);
            if(ox > ox_gap[0] && ox < ox_gap[1]){
                ox_values[pos] = ox;
            } else ox_values[pos] = 0;

            int bpm = round(pox.getHeartRate());
            if(bpm > bpm_gap[0] && bpm < bpm_gap[1]){
                bpm_values[pos] = bpm;
            } else bpm_values[pos] = 0;

            // lcd.setCursor(5, 1);
            // lcd.print(ox);

            // lcd.print(" _");
            // lcd.print(pos);
            // lcd.print("   ");

            // ox = 0;
            // temp = 0.0;
            // bpm = 0;
        }


        tsLastReport = millis();
        sensor_val_position++;
    }


    if(millis() > remaining_time ){
        remaining_time = 0;
        sensor_val_position = 0;
        
        initSEND_DATA();
    }
}

void initSEND_DATA(){
    STATE = _SEND_DATA;
    lcd.clear();
}

void onSEND_DATA(){
    lcd.setCursor(0,0);
    lcd.print("Enviando datos...");

    uint8_t ox = 0;
    double temp = 0.0;
    int bpm = 0;

    for (size_t i = 0; i < 10; i++)
    {
        // if(ox_values[i] != 0 && ox == 0) ox = ox_values[i];
        if(temp_values[i] != 0 && temp == 0.0) temp = temp_values[i];
        // if(bpm_values[i] != 0 && bpm == 0) bpm = bpm_values[i];
    }


    ox = getMostPopularElement(ox_values, 10);
    bpm = getMostPopularElement(bpm_values, 10);


    const int temp_0values = non0Values(temp_values);
    const int ox_0values = non0Values(ox_values);
    const int bpm_0values = non0Values(bpm_values);

    if(ox == 0) ox = get_non0Value(ox_values);
    if(bpm == 0) bpm = get_non0Value(bpm_values);

    if(ox_0values == 0 && temp_0values == 0 && bpm_0values == 0){
        initERROR();
    }
    else {
        if(ox_0values > 0 && (int)ox != 0){
            ox_sent = ox;
        }/*else ox_sent = random(93,97);*/

        if(temp_0values > 0){
            temp_sent = temp;
        }/*else temp_sent = random(temp_gap[0], temp_gap[1]) + ((double)(random(10,90))/100.0);*/

        if(bpm_0values > 0 && bpm != 0){
            bpm_sent = bpm;
        }/*else bpm_sent = random(bpm_gap[0], bpm_gap[1]);*/

        String data = "$ID=";
        data += String(ID);
        data += "$O=";
        data += String(ox_sent);
        data += "$T=";
        data += String(temp_sent);
        data += "$B=";
        data += String(bpm_sent);

        //  EXAMPLE 
        // $ID=12345$O=94.00$T=36.59$B=84.68

        Serial.println(data);

        initSUCCESS();
    }


    
    // lcd.setCursor(0,0);
    // lcd.print(temp_values[0]);
    // lcd.print(" ");
    // lcd.print(temp_values[1]);
    // lcd.print(" ");
    // lcd.print(temp_values[2]);
    // lcd.print(" ");
    // lcd.print(temp_values[3]);
    // lcd.print(" ");
    // lcd.print(temp_values[4]);
    // lcd.print(" ");
    // lcd.setCursor(0,1);
    // lcd.print(temp_values[5]);
    // lcd.print(" ");
    // lcd.print(temp_values[6]);
    // lcd.print(" ");
    // lcd.print(temp_values[7]);
    // lcd.print(" ");
    // lcd.print(temp_values[8]);
    // lcd.print(" ");
    // lcd.print(temp_values[9]);
    // lcd.print(" ");

}

void initSUCCESS(){
    STATE = _SUCCESS;
    lcd.clear();
}

void onSUCCESS(){
    lcd.setCursor(0,0);
    lcd.print("Lectura exitosa");
    lcd.setCursor(0,1);
    lcd.print("Muchas gracias!");

    if(keyPressed == 'A'){
        STATE = _RESET;
        RESET();
    }
}

void initERROR(){
    lcd.clear();
    STATE=_ERROR;
}

void onERROR(){
    lcd.setCursor(0, 0);
    lcd.print("Error de lectura");
    lcd.setCursor(0, 1);
    lcd.print("Vuelva a iniciar");

    if(keyPressed == 'A'){
        STATE = _RESET;
        RESET();
    }
}

void RESET(){
    lcd.clear();
    for (size_t i = 0; i < 10; i++)
    {
       ox_values[i] = 0;
       temp_values[i] = 0.0;
       bpm_values[i] = 0;
    }
    
    sensor_val_position = 0;

    ox_sent = 0.0;
    bpm_sent = 0.0;
    temp_sent = 0.0;

    

    ID = "";

    // Serial.println("\nRESET:");
    // Serial.println("Values");

    // for (size_t i = 0; i < 10; i++)
    // {
    //     Serial.println(ox_values[i]);
    //     Serial.println(temp_values[i]);
    //     Serial.println(bpm_values[i]);
    // }

    STATE = _IDLE;

    delay(500);
    resetFunc();
}

// Tools
String numbers = "1234567890";

bool isNumber(char c){
    if(numbers.indexOf(c)>=0)return true;
    else return false;
}

int non0Values(uint8_t vals[]){
    int counter = 0;
    uint8_t val0 = 0;

    // Serial.println("ox");

    for (size_t i = 0; i < 10; i++){
        if(vals[i] != val0) counter++;
        // Serial.println(vals[i]);
        
    }
    // Serial.println("ox counter"+String(counter));
    return counter;
}

int non0Values(double vals[]){
    int counter = 0;
    double val0 = 0;
    // Serial.println("temp");

    for (size_t i = 0; i < 10; i++){
        if(vals[i] != val0) counter++;
        // Serial.println(vals[i]);

    }
    // Serial.println("temp counter"+String(counter));

    return counter;
}

int non0Values(float vals[]){
    int counter = 0;
    float val0 = 0;
    // Serial.println("temp");

    for (size_t i = 0; i < 10; i++){
        if(vals[i] != val0) counter++;
        // Serial.println(vals[i]);

    }
    // Serial.println("temp counter"+String(counter));

    return counter;
}

int non0Values(int vals[]){

    int counter = 0;
    int val0 = 0;
    // Serial.println("bpm");

    for (size_t i = 0; i < 10; i++){
        if(vals[i] != val0) counter++;
        // Serial.println(vals[i]);

    }

    // Serial.println("bpm counter"+String(counter));

    return counter;
}

uint8_t get_non0Value(uint8_t vals[]){
    uint8_t val0 = 0;

    for (size_t i = 0; i < 10; i++){
        if(vals[i] != val0) return vals[i];
    }
    return 0;
}

float get_non0Value(float vals[]){
    float val0 = 0;

    for (size_t i = 0; i < 10; i++){
        if(vals[i] != val0) return vals[i];
    }
    return 0;
}

int get_non0Value(int vals[]){
    int val0 = 0;

    for (size_t i = 0; i < 10; i++){
        if(vals[i] != val0) return vals[i];
    }

    return 0;
}


int getMostPopularElement(int arr[], const int n)
{
    int count = 1, tempCount;
    int temp = 0,i = 0,j = 0;
    //Get first element
    int popular = arr[0];
    for (i = 0; i < (n- 1); i++)
    {
        temp = arr[i];
        tempCount = 0;
        for (j = 1; j < n; j++)
        {
            if (temp == arr[j] && arr[j] != 0)
                tempCount++;
        }
        if (tempCount > count)
        {
            popular = temp;
            count = tempCount;
        }
    }
    return popular;
}

uint8_t getMostPopularElement(uint8_t  arr[], const int n)
{
    int count = 1, tempCount;
    uint8_t temp = 0;
    int i = 0,j = 0;
    //Get first element
    int popular = arr[0];
    for (i = 0; i < (n- 1); i++)
    {
        if(arr[i] == 0) continue;

        temp = arr[i];
        tempCount = 0;
        for (j = 1; j < n; j++)
        {
            if (temp == arr[j]  && (int)(arr[j]) != 0)
                tempCount++;
        }
        if (tempCount > count)
        {
            popular = temp;
            count = tempCount;
        }
    }
    return popular;
}

// double getMostPopularElement(double  arr[], const int n)
// {
//     int count = 1, tempCount;
//     double temp = 0;
//     int i = 0,j = 0;
//     //Get first element
//     double popular = arr[0];
//     for (i = 0; i < (n- 1); i++)
//     {
//         if(arr[i] == 0.0) continue;

//         temp = arr[i];
//         tempCount = 0;
//         for (j = 1; j < n; j++)
//         {
//             if (temp == arr[j]  && (double)(arr[j]) != 0)
//                 tempCount++;
//         }
//         if (tempCount > count)
//         {
//             popular = temp;
//             count = tempCount;
//         }
//     }
//     return popular;
// }
