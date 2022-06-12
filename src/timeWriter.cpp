#include "timeWriter.hpp"
#include <arduino.h>
#include <LiquidCrystal.h>

unsigned long runningTime = 0;
int loop_number = 0;
bool second_hide = false;
bool year_hide = false;

String month_titles[12] = {"Ocak", "Subat", "Mart", "Nisan", "Mayis", "Haziran", "Temmuz", "Agustos", "Eylul", "Ekim", "Kasim", "Aralik"};

String twodigitwriter(int value)
{
    String result;
    if (value < 10)
        result += "0";
    result += String(value);
    return result;
}

int daylen(int value)
{
    if (value < 10) return 2;
    else return 3;
}

void timeWriter(LiquidCrystal *screen, tmElements_t *time, int (*doitinsparetime)(bool))
{  
    
    for( ; ; )
    {
        if((millis() - runningTime) > 1000)
        {   
            runningTime = millis();
            loop_number++;
            if (loop_number == 60)
            {
                loop_number = 0;
                screen->begin(16,2);
            }
            if(RTC.read(*time))
            {
                int month_num = time->Month - 1;
                screen->clear();   
                screen->setCursor(0,1);
                screen->print(twodigitwriter(time->Hour));
                screen->print(".");
                screen->print(twodigitwriter(time->Minute));
                if(second_hide == false){ screen->print("."); screen->print(twodigitwriter(time->Second));} // will you write the seconds on the screen?
                screen->setCursor((16 - (daylen(time->Day) + month_titles[time->Month-1].length())),0);
                screen->print(time->Day);
                screen->print(" ");
                screen->print(month_titles[time->Month-1]);
                screen->setCursor(12,1);
                screen->print(1970 + time->Year);
                
            }
            else
            { 
                screen->clear();
                screen->setCursor(0,1);
                screen->print("--.--");
                if(second_hide == false) screen->print(".--");
                screen->setCursor(12,0);
                screen->print("---");
            }

            if((*doitinsparetime)(RTC.read(*time)) == 47)
            {
                break;
            }
        }
        else
        {
            if((*doitinsparetime)(RTC.read(*time)) == 47)
            {
                break;
            }
        }
        
    }
}
