#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include "CTBot.h"

const int trigPin1 = D0;
const int echoPin1 = D1;

#define Green D4
#define Red D3
#define BUZZER D5
CTBot myBot;

unsigned long ch_no = 2010424;
const char * write_api ="WDBQ6G7XCT700HPD";
char auth[] = "mwa0000028912887";
char ssid[] = "BPBD_2";
char pass[] = "123456789";
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 10000;
WiFiClient client;
long duration1;
int distance1;

String token = "5784862043:AAHvBNUfNLvpT0VxnUdor-Q38n1KvVtLtV8";  
const int id = 1252029108;

int alarmLuber = 15;

bool flag1 = false;
bool flag2 = false;
bool debug = true;

void setup() {
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(Green, OUTPUT);
  pinMode(Red, OUTPUT);
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conneted");
  Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);
  startMillis = millis();
  //
  myBot.setTelegramToken(token);
}

void loop() 
{
  long duration, ketinggian, air;
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  duration = pulseIn(echoPin1, HIGH);
  ketinggian = (duration/2) / 29.1;
  air = ketinggian;

  Serial.println("ketinggian air :");
  Serial.print(air);
  Serial.println("cm");
  delay(400);

  if(ketinggian <=15)
  {
    digitalWrite(Red, HIGH);
    digitalWrite(Green, LOW);
    tone(BUZZER, 300);
    delay(1000);
    noTone(BUZZER);
  }
  else
  {
    digitalWrite(Green, HIGH);
    digitalWrite(Red, LOW);
  }


  TBMessage msg;
  if(myBot.getNewMessage(msg))
  {
    Serial.println("Ada pesan masuk :" + msg.text);
    //
    String pesan = msg.text;
    if(pesan == "Ketinggian")
    {
      myBot.sendMessage(id, "Jarak permukaan air dengan sensor:  " + String(air) + " cm");
    }
    else if(pesan == "Status?")
    {
      if(air>15){
        myBot.sendMessage(id, "Aman");
      }
      else if(air<15){
        myBot.sendMessage(id, "Bahaya");
      }
      
    }
  }

  if (air <= alarmLuber && flag1 == false){
    flag1 = true;
    flag2 = false;

    myBot.sendMessage(id, "Warning!!!! jarak permukaan air dengan sensor : " + String(air) + " cm. Waspada Air luber");
  }


  currentMillis = millis();
  if (currentMillis - startMillis >= period)
  {
    ThingSpeak.setField(1, ketinggian);
    ThingSpeak.writeFields(ch_no, write_api);
    startMillis = currentMillis;
  }    
}
