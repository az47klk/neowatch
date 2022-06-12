#ifndef MENUNUN_CAYINA_DEM_ATAM
#define MENUNUN_CAYINA_DEM_ATAM

#include <arduino.h>
#include <LiquidCrystal.h>
#include <IRRemote.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#define UP 1
#define DOWN 2
#define UPDOUBLESPEED 3
#define DOWNDOUBLESPEED 4
#define CUSTOM 5

#define TIME 1
#define DATE 2


void row_draw(int, String, String, LiquidCrystal*);
void row_draw(int, String, int, LiquidCrystal*);
void menu(LiquidCrystal*, int, String[], IRrecv*, decode_results*, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long*, bool*, void(int), void());
int writeAsNumber(LiquidCrystal*, IRrecv*, decode_results*, String, int, unsigned long , unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long , unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long , unsigned long, unsigned long, unsigned long*, bool*, void());

void choiser(LiquidCrystal*, int, String[], int[], IRrecv*, decode_results*, 
unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, 
int, int, unsigned long*, bool*, void(int, int,int), void());

void choiser(LiquidCrystal*, int, String[], String[], IRrecv*, decode_results*, 
unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, 
int, int, unsigned long*, bool*, void(int, int,int), void());

void timeDateSet(LiquidCrystal*, IRrecv *, decode_results*, tmElements_t*, int, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long, unsigned long*, bool*, void());

#endif