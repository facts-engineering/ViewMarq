/********************************************************************************
MIT License

Copyright (c) 2019 FACTS Engineering, LLC

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
********************************************************************************/

#ifndef VMDisplay_h
#define VMDisplay_h

#include "Arduino.h"
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#define GREEN 0
#define RED 1
#define AMBER 2
#define ADVANCED 3

#define SCROLL_LEFT 0
#define SCROLL_RIGHT 1
#define SCROLL_UP 2
#define SCROLL_DOWN 3
#define LEFT_JUSTIFIED 4
#define CENTER_JUSTIFIED 5
#define RIGHT_JUSTIFIED 6

#define SLOW 0
#define MEDIUM 1
#define FAST 2

#define NONE 3

class VMDisplay {
	private:
		int _ID;
		IPAddress address;
		static Client *client;
		bool _completed = false;
		int _maintenanceCommand = 4;
		uint16_t _commandData[512];
		char _commandString[512];
		char *_decLoc;
		char *_strLoc;
	public:
		static ModbusTCPClient VMClient;
		VMDisplay(int ID, Client &tempClient, IPAddress &ip) : address(ip) {
			VMDisplay::setLine(1, true);
			client = &tempClient;
			_ID = ID;
		}
		struct Line {
			bool isEnabled = false;
			int number;
			int textSize = 0;
			int color = 0;
			int scrollType = 4;
			int scrollSpeed = 1;
			int blink = 3;
			int variablePresent = 0;
			char text[256];
		};
		Line line[4];
		void setLine(int lineSelected, bool state);
		void setColor(int lineSelected, int color);
		void setTextSize(int lineSelected, int textSize);
		void setScrollType(int lineSelected, int scrollType);
		void setScrollSpeed(int lineSelected, int scrollSpeed);
		void setBlink(int lineSelected, int blink);
		void setText(int lineSelected, const char text[]);
		void setTestCondition(int condition);

		void updateDecimal(int variable, long int number, bool editBounds = true);
		void updateDecimal(int variable, const double actual, bool editBounds = true);

		void updateStringVar(int variable, const char text[100], bool editBounds = true);
    
		int lineConfig(int startPos, int lineSelected, char *arr);
		int writeToArr(int startPos, const char text[], char *arr);
		int generateString(char *string);
		void writeMessage();
		void writeMessage(const char text[]);
		void resetMessage();
		void printMessage();
		void returnMessage(char *arr);
		void changeIPAddress(IPAddress &ip);
		void connect();
		void sendMessage();

		int messageLength = 0;
};

#endif
