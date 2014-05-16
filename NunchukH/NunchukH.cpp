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

#include <Arduino.h>
#include <Wire.h>
#include <NunchukH.h>

#define ADDRESS 0x52
#define CENTER_DEADBAND 10

void ArduinoNunchuk::init()
{
  Wire.begin();

  ArduinoNunchuk::_sendByte(0x55, 0xF0);
  ArduinoNunchuk::_sendByte(0x00, 0xFB);

  ArduinoNunchuk::update();
}

void ArduinoNunchuk::update()
{
  int count = 0;
  int values[6];

  Wire.requestFrom(ADDRESS, 6);

  while(Wire.available())
  {
    values[count] = Wire.read();
    count++;
  }

  ArduinoNunchuk::analogX = values[0];
  
// Process the X axis functions:  
  ArduinoNunchuk::displacementX = abs(128 - values[0]);
  if (ArduinoNunchuk::displacementX < CENTER_DEADBAND)
  {
    ArduinoNunchuk::displacementX = 0;
  }
  else
  {
  	ArduinoNunchuk::displacementX -= CENTER_DEADBAND;
	if(values[0] < 128)
	{
	  ArduinoNunchuk::displacementX *= -1;
	}
  }

// Process the Y axis functions:  
  ArduinoNunchuk::displacementY = abs(128 - values[1]);
  if (ArduinoNunchuk::displacementY < CENTER_DEADBAND)
  {
    ArduinoNunchuk::displacementY = 0;
  }
  else
  {
  	ArduinoNunchuk::displacementY -= CENTER_DEADBAND;
	if(values[1] < 128)
	{
	  ArduinoNunchuk::displacementY *= -1;
	}
  }

  ArduinoNunchuk::accelX = (values[2] << 2) | ((values[5] >> 2) & 3);
  ArduinoNunchuk::accelY = (values[3] << 2) | ((values[5] >> 4) & 3);
  ArduinoNunchuk::accelZ = (values[4] << 2) | ((values[5] >> 6) & 3);
  ArduinoNunchuk::zButton = !((values[5] >> 0) & 1);
  ArduinoNunchuk::cButton = !((values[5] >> 1) & 1);

  ArduinoNunchuk::_sendByte(0x00, 0x00);
}

void ArduinoNunchuk::_sendByte(byte data, byte location)
{
  Wire.beginTransmission(ADDRESS);

  Wire.write(location);
  Wire.write(data);

  Wire.endTransmission();

  delay(10);
}