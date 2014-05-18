/*
 * Navchuk.cpp - Menu navigation based Wii Nunchuk library for Arduino
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
#include <Navchuk.h>

#define ADDRESS 0x52
#define CENTER_DEADBAND 10    //joystick counts for center deadband area
#define MOTION_THRESHOLD 10   //accelerometer delta counts for motion detection

#define DIGITAL_THRESHOLD 60
#define NUNCHUK_IDLE ' '//nothing selected
#define NUNCHUK_F 'F'   //joystick forward
#define NUNCHUK_B 'B'   //joystick backward
#define NUNCHUK_L 'L'   //joystick left
#define NUNCHUK_R 'R'   //joystick right
#define NUNCHUK_Z 'Z'   //Z button pressed
#define NUNCHUK_C 'C'   //C button pressed
#define NUNCHUK_M 'M'   //Motion detected from accelerometers

void Navchuk::init()
{
  Wire.begin();

  Navchuk::_sendByte(0x55, 0xF0);
  Navchuk::_sendByte(0x00, 0xFB);

  Navchuk::update();
}

void Navchuk::update()
{
  int count = 0;
  int values[6];

  Wire.requestFrom(ADDRESS, 6);

  while(Wire.available())
  {
    values[count] = Wire.read();
    count++;
  }
  Navchuk::_sendByte(0x00, 0x00);
  
// Process the X axis displacement and direction:
  analogDisplacementX = abs(128 - values[0]);
  if (analogDisplacementX < CENTER_DEADBAND)  //joystick is inside the center deadband
  {
    analogDisplacementX = 0;
    analogDirectionX = 0;
  }
  else  //joystick is shifted far enough off-center
  {
  	analogDisplacementX -= CENTER_DEADBAND;
	if (values[0] < 128) 
	  analogDirectionX = -1;  //joystick is shifted to the left
	else
	  analogDirectionX = 1;   //joystick is shifted to the right
  }

// Process the Y axis displacement and direction:  
  analogDisplacementY = abs(128 - values[1]);
  if (analogDisplacementY < CENTER_DEADBAND)  //joystick is inside the center deadband
  {
    analogDisplacementY = 0;
    analogDirectionY = 0;
  }
  else  //joystick is shifted far enough off-center
  {
  	analogDisplacementY -= CENTER_DEADBAND;
	if (values[1] < 128)
	  analogDirectionY = -1;  //joystick is shifted backward
	else
	  analogDirectionY = 1;   //joystick is shifted forward
  }

//Process data into menu navigation "keys"
//Higher priority functions go at the bottom of the function, as these are the last
//to be processed and will override any previous functions

  userInputState = NUNCHUK_IDLE; //default to no action

  // Process the accelerometer data for motion detection purposes only
  accelX = (values[2] << 2) | ((values[5] >> 2) & 3);
  accelY = (values[3] << 2) | ((values[5] >> 4) & 3);
  accelZ = (values[4] << 2) | ((values[5] >> 6) & 3);

  if (abs(accelY - accelPrevious) > MOTION_THRESHOLD)
    userInputState = NUNCHUK_M;
  accelPrevious = accelY;
  
// Process the four digital joystick directions:
  if (analogDisplacementX != 0 || analogDisplacementY != 0)
  { 
    // Process the X-axis
    if (analogDisplacementX > DIGITAL_THRESHOLD)
      {
        if (analogDirectionX > 0) userInputState = NUNCHUK_R;
        else userInputState = NUNCHUK_L;
      }

  // Process the Y axis:  
    if (analogDisplacementY > DIGITAL_THRESHOLD)
    {
      if (analogDirectionY > 0) userInputState = NUNCHUK_F;
      else userInputState = NUNCHUK_B;
    }
  }
// Process the Z pushbutton:
  if (!((values[5] >> 0) & 1))
    userInputState = NUNCHUK_Z;
  
// Process the C pushbutton:
  if (!((values[5] >> 1) & 1))
    userInputState = NUNCHUK_C;

// Process the edge sensing of the user inputs
  if(userInputState != userInputPrevious)
  {
    userInput = userInputState; //set the edge state when the value first changes
    userInputPrevious = userInputState;
  }
  else
  {
    userInput = NUNCHUK_IDLE; //clear the edge state once the values are the same
  }
}

void Navchuk::_sendByte(byte data, byte location)
{
  Wire.beginTransmission(ADDRESS);

  Wire.write(location);
  Wire.write(data);

  Wire.endTransmission();

  delay(10);
}