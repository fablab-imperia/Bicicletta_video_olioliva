#include <Keyboard.h>

#define RAGGIO_RUOTA 1
#define PI_GRECO 3.1415
 
 volatile byte giri;
 volatile float tempo_secondi;
 volatile unsigned long timeold;
 volatile float velocita_attuale;
 void setup()
 {
   Serial.begin(115200);
   attachInterrupt(digitalPinToInterrupt(2), magnet_detect, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)
   //giri = 0;
   tempo_secondi = 0;
   timeold = 0;
 }
 void loop()//Measure RPM
 {
   if (millis()-timeold >= 2000) { 
     velocita_attuale = 0;
   }

   Serial.println(velocita_attuale);

   
 }
 void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
 {
   //giri++;
   tempo_secondi = (millis() - timeold)/1000.0;     //*giri;
   velocita_attuale = 2*PI_GRECO*RAGGIO_RUOTA/tempo_secondi;
   //Serial.println(millis() - timeold);
   timeold = millis();
   //giri = 0;
   //Serial.println(velocita_attuale);
   //Serial.println("detect");
 }
