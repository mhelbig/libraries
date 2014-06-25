/*
 * NBtimer.h - Non-blocking timer library for Arduino
 *
 */

#ifndef NBtimer_H
#define NBtimer_H

#include <Arduino.h>

class NBtimer
{
  public:
 
  void init(long timeSet);
  void addTime(long additionalTime);
  boolean expired(void);
  long remaining(void);

  private:
  long timeSetting;
  long timeStamp;
};

#endif
