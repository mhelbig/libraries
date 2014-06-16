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

#define ADDRESS 0x52  // I2C address for Nunchuk device

// A/D constants
#define CENTER_DEADBAND 10    //joystick counts for center deadband area
#define MOTION_THRESHOLD 20   //accelerometer delta counts for motion detection
#define DIGITAL_THRESHOLD 45  //analog displacement value needed to be considered active in a direction

// Timing constants
#define BUTTON_HELD_THRESHOLD 2000 //time in mS before a direction is considered held & reports a lower-case ASCII character
#define KEY_REPEAT_DELAY      650  //time in mS to wait before doing key repeat on any joystick axis
#define KEY_REPEAT_RATE       400  //time in mS that keys repeat at 
#define NUNCHUK_IDLE_DELAY  10000 //60000  //time in mS that nunchuk registered no movement and is considered idle
#define NUNCHUCK_IDLE_SAMPLE_TIME 250 //time in mS between accelerometer readings for movement detection

// direction definitions
#define NUNCHUK_NULL ' ' //nothing selected
#define NUNCHUK_F 'F'    //joystick forward
#define NUNCHUK_B 'B'    //joystick backward
#define NUNCHUK_L 'L'    //joystick left
#define NUNCHUK_R 'R'    //joystick right
#define NUNCHUK_Z 'Z'    //Z button pressed
#define NUNCHUK_C 'C'    //C button pressed
#define NUNCHUK_IDLE 'I' // Nunchuk has been idle for a while
#define NUNCHUK_MOVE 'M' // Nunchuck is being moved around

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
  static long repeatDelayTime;
  static long repeatRateTime;
  static long buttonHeldTime;
  static long nunchukIdleTime;
  static long nunchukIdleSampleTime;

// Serial.print("."); 
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

  userInputState = NUNCHUK_NULL; //default to no action unless overriden by data from Nunchuk

  // Process the accelerometer data to see if someone is using the nunchuk or it's sitting still
  accelX = (values[2] << 2) | ((values[5] >> 2) & 3);
  accelY = (values[3] << 2) | ((values[5] >> 4) & 3);
  accelZ = (values[4] << 2) | ((values[5] >> 6) & 3);

  if (abs(accelX - accelPreviousX) > MOTION_THRESHOLD ||
      abs(accelY - accelPreviousY) > MOTION_THRESHOLD ||
      abs(accelZ - accelPreviousZ) > MOTION_THRESHOLD ||
      userInputState != NUNCHUK_NULL)
    {
      nunchukIdleTime = millis() + NUNCHUK_IDLE_DELAY;
      userInputState = NUNCHUK_MOVE;
    }
  if (millis() > nunchukIdleSampleTime)
  {
    nunchukIdleSampleTime = millis() + NUNCHUCK_IDLE_SAMPLE_TIME;
      accelPreviousX = accelX;
      accelPreviousY = accelY;
      accelPreviousZ = accelZ;
  }
  if(millis() > nunchukIdleTime)
    {
      userInputState = NUNCHUK_IDLE;
    }

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
  if(userInputState != userInputPrevious) // we just changed to a new direction
  {
    userInput = userInputState; //set the edge state when the value first changes
    userInputPrevious = userInputState;
    repeatDelayTime = millis() + KEY_REPEAT_DELAY;
    buttonHeldTime = millis() + BUTTON_HELD_THRESHOLD;
  }
  else if (userInputState != ' ')   // it's being held in a direction and is not a null
  {
    userInput = NUNCHUK_NULL; //clear the edge state once the values are the same
    if (millis() > repeatDelayTime) //this means it's been held long enough to do the repeat
    {
// Process the key repeat function if it's time to send a repeat character
      if(millis() > repeatRateTime)
        {
          repeatRateTime = millis() + KEY_REPEAT_RATE;
          userInput = userInputState; //put a keystroke back in there (it'll get cleared the next time thru
        }  
    }
    if(millis() > buttonHeldTime)
      {
        userInput = userInputState + 32;  //Offset characters up to their lowercase ASCII value
        buttonHeldTime = millis() + BUTTON_HELD_THRESHOLD;
      }
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