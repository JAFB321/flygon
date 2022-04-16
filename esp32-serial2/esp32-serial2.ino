#define RXD2 16
#define TXD2 17

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("INIT SERIAL");
}

void loop() {
  const String txt = Serial2.readString();

  if(txt.length() > 0){
    Serial.print("READ: "); 
    Serial.println(txt);
  }
}
