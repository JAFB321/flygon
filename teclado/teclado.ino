#include <Keypad.h>

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

void setup() {
    Serial.begin(9600);
}

void loop() {
    keyPressed = keypad.getKey();


    if(keyPressed){
        Serial.println(keyPressed);
    }

    // delay(500);
    
}
