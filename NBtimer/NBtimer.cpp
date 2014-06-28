/*
 * NBtimer.h - Non-blocking timer library for Arduino
 *
 */

#include <Arduino.h>
#include <NBtimer.h>

void NBtimer::init(long timeSet)
{
  timeStamp = millis();
  timeSetting = timeSet;
}

void NBtimer::addTime(long additionalTime)
{
  timeSetting += additionalTime;
}

boolean NBtimer::expired(void)
{
  if (millis() - timeStamp > timeSetting) return (1);
  else return (0);
}

long NBtimer::remaining(void)
{
  return (timeStamp - millis() + timeSetting);
}