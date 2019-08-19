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

#include "Arduino.h"
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>
#include "ViewMarq.h"

#include <string.h>
#include <cmath>

Client *VMDisplay::client;	//initialize pointer to user defined client
ModbusTCPClient VMDisplay::VMClient(*client);	//initialize modbus client with user defined client

/*******************************************************************************
Description: Turn one of the ViewMarq display's four lines on or off by selecting
			 a line and setting its state. A line will only be written if it is
			 turned on here first (except line 1, which is on by default). If the
			 message is rewritten after turning a line off, it will no longer be
			 displayed.

Parameters: -int lineSelected - Specifies the line you are interacting with.
			-bool state - The state you are setting the line to.

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::setLine(int lineSelected, bool state) {
	if(lineSelected > 4 || lineSelected < 1) {
		Serial.println("Line selected must be between 1 and 4.");
	}
	else {
		line[lineSelected - 1].isEnabled = state;	//set the selected line's isEnabled member to state specified
	}
}

/*******************************************************************************
Description: Sets the color of the information being displayed on the selected line.

Parameters: -int lineSelected - Specifies the line you are interacting with.
			-int color - The color you are setting the line's information to.
			 Use constants RED, GREEN, and AMBER to select a color.

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::setColor(int lineSelected, int color) {
	if(lineSelected > 4 || lineSelected < 1) {
		Serial.println("Line selected must be between 1 and 4.");
	}
	else {
		line[lineSelected - 1].color = color;	//set the selected line's color member to the color specified
	}
}

/*******************************************************************************
Description: Set the text size of information displayed on the selected line. Note
			 that text sizes increase in order from 0-11, as opposed to the ASCII
			 code documentation, where it was 2, 0, 1, 3-11 in order from least to
			 greatest.

Parameters: -int lineSelected - Specifies the line you are interacting with.
			-int textSize - The size you are setting the line's text to. Enter
			 between 0-11, with 0 being smallest and 11 being largest.

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::setTextSize(int lineSelected, int textSize) {
	if(lineSelected > 4 || lineSelected < 1) {
		Serial.println("Line selected must be between 1 and 4.");
	}
	else {
		if(textSize == 0) {		//2 is actually the smallest character set
			line[lineSelected - 1].textSize = 2;	//keep it ordered least to greatest
		}
		else if(textSize == 1) {
			line[lineSelected - 1].textSize = 0;
		}
		else if(textSize == 2) {
			line[lineSelected - 1].textSize = 1;
		}
		else {	//anything other than 0, 1, and 2 are ordered normally
			line[lineSelected - 1].textSize = textSize;
		}
	}
}

/*******************************************************************************
Description: Set the type of scrolling the selected line will perform (if any).
			 The default scroll type is left justified

Parameters: -int lineSelected - Specifies the line you are interacting with.
			-int scrollType - The scroll type you are setting the line to.
			 Use constants SCROLL_LEFT, SCROLL_RIGHT, SCROLL_UP, SCROLL_DOWN,
			 LEFT_JUSTIFIED, CENTER_JUSTIFIED, and RIGHT_JUSTIFIED to specify.

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::setScrollType(int lineSelected, int scrollType) {
	if(lineSelected > 4 || lineSelected < 1) {
		Serial.println("Line selected must be between 1 and 4.");
	}
	else {
		line[lineSelected - 1].scrollType = scrollType;	//set the selected line's scrollType member to scroll specified
	}
}

/*******************************************************************************
Description: Set the speed of the selected line's scroll effect being used (if any).

Parameters: -int lineSelected - Specifies the line you are interacting with.
			-int scrollSpeed - The speed of the scroll being used.
			 Use constants SLOW, MEDIUM, and FAST to specify.

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::setScrollSpeed(int lineSelected, int scrollSpeed) {
	if(lineSelected > 4 || lineSelected < 1) {
		Serial.println("Line selected must be between 1 and 4.");
	}
	else {
		line[lineSelected - 1].scrollSpeed = scrollSpeed;	//set the selected line's scrollSpeed member to speed specified
	}
}

/*******************************************************************************
Description: Specify if the information on the selected line will be blinking,
			 and, if so, at what speed it will be. The default is no blinking.

Parameters: -int lineSelected - Specifies the line you are interacting with.
			-int blink - The speed at which the information will be blinking.
			 Use constants SLOW, MEDIUM, FAST, and NONE.

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::setBlink(int lineSelected, int blink) {
	if(lineSelected > 4 || lineSelected < 1) {
		Serial.println("Line selected must be between 1 and 4.");
	}
	else {
		line[lineSelected - 1].blink = blink;	//set the selected line's blink member to state specified
	}
}

/*******************************************************************************
Description: Set the text field to be displayed on the selected line using the
			 default or already specified properties.

Parameters: -int lineSelected - Specifies the line you are interacting with.
			-const char text[] - Enter a string literal with the text you would
			 like to display on the selected line.

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::setText(int lineSelected, const char text[]) {
	if(lineSelected > 4 || lineSelected < 1) {
		Serial.println("Line selected must be between 1 and 4.");
	}
	else {
		memset(line[lineSelected - 1].text, 0, 256);
		for(int i = 0; i < strlen(text); i++) {
			line[lineSelected - 1].text[i] = text[i];	//iterate and set the line's text field to the user defined text
		}
	}
}

/*******************************************************************************
Description: Maintenance function used to test the ViewMarq's LEDs using various
			 test patterns. If this function is used, it will overwrite any
			 messages that would be written to the sign. To turn the test off,
			 use the NONE constant as the argument. To use this function, you
			 must use the writeMessage function that generates the command
			 string automatically.

Parameters: -int condition - Specify the test condition being used on the sign.
			 Use constants RED, GREEN, AMBER, and ADVANCED.

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::setTestCondition(int condition) {
	_maintenanceCommand = condition;	//set the current maintenance command (if any)
}

/*******************************************************************************
Description: Update one of the display's stored decimal variables. This overload
			 is meant to be used with double values (decimals). This
			 function will adjust the bounds of the decimal point and digit length
			 automatically unless otherwise specified. If bounds are adjusted, the
			 message is rewritten to the sign, resetting its position to start.

Parameters: -int variable - Specifies which of the display's 32 decimal variables
			 you are updating.
			-const double actual - The double value to update the variable.
			-bool editBounds - Enable or disable bound adjustments (enabled defaultly).

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::updateDecimal(int variable, const double actual, bool editBounds) {
	variable -= 1;	//subtract one because the variable is zero addressed
	_decLoc = strstr(_commandString, "DEC");	//find location of DEC in the current command string
	if(strstr(_commandString, "DEC") == NULL) {	//if the command string isn't displaying a decimal
		editBounds = false;						//do not edit bounds
	}
	int count = 0;	//variable for counting decimal places
	double number = actual;	//temp value copy
	number = number < 0 ? number * -1 : number;	//flip number's sign if negative
	number -= (int)(number);	//subtract the whole number portion
	while(number >= 0.01 && number < 0.999) {	//while approximately not zero
		number *= 10;	//shift left
		number -= (int)(number);	//subtract what was shifted
		count++;	//increment counter
	}
	long int result = actual * pow(10, count);	//shift the float so that it can be casted
	//without truncating digits past the decimal

	/*  debug
	Serial.print("Actual: ");
	Serial.println(actual);
	Serial.print("Count: ");
	Serial.println(count);
	Serial.print("Result: ");
	Serial.println(result);
	Serial.println();
	*/

	if(editBounds) {	//option to account for changes in decimal places
		int digits = 0;	//digit counter
		long int temporary = result;	//temp value copy
		while(temporary) {	//while not zero
			temporary /= 10;	//shift to the right
			digits++;	//increment counter
		}

		if(digits < 10) {	//targeted value is in index 8
			if((_decLoc[8]) - 48 != count) {	//if the current decimal places doesn't match the new number's
				_decLoc[8] = (count) + 48;	//change the decimal places on the sign
			}
		}
		else {	//targeted value is in index 9
			if((_decLoc[9]) - 48 != count) {
				_decLoc[9] = (count) + 48;
			}
		}
		VMDisplay::writeMessage(_commandString);	//transfer the new command string into _commandData
		VMDisplay::sendMessage();	//send _commandData to the display
	}

	VMDisplay::updateDecimal(variable + 1, result, editBounds);	//send the shifted value to the int overload of this function
}

/*******************************************************************************
Description: Update one of the display's stored decimal variables. This overload
			 is meant to be used with integer values (whole numbers). This
			 function will adjust the digit length automatically unless otherwise
			 specified. If bounds are adjusted, the message is rewritten to the
			 sign, resetting its position to start.

Parameters: -int variable - Specifies which of the display's 32 decimal variables
			 you are updating.
			-long int number - The long integer value to update the variable with.
			-bool editBounds - Enable or disable bound adjustments (enabled defaultly).

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::updateDecimal(int variable, long int number, bool editBounds) {
	variable -= 1;	//subtract one because the variable is zero addressed
	_decLoc = strstr(_commandString, "DEC");
	if(strstr(_commandString, "DEC") == NULL) {
		editBounds = false;
	}
	if(editBounds) {	//option to account for changes in total digits
		int digits = 0;
		long int temporary = number;
		while(temporary) {
			temporary /= 10;
			digits++;
		}
		if(!digits) {	//a syntax error is displayed if 0 is set for digits
			editBounds = false;	//do not edit bounds
		}
		//conditions for if the decimal bounds need to be shifted
		if((digits < 10 && (_decLoc[6] - 48 != digits || _decLoc[7] != ' ')) || (digits > 9 && _decLoc[7] != 48 + (digits - (10 * (digits / 10))))) {
			if(digits < 10) {	//one number needs to be changed
				_decLoc[6] = 48 + digits;	//set current digits to the amount in the number
				if(_decLoc[7] != ' ') {	//check if the number of digits was greater than 9 previously
					char temp[256];	//array to contain all characters after the number that was written
					for(int i = 0; i < strlen(_decLoc); i++) {	//shift the data after what was written to the left
						temp[i] = _decLoc[8 + i];
					}
					for(int i = 0; i < strlen(_decLoc) - 7; i++) {
						_decLoc[7 + i] = temp[i];
					}
				}
			}
			else {	//if the digits are 10 or 11 (>11 is not allowed)
				if(!(strlen(_decLoc) < 0)) {
					char temp[256];
					int places = digits;
					for(int i = 0; i < strlen(_decLoc); i++) {
						temp[i] = _decLoc[7 + i];
					}
					_decLoc[6] = 48 + (digits / 10);	//write 10s digit
					_decLoc[7] = 48 + (digits - (10 * (digits / 10)));	//write 1s digit
					for(int i = 0; i < strlen(_decLoc) - 7; i++) {	//shift data after written data to the right
						_decLoc[8 + i] = temp[i];
					}
				}
			}
			VMDisplay::writeMessage(_commandString);	//transfer the new command string into _commandData
			VMDisplay::sendMessage();	//send _commandData to the display
		}
	}
	VMDisplay::connect();	//ensure connection to the correct display
	VMClient.holdingRegisterWrite(100 + (variable * 2), number & 0xFFFF);	//write high byte into variable's register
	VMClient.holdingRegisterWrite(99 + (variable * 2), (number >> 16) & 0xFFFF);	//write low byte into variable's register
}

/*******************************************************************************
Description: Update one of the display's stored string variables. This
			 function will adjust the bounds of the character length automatically
			 unless otherwise specified. If bounds are adjusted, the message is
			 rewritten to the sign, resetting its position to start.

Parameters: -int variable - Specifies which of the display's 16 string variables
			 you are updating.
			-const char text[] - Enter a string literal of up to 100 characters to
			 update the variable with.
			-bool editBounds - Enable or disable bound adjustments (enabled defaultly).

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::updateStringVar(int variable, const char text[100], bool editBounds) {
	variable -= 1;	//subtract one because the variable is zero addressed
	_strLoc = strstr(_commandString, "STR");	//find location of DEC in the current command string
	if(strstr(_commandString, "STR") == NULL) {	//if the command string doesn't contain STR
		editBounds = false;						//do not edit bounds
	}
	int chars = strlen(text);	//total characters in text to write
	uint16_t temp[50];	//array of data to write
	memset(temp, 0, 50);	//clear array
	int tempPos = 0;	//index tracker in temp
	if(editBounds) {	//option to account for changes in character length
		if((chars < 10 && (_strLoc[6] - 48 != chars || _strLoc[7] != '>')) || (chars > 9 && _strLoc[7] != 48 + (chars - (10 * (chars / 10))))) {
			//conditions for if the character bounds need to be shifted
			if(chars < 10) {	//check if one char needs to be rewritten
				_strLoc[6] = 48 + chars;	//write char count
				while(_strLoc[7] != '>') {	//shift left until end of field is found
					char temp[256];
					for(int i = 0; i < strlen(_strLoc); i++) {
						temp[i] = _strLoc[8 + i];
					}
					for(int i = 0; i < strlen(_strLoc) - 7; i++) {
						_strLoc[7 + i] = temp[i];
					}
				}
			}
			else if(chars < 100) {	//check if two characters needs to be rewritten
				if(!(strlen(_strLoc) < 0)) {
					char temp[256];
					for(int i = 0; i < strlen(_strLoc); i++) {	//store data past digit to be written
						temp[i] = _strLoc[7 + i];
					}
					_strLoc[6] = 48 + (chars / 10);	//write char count 10s digit
					_strLoc[7] = 48 + (chars - (10 * (chars / 10))); //write char count 1s digit
					for(int i = 0; i < strlen(_strLoc) - 7; i++) {	//rewrite data to the right of what's been written
						_strLoc[8 + i] = temp[i];
					}
				}
			}
			else if(chars == 100) {	//check if three characters needs to be rewritten (only 100 chars are allowed)
				if(!(strlen(_strLoc) < 0)) {
					char temp[256];
					for(int i = 0; i < strlen(_strLoc); i++) {	//store data past digit to be written
						temp[i] = _strLoc[7 + i];
					}
					_strLoc[6] = 49;	//write a 1
					_strLoc[7] = 48;	//write a 0
					_strLoc[8] = 48;	//write a 0
					for(int i = 0; i < strlen(_strLoc) - 7; i++) {	//rewrite data to the right of what's been written
						_strLoc[9 + i] = temp[i];
					}
				}
			}
			VMDisplay::writeMessage(_commandString);	//transfer the new command string into _commandData
			VMDisplay::sendMessage();	//send _commandData to the display
		}
	}
	for(int i = 0; i < chars; i += 2) { //iterate and combine each two characters in an index of temp
		if(i + 1 > chars - 1) {
			temp[tempPos] = (uint16_t)(text[i]);
			tempPos++;
			break;
		}
		else {
			temp[tempPos] = ((uint16_t)(text[i + 1]) * 256) + (uint16_t)(text[i]);
		}
		tempPos++;
	}
	for(int i = 0; i < (sizeof(temp) / sizeof(uint16_t)); i++) {	//clear extra indexes
		if(i > tempPos) {
			temp[i] = 0;
		}
	}
	VMDisplay::connect();	//ensure connection to the correct display
	VMClient.beginTransmission(HOLDING_REGISTERS, 199 + (variable * 50), 50);	//write string to display's registers
	for (int i = 0; i < 50; i++) {
		VMClient.write(temp[i]);
	}
	VMClient.endTransmission();
}

/*******************************************************************************
Description: Write characters at the end of a char array passed to the function. The
			 function returns how many characters it has written after completing.

Parameters: -int startPos - The array index to start writing entered characters.
			-const char text[] - Enter a string literal to be written into the array.
			-char *arr - A pointer to the char array you are writing the characters to.

Returns: 	-Number of characters written to the char array.

Example Code:
*******************************************************************************/
int VMDisplay::writeToArr(int startPos, const char text[], char *arr) {
	int charsWritten = 0;	//counter for characters written
	for(int i = startPos; i < startPos + strlen(text); i++) {
		arr[i] = text[i - startPos];	//write new char at the end of the array
		charsWritten++;	//increment counter
	}
	return charsWritten;
}

/*******************************************************************************
Description: Configure the ASCII array with data concerning the custom settings
			 of each enabled line.

Parameters: -int startPos - The array index to begin appending line-specific
			 characters to.
			-int lineSelected - The line that contains the data being used to
			 write the information.
			-char *arr A pointer to the char array you are writing line-specific
			 data to.

Returns: 	-The amount of characters written to the pointed array.

Example Code:
*******************************************************************************/
int VMDisplay::lineConfig(int startPos, int lineSelected, char *arr) {
	int pos = startPos;
	bool colorRetained;
	bool winRetained;
	int linesClearBelow = 0;
	int clearPixels = 0;
	const int lineHeight = lineSelected * 8;
	if(lineSelected != 0) {	//if the line above has the same color or scrollType, data can be retained
		colorRetained = (line[lineSelected].color == line[lineSelected - 1].color) && line[lineSelected - 1].isEnabled;
		winRetained = (line[lineSelected].scrollType == line[lineSelected - 1].scrollType) && line[lineSelected - 1].isEnabled;
	}
	else {	//line 1 (specified by 0) retains nothing
		colorRetained = false;
		winRetained = false;
	}
	for(int i = 1; i < (4 - lineSelected); i++) {	//increment lines clear below for every disabled or scroll-matching line below
		if((line[lineSelected].scrollType == line[lineSelected + i].scrollType) || !line[lineSelected + i].isEnabled) {
			linesClearBelow++;
		}
		else {
			break;	//leave loop as soon as a non-clear line is found
		}
	}
	clearPixels = (lineHeight + 8 + (linesClearBelow * 8)) - 1;	//pixels to be specified for window height
	if(!winRetained) {	//is the line's window hasn't been retained
		pos +=	writeToArr(pos, "<WIN 0 ", arr);	//write "<WIN 0 " at end of array
		if(lineSelected < 2) {	//single digit line height check
			arr[pos] = 48 + lineHeight;	//write digit at end of array
			pos++;	//increment pos
		}
		else {	//two digit line height
			arr[pos] = 48 + (lineHeight / 10);	//write 10s digit
			pos++;
			arr[pos] = lineHeight - ((lineHeight / 10) * 10) + 48;	//write 1s digit
			pos++;
		}
		pos +=	writeToArr(pos, " 287 ", arr);	//write window width of max
		arr[pos] = 48 + (clearPixels / 10);	//write window height
		pos++;
		arr[pos] = clearPixels - ((clearPixels / 10) * 10) + 48;
		pos++;
		pos += writeToArr(pos, ">", arr);
	}

	pos += writeToArr(pos, "<POS 0 ", arr);	//line starting position
	if(lineSelected < 2) {
			arr[pos] = 48 + lineHeight;
			pos++;
		}
	else {
		arr[pos] = 48 + (lineHeight / 10);
		pos++;
		arr[pos] = lineHeight - ((lineHeight / 10) * 10) + 48;
		pos++;
	}
	pos += writeToArr(pos, ">", arr);	//close field

	if(!winRetained || (line[lineSelected].scrollType >= 4 && line[lineSelected].scrollType <= 6)) {	//window being retained also means scroll is retained
		switch(line[lineSelected].scrollType) {	//use default scroll type or type defined by user
			case 0:
				pos += writeToArr(pos, "<SL>", arr);
				break;
			case 1:
				pos += writeToArr(pos, "<SR>", arr);
				break;
			case 2:
				pos += writeToArr(pos, "<SU>", arr);
				break;
			case 3:
				pos += writeToArr(pos, "<SD>", arr);
				break;
			case 4:
				pos += writeToArr(pos, "<LJ>", arr);
				break;
			case 5:
				pos += writeToArr(pos, "<CJ>", arr);
				break;
			case 6:
				pos += writeToArr(pos, "<RJ>", arr);
				break;
		}
		if(line[lineSelected].scrollType < 4) {		//if the line is set to scroll
			pos += writeToArr(pos, "<S ", arr);	//write its scroll speed as defined
			if(line[lineSelected].scrollSpeed == 0) {
				pos += writeToArr(pos, "S>", arr);
			}
			else if(line[lineSelected].scrollSpeed == 1) {
				pos += writeToArr(pos, "M>", arr);
			}
			else if(line[lineSelected].scrollSpeed == 2) {
				pos += writeToArr(pos, "F>", arr);
			}
		}
	}

	pos += writeToArr(pos, "<BL ", arr);	//write blink speed (if any)
	switch(line[lineSelected].blink) {	//use variable defined defaultly or by user
		case 0:
			pos += writeToArr(pos, "S>", arr);
			break;
		case 1:
			pos += writeToArr(pos, "M>", arr);
			break;
		case 2:
			pos += writeToArr(pos, "F>", arr);
			break;
		case 3:
			pos += writeToArr(pos, "N>", arr);
			break;
	}

	pos += writeToArr(pos, "<CS ", arr);	//write text size (Character Set)
	if(line[lineSelected].textSize > 9) {	//two digit size
		arr[pos] = 48 + (line[lineSelected].textSize / 10);
		pos++;
		arr[pos] = line[lineSelected].textSize - ((line[lineSelected].textSize / 10) * 10) + 48;
		pos++;
	}
	else {	//one digit size
		arr[pos] = 48 + line[lineSelected].textSize;
		pos++;
	}
	pos += writeToArr(pos, ">", arr);

	if(!colorRetained) {	//if color wasn't retained
		if(line[lineSelected].color == 0) {	//use variable defined defaultly or by user
			pos += writeToArr(pos, "<GRN>", arr);
		}
		else if(line[lineSelected].color == 1) {
			pos += writeToArr(pos, "<RED>", arr);
		}
		else if(line[lineSelected].color == 2) {
			pos += writeToArr(pos, "<AMB>", arr);
		}
	}

	pos += writeToArr(pos, "<T>", arr);	//open text field
	pos += writeToArr(pos, line[lineSelected].text, arr);	//write user-defined text (defaultly "")
	pos += writeToArr(pos, "</T>", arr);	//close text field

	pos-= 11;

	return pos;
}

/*******************************************************************************
Description: Uses data stored in the object's members (and the line's members)
			 to fill the char array pointed to with the command message.

Parameters: -char *string - A pointer to the char array to be loaded with the
			 ASCII command message.

Returns: 	-The amount of characters written to the specified array.

Example Code:
*******************************************************************************/
int VMDisplay::generateString(char *string) {
	char temp[512];	//temp array to contain the command string
	memset(temp, 0x00, 512);	//clear array
	int tempPos = 0;	//counter for array position

	tempPos += VMDisplay::writeToArr(tempPos, "<ID ", temp);	//start ID field
	if(_ID > 99) {	//three chars need to be written
		temp[tempPos] = 48 + (_ID / 100);
		tempPos++;
		temp[tempPos] = 48 + (_ID - ((_ID / 100) * 100)) / 10;
		tempPos++;
		temp[tempPos] = 48 + (_ID - ((_ID / 10) * 10));
		tempPos++;
	}
	else if(_ID > 9) {	//two chars need to be written
		temp[tempPos] = 48 + (_ID / 10);
		tempPos++;
		temp[tempPos] = 48 + (_ID - (10 * (_ID / 10)));
		tempPos++;
	}
	else {	//one char needs to be written
		temp[tempPos] = 48 + _ID;
		tempPos++;
	}
	tempPos += VMDisplay::writeToArr(tempPos, "><CLR>", temp);	//write clear field
	if(_maintenanceCommand != 4) {	//a maintenance command being set will overwrite any lines defined
		tempPos += VMDisplay::writeToArr(tempPos, "<MTN ", temp);
		temp[tempPos] = 48 + _maintenanceCommand;
		tempPos++;
		tempPos += VMDisplay::writeToArr(tempPos, ">", temp);
	}
	else {	//no maintenance command has been set
		for(int i = 0; i < 4; i++) {	//pass temp to lineConfig for each line enabled
			if(line[i].isEnabled) {
				tempPos += VMDisplay::lineConfig(tempPos, i, temp);
			}
		}
	}
	int stringPos = 0;	//position of array being passed into the function
	memset(_commandString, 0, 512);
	for(int i = 0; i < 511; i++) {	//write every char > 0x19 into the string passed in
		if(temp[i] > 0x19) {
			string[stringPos] = temp[i];
			_commandString[stringPos] = temp[i];
			stringPos++;
		}
	}

	return tempPos;
}

/*******************************************************************************
Description: After the generateString function fills the char array with ASCII
			 data for the display, this function prepares it to be written through
			 modbus by flipping each two characters and putting them into a single
			 uint16_t contained in the _commandData array. This overload is called
			 with no parameters, and uses data previously specified using the
			 line-specific commands.

Parameters: -None

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::writeMessage() {
	uint16_t temp[512];	//array containing data to be sent through modbus
	int tempPos = 0;	//counter for temp's position
	char text[512];	//array containing the command string
	memset(text, 0, 512);	//clear text
	memset(temp, 0, 512);	//clear temp
	memset(_commandData, 0, 512);	//clear message
	VMDisplay::generateString(text);	//write command string into text
	bool endedOdd = false;	//flag when every char cannot be paired evenly
	char terminate[] = { 0x0D, 0x0D, 0xCC };	//end carriage chars
	_completed = false;	//flag the message as not being sent
	for(int i = 0; i < strlen(text); i += 2) {	//pair each two chars together and add them to temp
		if(i + 1 > strlen(text) - 1) {	//if the program is going to reach out of bounds
			//set endedOdd to true, and write the all three end carriage chars with the last text char
			temp[tempPos] = ((uint16_t)(terminate[0]) * 256) + (uint16_t)(text[i]);
			temp[tempPos + 1] = ((uint16_t)(terminate[2]) * 256) + (uint16_t)(terminate[1]);
			tempPos += 2;
			endedOdd = true;
			break;
		}
		else {	//pair and write normally
			temp[tempPos] = ((uint16_t)(text[i + 1]) * 256) + (uint16_t)(text[i]);
			tempPos++;
		}
	}
	if(!endedOdd) {	//after ending even, write the first two end carriage chars
		temp[tempPos] = ((uint16_t)(terminate[1]) * 256) + (uint16_t)(terminate[0]);
		tempPos++;
	}
	for(int i = 0; i < (sizeof(temp) / sizeof(uint16_t)); i++) {
		if(i > tempPos) {	//clear any unused indexes
			temp[i] = 0;
		}
		_commandData[i] = temp[i];	//write temp into _commandData
	}
	messageLength = tempPos;	//set messageLength to tempPos
}

/*******************************************************************************
Description: This function prepares the data passed to be written through modbus
			 by flipping each two characters and putting them into a single
			 uint16_t contained in the _commandData array. This overload is run by
			 passing the entire command string in as a string literal. This is intended
			 to make it easier to generate and edit the command string directly to
			 produce more custom results.
			 Documentation on how to write the ViewMarq command string can be found
			 at: https://cdn.automationdirect.com/static/manuals/mduserm/appxa.pdf

Parameters: -const char text[] - A string literal containing the ASCII command string
			 typed by hand, copied from the ViewMarq software, or taken from this
			 program's printMessage function.

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::writeMessage(const char text[]) {
	uint16_t temp[512];
	int tempPos = 0;
	memset(temp, 0, 512);
	if((int)text != (int)&_commandString) {	//prevent from wiping input in case the input is _commandString
		memset(_commandString, 0, 512);
		strcpy(_commandString, text);
	}
	memset(_commandData, 0, 512);
	bool endedOdd = false;
	char terminate[] = { 0x0D, 0x0D, 0xCC };
	strcpy(_commandString, text);
	_completed = false;
	for(int i = 0; i < strlen(text); i += 2) {
		if(i + 1 > strlen(text) - 1) {
			temp[tempPos] = ((uint16_t)(terminate[0]) * 256) + (uint16_t)(text[i]);
			temp[tempPos + 1] = ((uint16_t)(terminate[2]) * 256) + (uint16_t)(terminate[1]);
			tempPos += 2;
			endedOdd = true;
			break;
		}
		else {
			temp[tempPos] = ((uint16_t)(text[i + 1]) * 256) + (uint16_t)(text[i]);
		}
		tempPos++;
	}
	if(!endedOdd) {
		temp[tempPos] = ((uint16_t)(terminate[1]) * 256) + (uint16_t)(terminate[0]);
		tempPos++;
	}
	for(int i = 0; i < (sizeof(temp) / sizeof(uint16_t)); i++) {
		if(i > tempPos) {
			temp[i] = 0;
		}
		_commandData[i] = temp[i];
	}
	messageLength = tempPos;
}

void VMDisplay::resetMessage() {
	for(int i = 1; i < 5; i++) {
		VMDisplay::setText(i, "");
		VMDisplay::setColor(i, 0);
		VMDisplay::setTextSize(i, 0);
		VMDisplay::setScrollType(i, 4);
		VMDisplay::setScrollSpeed(i, 1);
		VMDisplay::setBlink(i, 3);
	}
	_completed = false;
	messageLength = 0;
	memset(_commandString, 0, 512);
	memset(_commandData, 0, 512);
}

/*******************************************************************************
Description: Prints out the command message to the arduino console. The intent of
			 this function is to allow for troubleshooting, or to enable users to
			 copy the generated command message and then modify it to their needs
			 with the alternate writeMessage overload.

Parameters: -None

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::printMessage() {
	for(int i = 0; i < strlen(_commandString); i++) {
		Serial.print(_commandString[i]);	//print each char in the string
	}
	Serial.println();	//new line
}

/*******************************************************************************
Description: Returns the command string into a char array passed into the function
			 by the user. The intent of this function is to store the command string
			 so that users can pass it to the alternate writeMessage overload with
			 or without modifying it.

Parameters: -None

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::returnMessage(char *arr) {
	for(int i = 0; i < strlen(_commandString); i++) {
		arr[i] = _commandString[i];	//print each char in the array passed
	}
}

/*******************************************************************************
Description: Establishes connection with the Modbus TCP server to allow for
			 communications with the ViewMarq display.

Parameters: -None

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::changeIPAddress(IPAddress &ip) {
	for(int i = 0; i < 4; i++) {
		address[i] = ip[i];
	}
}

/*******************************************************************************
Description: Establishes connection with the Modbus TCP server to allow for
			 communications with the ViewMarq display.

Parameters: -None

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::connect() {
	VMClient.stop();	//disconnect modbus server
	if (!VMClient.begin(address)) {	//begin modbus server on this display's IP
		Serial.print("Modbus TCP Client on display with ID ");
		Serial.print(_ID);
		Serial.println(" failed!");
		delay(500);
		VMDisplay::connect();	//try and reconnect
	}
}

/*******************************************************************************
Description: Sends the data in the _commandData uint16_t array to the display
			 using the modbus TCP server. It does this in one or more transactions
			 of, at most, 246 bytes.

Parameters: -None

Returns: 	-None

Example Code:
*******************************************************************************/
void VMDisplay::sendMessage() {
	int commandPos = 0;	//counter for _commandData's position
	int messagesToWrite = 0;	//amount of transmissions to perform
	VMDisplay::connect();	//ensure connection to the correct modbus server
	for(int i = 0; i < 6; i++) {	//determine how many messages to write
		if(messageLength > 123 * i) {
			messagesToWrite++;
		}
	}
	if (!_completed) {	//make sure the current message hasn't already been written
		for(int i = 0; i < messagesToWrite; i++) {	//make a transmission of 123 uint16_ts for every message to write
			VMClient.beginTransmission(HOLDING_REGISTERS, 10999 + commandPos, 123);
			for (int j = 0; j < 124; j++) {
				if(!VMClient.write(_commandData[commandPos]) && i != 0) {
					/*
					Serial.print("Write of char failed: |");
					Serial.print(_commandData[commandPos]);
					Serial.print("| ");
					Serial.print("At position: ");
					Serial.println(i);
					*/
				}
				else {
					commandPos++;
				}
			}
			VMClient.endTransmission();
		}
		_completed = true;	//mark this message as being written
	}
}
