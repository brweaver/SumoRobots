# SumoRobots

Getting started with the [Zumo][1] Robot: 

1) Install the [Arduino IDE][2].

2) Install the [Pololu A-Star drivers][3] for the IDE.
	
To install the add-on, copy the "pololu" folder into the "hardware" subfolder of the Arduino sketchbook location.  Create the "hardware" subfolder if it doesn't already exist. The Arduino sketchbook location is typically in your Documents folder in a subfolder named "Arduino". You can see the sketchbook location in the Arduino IDE Preferences dialog, which is available from the File menu.

3) Install the [Zumo examples][4], but if you are using version 1.6.2 or later of the Arduino software (IDE), you can use the Library Manager to install the library:

* In the Arduino IDE, open the "Sketch" menu, select "Include Library", then "Manage Libraries...".
* Search for "Zumo32U4".
* Click the Zumo32U4 entry in the list.
* Click "Install".

[1]: https://www.pololu.com/product/2510
[2]: https://www.arduino.cc/en/Main/Software
[3]: https://github.com/pololu/a-star
[4]: https://github.com/pololu/zumo-32u4-arduino-library
