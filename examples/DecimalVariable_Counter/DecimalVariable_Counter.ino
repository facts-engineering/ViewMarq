/************************************************
Written by Tristan Warder for FACTS Engineering
Copyright (c) 2019 FACTS Engineering, LLC
Licensed under the MIT license.
************************************************/

#include <ViewMarq.h>

//This example program shows how to use the ViewMarq Arduino library to program a ViewMarq
//display to count seconds since the program started using an Arduino Microprocessor and
//an Arduino Ethernet shield.

IPAddress address(192, 168, 0, 182); // update with the IP Address of your Modbus server

byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF }; //change if there are any devices on your network with this MAC address

EthernetClient signClient;

VMDisplay sign(0, signClient, address);  //initialize the VMDisplay with its ID, communications client, and IPAddress.
//if the ID is 0, any ViewMarq will accept the code. Anything else needs to be assigned to that ViewMarq via its software.

void setup() {
  Ethernet.begin(mac);  //begin ethernet communications
  Serial.begin(9600);   //begin serial communications

  //manually write the command string to use a display a decimal variable stored on the display
  //syntax for decimal variables is <DEC (variable number) (digits) (decimal places)>
  sign.writeMessage("<ID 0><CLR><WIN 0 0 287 31><POS 0 0><LJ><BL N><CS 1><AMB><DEC 1 5 0>");
}

long int number = 1;

void loop() {
  sign.sendMessage();             //send the, now written, message to the display
  sign.updateDecimal(1, number);  //change the displayed variable (variable 1) being displayed to the value of the
  //variable "number". If the digits of this value is different than what was specified in the command string,
  //the function will automatically resend the message to reflect this.
  //To avoid this, specify false as the third argument for the function call
  delay(1000);
  number++;
}
