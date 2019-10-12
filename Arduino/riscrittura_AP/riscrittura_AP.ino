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

//#define DEBUG 1//1 acceso, 0 debug seriale spento

//Moduli
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

//Credenziali WIFI
char* ssid = "IMbici";
char* password = "#IMbici_FablabImperia_2019";
WiFiUDP Udp;
const IPAddress outIp2(192, 168, 1, 100);
//const IPAddress outIp3(192, 168, 1, 101);
const int outPort = 7878;
const int listenPort = 8000;


IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

//
#define T_UTILE_SENSORE 1000
#define T_ACCENSIONE_LED 100
#define T_INVIO_COMANDI 500
#define T_RILASCIO_TASTI 300

//Tasti da inviare
#define TASTO_ON 'a'
#define TASTO_OFF 'c'

#define PIN_LED 13
#define PIN_SENSORE D6

volatile int ms_rovescia_ultima_rilevazione = 0;
volatile int ms_rovescia_accensione_led = 0;
volatile int ms_invio_comandi = 0;


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

  WiFi.mode(WIFI_AP);
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

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
  //ms_tasto = ms_tasto - delta;

  aziona_led(ms_rovescia_accensione_led);

  sendOSC(ms_rovescia_ultima_rilevazione);

  delay(250);
  //Serial.println(analogRead(PIN_SENSORE));
  //sendOSC(TASTO_OFF, millis() / 1000);
}

void magnet_detect()
{
  ms_rovescia_ultima_rilevazione = T_UTILE_SENSORE;
  ms_rovescia_accensione_led = T_ACCENSIONE_LED;
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

void sendOSC(int dati_sensore)
{
  //OSCBundle bundle;

  OSCMessage msg("/bici/movimento");
  msg.add(dati_sensore);
  Udp.beginPacket(outIp2, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

  /*
  Udp.beginPacket(outIp3, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
  */

  Serial.println("Pacchetto inviato");
}
