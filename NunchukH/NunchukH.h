/*
 * NunchukH.h - Even more improved Wii Nunchuk library for Arduino
 *
 * Based on the following resources:
 *   http://www.gabrielbianconi.com/projects/arduinonunchuk/
 *   http://www.windmeadow.com/node/42
 *   http://todbot.com/blog/2008/02/18/wiichuck-wii-nunchuck-adapter-available/
 *   http://wiibrew.org/wiki/Wiimote/Extension_Controllers
 *
 */

#ifndef NunchukH_H
#define NunchukH_H

#include <Arduino.h>

class ArduinoNunchuk
{
  public:
    int analogX;
    int analogY;
    int displacementX;
    int displacementY;
	int accelX;
    int accelY;
    int accelZ;
    int zButton;
    int cButton;

    void init();
    void update();

  private:
    void _sendByte(byte data, byte location);
};

#endif
