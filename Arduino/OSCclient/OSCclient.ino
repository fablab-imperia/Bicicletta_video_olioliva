/*
  <Installazione interattiva pista ciclabile is an interactive installation to promote a cycle lane in Liguria.
  This program sends bike speed values to a pc>
    Copyright (C) 2019 Massimo Gismondi, Fablab Imperia

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

//Turorial OSC -> http://apps.incalcando.com/2017/05/22/sensor-data-transmitted-as-osc-over-wifi-huzzah-esp8266/

#define MODALITA 0//0=OSC, 1=Seriale

//Moduli
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>

//Credenziali WIFI
char* ssid = "FASTWEB-kqh64N";
char* password = "QTpgKvA5uQ";
WiFiUDP Udp;
const IPAddress outIp(192, 168, 1, 249);
const int outPort = 7878;

//
#define T_UTILE_SENSORE 1000
#define T_ACCENSIONE_LED 100
#define T_INVIO_COMANDI 500
#define T_RILASCIO_TASTI 300

//Tasti da inviare
#define TASTO_ON 'a'
#define TASTO_OFF 'c'

#define PIN_LED 13
#define PIN_SENSORE 3

volatile int ms_rovescia_ultima_rilevazione = 0;
volatile int ms_rovescia_accensione_led = 0;
volatile int ms_invio_comandi = 0;
volatile int ms_tasto = T_RILASCIO_TASTI;


unsigned int old_millis = 0;

//Prototipi
/*
  void aziona_tastiera(int tempo);
  void aziona_led(int tempo);
*/

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SENSORE, INPUT);

  Serial.begin(4800);
  //Inizializzo WIFI
  Serial.println();
  Serial.print("Connessione a... ");
  Serial.println(ssid);

  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //Connessione
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(0, LOW);
    delay(10);
    digitalWrite(0, HIGH);
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  attachInterrupt(digitalPinToInterrupt(PIN_SENSORE), magnet_detect, RISING);
}

void loop()
{

  //Calcolo delta e decremento valori
  int millis_attuali = millis();
  int delta = millis_attuali - old_millis;
  old_millis = millis_attuali;
  ms_rovescia_ultima_rilevazione = ms_rovescia_ultima_rilevazione - delta;
  ms_rovescia_accensione_led = ms_rovescia_accensione_led - delta;
  ms_invio_comandi = ms_invio_comandi - delta;
  ms_tasto = ms_tasto - delta;

  if (ms_invio_comandi < 0)
  {
    ms_invio_comandi = T_INVIO_COMANDI;
    //Invio pressioni tasti
    aziona_tastiera(ms_rovescia_ultima_rilevazione);
  }
  aziona_led(ms_rovescia_accensione_led);

  if (ms_tasto < 0)
  {
    sendOSC(TASTO_ON, 0);
    sendOSC(TASTO_OFF, 0);
  }
  delay(500);
  sendOSC(TASTO_OFF, millis()/1000);
}

void magnet_detect()
{
  ms_rovescia_ultima_rilevazione = T_UTILE_SENSORE;
  ms_rovescia_accensione_led = T_ACCENSIONE_LED;
}

void aziona_tastiera(int tempo)
{
  if (MODALITA == 0) //UDP Osc
  {
    if (tempo > 0)
    {
      sendOSC(TASTO_ON, 1);
      ms_tasto = T_RILASCIO_TASTI;
    }
    else if (tempo > -2000)
    {
      sendOSC(TASTO_OFF, 1);
      ms_tasto = T_RILASCIO_TASTI;
    }
    else
    {
      //Nulla, evito di riempire la comunicazione se per
      //due secondi non compare niente
    }
  }
  else//Seriale
  {
    if (tempo > 0)
    {
      Serial.println("Acceso");
      ms_tasto = T_RILASCIO_TASTI;
    }
    else if (tempo > -2000)
    {
      Serial.println("Spento");
      ms_tasto = T_RILASCIO_TASTI;
    }
    else
    {
      //Nulla, evito di riempire la comunicazione se per
      //due secondi non compare niente
    }
  }
}

void aziona_led(int tempo)
{
  if (tempo > 0)
  {
    digitalWrite(PIN_LED, HIGH);
  }
  else
  {
    digitalWrite(PIN_LED, LOW);
  }
}

void sendOSC(char carattere, int stato_da_applicare)
{
  OSCBundle bundle;
  OSCMessage msg = bundle.add("/bici/" + carattere);
  msg.add(stato_da_applicare);
  Udp.beginPacket(outIp, outPort);
  bundle.send(Udp);
  Udp.endPacket();
  Serial.println("Pacchetto inviato");
}
