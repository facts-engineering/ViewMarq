/************************************************
Written by Tristan Warder for FACTS Engineering
Copyright (c) 2019 FACTS Engineering, LLC
Licensed under the MIT license.
************************************************/

#include <ViewMarq.h>

IPAddress server(192, 168, 0, 182); // update with the IP Address of your Modbus server

byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //change if there are any devices on your network with this MAC address

EthernetClient signClient;

VMDisplay sign(1, signClient, server);  //initialize the VMDisplay with its ID, communications client, and IPAddress.
//if the ID is 0, any ViewMarq will accept the code. Anything else needs to be assigned to that ViewMarq via its software.

void setup() {
  //Initialize serial and wait for port to open:
  Ethernet.begin(mac);  //begin ethernet communications
  Serial.begin(9600); //begin serial communications
  //write command string to display seconds since the program started
  sign.writeMessage("<ID 0><CLR><WIN 0 0 287 31><POS 0 0><LJ><BL N><CS 1><RED><T>Started </T><DEC 1 4 0><T> Seconds Ago</T>");
}

long int number = 0;
int counterState = 0;

void loop() {
  if(counterState == 0) {
    number = millis() / 1000; //set number to the seconds since program started
  }
  else if(counterState == 1) {
    number = (millis() / 1000) / 60;  //set number to the minutes since program started
  }
  else if(counterState == 2) {
    number = (millis() / 1000) / 60 / 60; //set number to the hours since program started
  }
  if(number >= 60 && counterState == 0) {
    //write command string to display minutes since the program started
    sign.writeMessage("<ID 0><CLR><WIN 0 0 287 31><POS 0 0><LJ><BL N><CS 1><RED><T>Started </T><DEC 1 4 0><T> Minutes Ago</T>");
    number = 1;
    counterState++;
  }
  else if(number >= 60 && counterState == 1) {
    sign.writeMessage("<ID 0><CLR><WIN 0 0 287 31><POS 0 0><LJ><BL N><CS 1><RED><T>Started </T><DEC 1 4 0><T> Hours Ago</T>");
    number = 1;
    counterState++;
  }
  sign.updateDecimal(1, number);  //update the value of the variable being displayed to the value of "number".
  sign.sendMessage();
}
