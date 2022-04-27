int trig = 11;
int echo = 10;
int relay = 12;

int button = 13;

long gel_remaining_time = 0;
long gel_time = 1; // Tiempo para tomar gel
bool is_gel_ready = false;
bool is_gel_throwing = false;

void setup() {
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(button, INPUT);

  digitalWrite(relay, HIGH);

  Serial.begin(9600);
}
void loop() {

  if(isGelButtonPressed()){
    is_gel_ready = true;
  }

  if(is_gel_ready && !is_gel_throwing){
    double distancia = getDistance();
    
    if (distancia < 10){
        is_gel_throwing = true;
        gel_remaining_time = millis()+gel_time*1000;
        digitalWrite(relay, LOW);   
    }

  } else if(is_gel_ready && is_gel_throwing){
    if (millis() >= gel_remaining_time){    
      digitalWrite(relay, HIGH);
      is_gel_ready = false;
      is_gel_throwing = false;
    } 
  }

  
}

double getDistance(){
  digitalWrite(trig, HIGH);
  delay(1);
  digitalWrite(trig, LOW);
  int tiempo = pulseIn(echo, HIGH);
  return tiempo / 58.2;
}

bool isGelButtonPressed(){
  return digitalRead(button) == HIGH;
}