#include <HTTPClient.h>
#include <WiFi.h>

#define RXD2 16
#define TXD2 17

 // Wifi 
const char* ssid = "FLYGON_WIFI";
const char* pass = "FLYGON12345";

// IFTTT Webwook url
const String WebhookURL = "https://flygon1-ues.herokuapp.com"; // variable 2


void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial.println("INIT SERIAL");

  WiFi.begin(ssid, pass);

  Serial.println("Connecting...");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected");
  Serial.println(WiFi.localIP());
}

void loop() {

  const String txt = Serial2.readString();

  if(txt.length() > 0){
    Serial.println(txt);
    handleRead(txt);
  }
}

void handleRead(String read){
    
    read.trim();
    if(read[0] != '$') return;

    String id = "";
    String ox = "";
    String bpm = "";
    String temp = "";

    for (size_t i = 0; i < read.length(); i++)
    {
       if(read[i] == '$'){
           if(i+3 >= read.length()) continue;
           
           if(read[i+1] == 'I' && read[i+2] == 'D'){
                for (size_t j = i+3; j < read.length(); j++)
                {
                    if(read[j] == '$') break;
                    if(read[j] != '=') id += read[j];
                }
           }
           
           if(read[i+1] == 'O'){
                for (size_t j = i+2; j < read.length(); j++)
                {
                    if(read[j] == '$') break;
                    if(read[j] != '=') ox += read[j];
                }
           }

           if(read[i+1] == 'T'){
                for (size_t j = i+2; j < read.length(); j++)
                {
                    if(read[j] == '$') break;
                    if(read[j] != '=') temp += read[j];
                }
           }

           if(read[i+1] == 'B'){
                for (size_t j = i+2; j < read.length(); j++)
                {
                    if(read[j] == '$') break;
                    if(read[j] != '=') bpm += read[j];
                }
           }

       }
    }
    
    Serial.println("ID: "+id);
    Serial.println("ox: "+ox);
    Serial.println("temp: "+temp);
    Serial.println("bpm: "+bpm);

    if(uploadData(id, ox, bpm, temp) == true){
        Serial.println("Sent");
    } else Serial.println("Error on send");
}


bool uploadData(String id,  String ox, String bpm, String temp ){
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;

    http.begin(WebhookURL+"/api/alumno/"+id+"/lectura");
    http.addHeader("Content-Type", "application/json");


    String json = "{";
                 json += "\"temp\":"+temp+","; 
                 json += "\"ox\":"+ox+","; 
                 json += "\"bpm\":"+bpm+""; 
           json += "}"; 

    int httpResponseCode = http.POST(json);
    
    // Serial.println(json);
    // Serial.println(value);
    // Serial.println(_value);
    // Serial.println(httpResponseCode);

    if(httpResponseCode >= 200 && httpResponseCode < 300)
    return true;
    else return false;
  }
}
