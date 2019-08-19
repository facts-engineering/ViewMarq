/************************************************
Written by Tristan Warder for FACTS Engineering
Copyright (c) 2019 FACTS Engineering, LLC
Licensed under the MIT license.
************************************************/

#include <ViewMarq.h>

//This example program shows how to use the ViewMarq Arduino library to program a ViewMarq
//display to show "Hello World!" using an Arduino Microprocessor and an Arduino Ethernet
//shield.

IPAddress address(192, 168, 0, 182); // update with the IP Address of your Modbus server

byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF }; //change if there are any devices on your network with this MAC address

EthernetClient signClient;

VMDisplay sign(0, signClient, address);  //initialize the VMDisplay with its ID, communications client, and IPAddress.
//if the ID is 0, any ViewMarq will accept the code. Anything else needs to be assigned to that ViewMarq via its software.

void setup() {
  Ethernet.begin(mac);  //begin ethernet communications
  Serial.begin(9600);   //begin serial communications
  while(!Serial) { ; }
  // sign.setLine(1, true);                //enable line 1 of 4 (line one is defaultly enabled, all others are disabled defaultly)
  // sign.setText(1, "Hello World!");      //set line 1's text to "Hello World!" (defaultly blank)
  // sign.setTextSize(1, 2);               //set line 1's text size to 2 of 11 (defaultly 1)
  // sign.setScrollType(1, SCROLL_LEFT);   //set line 1's text to scroll left (defaultly left justified)
  // sign.setScrollSpeed(1, SLOW);         //set line 1's scroll speed to slow (defaultly medium)
  // sign.setBlink(1, FAST);               //set line 1's text to blink fast (defaultly off)
  // sign.setColor(1, AMBER);              //set line 1's text color to amber (defaultly green)
  //
  // sign.writeMessage();                  //write the display's message with these settings
  // sign.printMessage();

  //  The printMessage function will print the whole command string into the serial monitor. This can then be copied and used
  //  in the writeMessage function by pasting it as a string in quotes as seen below. This works very well if you want to quickly
  //  modify the command string, reduce the amount of code written, or if you want to use more advanced functions with your
  //  display that the "set" commands do not support, such as stored variables.

  //  the line below is equivalent to the commented out functions above
 sign.writeMessage("<ID 0><CLR><WIN 0 0 287 31><POS 0 0><SL><S S><BL F><CS 1><AMB><T>Hello World!</T>");
}

void loop() {
  sign.sendMessage();                   //send the, now written, message to the display
}
