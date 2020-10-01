#include <CTBot.h>
#include <ArduinoJson.h>
#include <AntaresESP8266HTTP.h>
#include "data.h"

#define WIFISSID "OPPO A3s"
#define PASSWORD "123lalita"

#define ACCESSKEY "d5e3724d6720d3e2:164864b3c1aef838"
#define projectName "nodemcu-banjir"
#define deviceName "ketinggia-air"

AntaresESP8266HTTP antares(ACCESSKEY);


CTBot myBot;
String token = "1040754658:AAEyLrpoDv8SRirxHrsZJElcSJIAYFq7Umk";
const int id = 803853646;

// defines variables
int duration;
int distance;
int safetyDistance;
boolean kirimTBot = true;
String jarak = "";

String jsonString = "{}";

// defines pins numbers
const int trigPin = 2;  //D4
const int echoPin = 0;  //D3
const int ledPin = 4;  //D2
const int buzzerPin = 5;  //D1

float regresi_a;
float regresi_b;

void setup() {


  Serial.begin(115200);
  myBot.wifiConnect(WIFISSID, PASSWORD);
  myBot.setTelegramToken(token);
  delay(1000);

  antares.setDebug(true);
  antares.wifiConnection(WIFISSID, PASSWORD);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  hitungParameterRegresi(regresi_a, regresi_b);

}

// the loop function runs over and over again forever
void loop() {
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
  
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
  
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
  
    // Calculating the distance
    distance = duration * 0.034 / 2;
  
    //distance = 4;
    //duration = 27;

    float perkiraanWaktu = 1.0 * regresi_b * distance + regresi_a;
  
  
    isiJson("distance", distance);
    isiJson("duration", perkiraanWaktu);
  
    //Serial.println(jsonString);
  //  antares.isi(jsonString);
    antares.isi(jsonString);
    antares.send(projectName, deviceName);
  
  
    safetyDistance = distance;
    if (safetyDistance <= 5) {
      jarak = "Ketinggian Air Saat Ini Adalah : ";
      jarak += int(distance);
      jarak += " *meter\n";
      jarak += " *Air Sudah Mencapai Ketinggian maksimal Awas Banjir!! \n";
      jarak += " Perkiraan waktu air sampai ke perumahan warga (miniatur)";
      jarak += String(perkiraanWaktu);
      jarak += " menit ";
      if (kirimTBot) {
        myBot.sendMessage(id, jarak, "");
        kirimTBot = false;
  
      }
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(ledPin, HIGH);
      Serial.println("Banjir");
    }
    else {
      digitalWrite(buzzerPin, LOW);
      digitalWrite(ledPin, LOW);
      kirimTBot = true;
      Serial.println("Aman");
    }


}

void isiJson(String key, int value) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& object = jsonBuffer.parseObject(jsonString);
  object[key] = value;
  String newInsert;
  object.printTo(newInsert);
  jsonString =  newInsert;
}

void hitungParameterRegresi(float &a, float &b)
{
  float X = 0;
  float Y = 0;
  float X2 = 0;
  float Y2 = 0;
  float XY = 0;

  int jumlahData = sizeof(data) / sizeof(Data);

  for (int i = 1; i < jumlahData; i++)
  {
    float x = abs(data[i].distance - data[i - 1].distance) / data[i].selisihWaktu;
    float y = data[i].selisihWaktu;

    X +=  x;
    Y +=  y;
    X2 += (x * x);
    Y2 += (y * y);
    XY += x * y;
  }

  a = ((Y * X2) - (X * XY)) / ((jumlahData * X2) - (X * X));
  b = ((jumlahData * XY) - (X * Y)) / ((jumlahData * X2) - (X * X));

  Serial.print("X=");
  Serial.println(X);
  Serial.print("Y=");
  Serial.println(Y);
  Serial.print("X2=");
  Serial.println(X2);
  Serial.print("Y2=");
  Serial.println(Y2);
  Serial.print("XY=");
  Serial.println(XY);
  Serial.print("a=");
  Serial.println(a);
  Serial.print("b=");
  Serial.println(b);

  
}
