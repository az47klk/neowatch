#ifndef TIME_WRITER_LIB
#define TIME_WRITER_LIB

#include <arduino.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <LiquidCrystal.h>

String twodigitwriter(int);
int daylen(int);
void timeWriter(LiquidCrystal*, tmElements_t*, int(bool));

#endif
