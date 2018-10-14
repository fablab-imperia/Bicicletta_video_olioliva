/*
<Installazione interattiva pista ciclabile is an interactive installation to promote a cycle lane in Liguria.
This program sends bike speed values to a pc>
    Copyright (C) 2018 Massimo Gismondi, Stefano Semeria

This file is part of Installazione interattiva pista ciclabile.

    Installazione interattiva pista ciclabile is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Installazione interattiva pista ciclabile is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Nome-Programma.  If not, see <http://www.gnu.org/licenses/>.
*/

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
