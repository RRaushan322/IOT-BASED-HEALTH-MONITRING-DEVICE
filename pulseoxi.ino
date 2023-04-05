#include <Arduino.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <ESP8266WiFi.h>

#include <FirebaseArduino.h>

#define REPORTING_PERIOD_MS 1000

#define FIREBASE_HOST "THE LINK OF UR DB PROJECT"
#define FIREBASE_AUTH "THE KEY OF UR FIREBASE PROJECT"

#define WIFI_SSID "YOUR SSID"
#define WIFI_PASSWORD "YOUR PASS"
int outputpin= A0;
PulseOximeter pox;

unsigned long previousMillis=0;
const long interval=1000;
volatile boolean heartBeatDetected = false;

void onBeatDetected()
{
heartBeatDetected = true;
Serial.println("Beat!");

}

void setup()
{
Serial.begin(115200);

WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print("Connecting");
while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
delay(500);
}
Serial.println();
Serial.print("Connected with IP: ");
Serial.println(WiFi.localIP());

Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

if (Firebase.failed()) {
Serial.print("setting /message failed:");
Serial.println(Firebase.error());
}

if (!pox.begin()) {
Serial.println("FAILED");
for(;;);
} else {
Serial.println("SUCCESS");
}

pox.setOnBeatDetectedCallback(onBeatDetected);
}

void pulse(){

float bpm = pox.getHeartRate();
float SpO2 =pox.getSpO2();

if( heartBeatDetected && bpm !=0) {

if(SpO2>0){

Firebase.setFloat("tech/Pulse rate", bpm);
Firebase.setFloat("tech/SpO2", SpO2);

Serial.print("data send to firebase");

}

}

}

void loop()
{
String path = "/Temp";
int analogValue = analogRead(outputpin);
float millivolts = (analogValue/1024.0) * 3300; 
float celsius = millivolts/10;
Serial.print("in DegreeC= ");
Serial.println(celsius);
Firebase.pushFloat("/Temp", celsius);

pox.update();

unsigned long currentMillis=millis();
if(currentMillis-previousMillis>=interval)
{
pox.shutdown();
pulse();
pox.resume();
previousMillis=currentMillis;

}

}
