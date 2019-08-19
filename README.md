
# ViewMarq Library
## Introduction
This library should be used with an Arduino Compatible device capable of handling TCP/IP connections and an [Automation Direct ViewMarq Message Display](https://www.automationdirect.com/adc/shopping/catalog/hmi_(human_machine_interface)/viewmarq_led_message_displays). This library allows for networked ViewMarq display(s) to be fully controlled through an Arduino Compatible device on the same network.

**Features:**
 - Control of one or more ViewMarq displays from a single Arduino Compatible device.
 - Display control through a user-input ASCII command string (like that generated in the official ViewMarq software).
 - Library functions that act as an editor for the display.
   - Functions construct an ASCII command string that can be returned to the user.
 - Control of string and decimal variables stored on the ViewMarq.

**When working with the ViewMarq display's ASCII command string, it is a huge help to look at the [official documentation](https://cdn.automationdirect.com/static/manuals/mduserm/appxa.pdf).**

## Usage
This library requires the following libraries to function. Please install these before using the ViewMarq libary.

```
#include <ArduinoRS485.h>  
#include <ArduinoModbus.h>  
```

To start, set up your TCP/IP client to communicate with the display (such as the EthernetClient or WiFiClient classes).
Create an IPAddress object with the IP of your ViewMarq display.
Create your VMDisplay object. Instantiate it with the display's ID (0 is universal), its TCP client, and its IPAddress:

`VMDisplay display(0, yourCommunicationClient, yourViewMarqIP);`

To communicate with multiple displays, you can either make multiple VMDisplay objects (simultaneous control) or use the changeIPAddress function to target a different display (single control).

Ensure you start your communication client in setup before attempting communications.
To display a message on your ViewMarq, you can use the functions provided to construct a message, or you can input your own, pre-constructed command string.

### To construct your own message:
Set the lines you want enabled (line 1 is enabled by default).

`display.setLine(2, true);`

Set the text for each line you want to display.

```
display.setText(1, "Hello");
display.setText(2, "World!");
```

Set each line's text size (0 by default).

```
display.setTextSize(1, 1);
display.setTextSize(2, 1);
```

Set each line's color (green by default)

```
display.setColor(1, RED);
display.setColor(2, AMBER);
```

Set each line's scroll type or alignment (left justified by default).

```
display.setScrollType(1, SCROLL_LEFT);
display.setScrollType(2, SCROLL_RIGHT);
```

Set each line's scroll speed (slow by default).
```
display.setScrollSpeed(1, SLOW);
display.setScrollSpeed(2, SLOW);
```

Set each line's blink speed, if any (off by default).

`display.setBlink(2, MEDIUM);`

Call writeMessage with no arguments to construct a message using the functions.

`display.writeMessage();`

**After customizing your message, you MUST use the writeMessage function to generate the ASCII command string to send.**
### To use a prewritten message:
Enter the message into the writeMessage function as a string.

`display.writeMessage("<ID 0><CLR><WIN 0 0 287  7><POS 0 0><SL><S S><BL N><CS 0><RED><T>Hello</T><WIN 0 8 287 31><POS 0 8><SR><S S><BL M><CS 0><AMB><T>World!</T>");`

**Note:** a prewritten message must be used in order to display one of the onboard decimal or string variables. The values of these variables can then be changed using the updateDecimal and updateStringVar functions. 
A message that displays variable 1 as a 6 character string:

`<ID 0><CLR><WIN 0 0 287 31><POS 0 0><CJ><BL N><CS 3><GRN><STR 1 6>`

### After writing your message through either method:
You can then send your message with the sendMessage function, where it will be displayed automatically.

`display.sendMessage();`

**Note:** multiple calls of sendMessage will not send any data unless the message has been changed and rewritten first.

## Examples
**The following examples are included with the library:**
 - **Ethernet_HelloWorld:** Displays "Hello World!" on a ViewMarq display using the Arduino Ethernet Library.
 - **WiFi_HelloWorld:** Displays "Hello World!" on a ViewMarq display using the Arduino WiFiNINA Library.
   - All examples besides this one use an ethernet client because of latency and connection time.
 - **Manual_HelloWorld:** Displays "Hello World!" on a ViewMarq display using a user-input ASCII command string.
 - **DecimalVariable_Counter:** Displays the seconds since the program began on a ViewMarq display using a decimal variable.
 - **DecimalVariable_TimeSinceStart:** Displays the time (seconds, minutes, hours) since the program began on a ViewMarq display using a decimal variable.
 - **StringVariable_HelloWorld:** Swap between displaying "Hello" and "World" on a ViewMarq display using a string variable.
