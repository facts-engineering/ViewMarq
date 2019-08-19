/************************************************
Written by Tristan Warder for FACTS Engineering
Copyright (c) 2019 FACTS Engineering, LLC
Licensed under the MIT license.
************************************************/

#include <ViewMarq.h>
#include <WiFiNINA.h> //use any WiFi library using the Client class
#include "arduino_secrets.h"

//This example program shows how to use the ViewMarq Arduino library to program a ViewMarq
//display to show "Hello World!" using an Arduino Microprocessor and an Arduino Ethernet
//shield.

IPAddress address(192, 168, 0, 182); // update with the IP Address of your Modbus server

byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF }; //change if there are any devices on your network with this MAC address

WiFiClient signClient;

VMDisplay sign(0, signClient, address);  //initialize the VMDisplay with its ID, communications client, and IPAddress.
//if the ID is 0, any ViewMarq will accept the code. Anything else needs to be assigned to that ViewMarq via its software.

void setup() {
  while(!WiFi.begin(SECRET_SSID, SECRET_PASS)) { ; }  //wait until WiFi is connected
  Serial.begin(9600);   //begin serial communications
  Serial.println("Connected!");

  sign.setLine(1, true);                //enable line 1 of 4 (line one is defaultly enabled, all others are disabled defaultly)
  sign.setText(1, "Hello World!");      //set line 1's text to "Hello World!" (defaultly blank)
  sign.setTextSize(1, 2);               //set line 1's text size to 2 of 11 (defaultly 1)
  sign.setScrollType(1, SCROLL_LEFT);   //set line 1's text to scroll left (defaultly left justified)
  sign.setScrollSpeed(1, SLOW);         //set line 1's scroll speed to slow (defaultly medium)
  sign.setBlink(1, FAST);               //set line 1's text to blink fast (defaultly off)
  sign.setColor(1, AMBER);              //set line 1's text color to amber (defaultly green)

  sign.writeMessage();                  //write the display's message with these settings
}

void loop() {
  sign.sendMessage();                   //send the, now written, message to the display
}
