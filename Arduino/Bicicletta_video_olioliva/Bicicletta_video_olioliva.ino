/*
<Installazione interattiva pista ciclabile is an interactive installation to promote a cycle lane in Liguria.
This program sends bike speed values to a pc>
    Copyright (C) 2018 Massimo Gismondi, Stefano Semeria, Francesco Scarrone

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
    along with Installazione interattiva pista ciclabile.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Keyboard.h>

#define RAGGIO_RUOTA 1
#define PI_GRECO 3.1415
#define VEL_MAX 6.0/*IN METRI AL SECONDO*/
#define VEL_FOLLE 11
#define MODALITA 1 //0 tastiera, 1 seriale
 
volatile unsigned long timeold;
volatile float velocita_attuale;

void setup()
{
  Serial.begin(9600);//Potrebbe essere parecchio alta, dobbiamo verificare la probabilita di errori di trasmissione 
  attachInterrupt(digitalPinToInterrupt(2), magnet_detect, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)
  Keyboard.begin();//Teoricamente necessario solo nella modalità tastiera, non in quella seriale
  timeold = 0;
}
void loop()
{
  if (millis()-timeold >= 2000) { 
    velocita_attuale = 0;
  }
  if (MODALITA==0)
  {
    gestionePressioneTasti(velocita_attuale);
  }
  else if (MODALITA==1)
  {
    Serial.print(int(velocita_attuale*1000));
    Serial.print(' ');
  }
  delay(10);
  
  
}
void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
{
  float tempo_secondi = (millis() - timeold)/1000.0;     //*giri;

  if(tempo_secondi>2*PI_GRECO*RAGGIO_RUOTA/VEL_FOLLE){
    velocita_attuale = (2*PI_GRECO*RAGGIO_RUOTA)/tempo_secondi;
    //Serial.println(millis() - timeold);
    timeold = millis();
    //Serial.println(velocita_attuale);
    //Serial.println("detect");
  }
  else
  {
    //Serial.println("Folle");
    velocita_attuale = (2*PI_GRECO*RAGGIO_RUOTA)/tempo_secondi;
    //Serial.println(millis() - timeold);
    timeold = millis();
  }
}

void gestionePressioneTasti(float velocita_attuale)
{
  static char ultimoCaratterePremuto='A';
  char nuovoCarattere = mappaVelocitaTasto(velocita_attuale);
  if (nuovoCarattere!=ultimoCaratterePremuto)
  {
    //Keyboard.write(nuovoCarattere);//DA ATTIVARE SOLO DOPO UN CONTROLLO CON PRINTLN
    Serial.print("Carattere che sara premuto: ");
    Serial.println(nuovoCarattere);
    ultimoCaratterePremuto = nuovoCarattere;
  }
}
char mappaVelocitaTasto(float velocita_attuale)
{
  int N = 5;
  char tasti[N] = {'A','S','D','F','G'};
  int indice = ((velocita_attuale/VEL_MAX)*N);
  if (indice<0)
  {
    indice=0;
  }
  else if (indice>=N)
  {
    indice=N-1;
  }
  return tasti[indice];
}
