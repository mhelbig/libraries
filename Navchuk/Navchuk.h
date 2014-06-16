/*
 * Navchuk.h - Menu navigation based Wii Nunchuk library for Arduino
 *
 * Based on the following resources:
 *   http://www.gabrielbianconi.com/projects/arduinonunchuk/
 *   http://www.windmeadow.com/node/42
 *   http://todbot.com/blog/2008/02/18/wiichuck-wii-nunchuck-adapter-available/
 *   http://wiibrew.org/wiki/Wiimote/Extension_Controllers
 *
 */

#ifndef Navchuk_H
#define Navchuk_H

#include <Arduino.h>

class Navchuk
{
  public:
  char analogDisplacementX;   //how far off center
	char analogDirectionX;      //which direction + or -
  char analogDisplacementY;   //how far off center
	char analogDirectionY;      //which direction + or -
  char userInputState;        //enumerated value that reflects the 4 basic joystick directions
  char userInput;             //enumerated value for joystick directions and button presses
  
  void init();
  void update();

  private:
  char userInputPrevious;  //used for edge detection
	int accelX;
  int accelY;
  int accelZ;
  int accelPreviousX;
  int accelPreviousY;
  int accelPreviousZ;
/*  
  int zButtonState;
  int zButtonPrevious;
  int zButtonEdge;
  int cButtonState;
  int cButtonPrevious;
  int cButtonEdge;
*/  
  void _sendByte(byte data, byte location);
};

#endif
