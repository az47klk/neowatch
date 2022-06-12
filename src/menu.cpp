#include "menu.hpp"
#include <arduino.h>
#include <LiquidCrystal.h>
#include <IRRemote.h>
#include <TimeLib.h>
#include <DS1307RTC.h>


void row_draw(int cursor, String row1, String row2, LiquidCrystal *screen)   // screen row drawer func
{
    screen->clear();
    if (cursor == UP) screen->setCursor(0,0);
    else if (cursor == DOWN) screen->setCursor(0,1);
    screen->print(">");
    screen->setCursor(1,0);
    screen->print(row1);
    screen->setCursor(1,1);
    screen->print(row2);
}

void row_draw(int cursor, String row1, int row2, LiquidCrystal *screen)   // screen row drawer func for choiser (using values as int)
{
    screen->clear();
    if (cursor == UP) screen->setCursor(0,0);
    else if (cursor == DOWN) screen->setCursor(0,1);
    screen->print(">");
    screen->setCursor(1,0);
    screen->print(row1);
    screen->setCursor(1,1);
    screen->print(row2);
}

void __hourDraw(LiquidCrystal *screen, int hour, int minute, int second, int cursor)
{
        screen->clear();
        if (cursor == 0) screen->setCursor(4,0);
        else if (cursor == 1) screen->setCursor(7,0);
        else if (cursor == 2) screen->setCursor(10,0);
        else screen->setCursor(4,0);
        screen->print("**");
        screen->setCursor(4,1);
        if(hour < 10) screen->print("0");
        screen->print(hour);
        screen->print(":");
        if(minute < 10) screen->print("0");
        screen->print(minute);
        screen->print(":");
        if(second < 10) screen->print("0");
        screen->print(second);
}

void __dateDraw(LiquidCrystal *screen, int day, int month, int year, int cursor)
{
        screen->clear();
        if (cursor == 0) screen->setCursor(3,0);
        else if (cursor == 1) screen->setCursor(6,0);
        else if (cursor == 2) screen->setCursor(9,0);
        else screen->setCursor(4,0);
        if (cursor != 2) screen->print("**");
        else screen->print("****");
        screen->setCursor(3,1);
        if(day < 10) screen->print("0");
        screen->print(day);
        screen->print("/");
        if(month < 10) screen->print("0");
        screen->print(month);
        screen->print("/");
        screen->print(year);
}

void menu(LiquidCrystal *screen, int maxValue, String menuTitles[], IRrecv *receiver, decode_results *result, 
    unsigned long up_button,  unsigned long down_button,  unsigned long ok_button,  unsigned long exit_button, unsigned long *menu_time, bool *close_command, void (*doityourself)(int), void (*looping)())
{   /// menu screen starter
    int firstValue = 0;
    int secondValue = 1;
    for ( ; ; ){
        bool this_loop_break_main = false;
        screen->clear();
        bool function_run = false;
        if (firstValue < secondValue)
            row_draw(UP, menuTitles[firstValue], menuTitles[secondValue], screen);
        else if (secondValue < firstValue)
            row_draw(DOWN, menuTitles[secondValue], menuTitles[firstValue], screen);

        for ( ; ; )
        {
            (*looping)();
            bool this_loop_break = false;
            if (*close_command) 
            {
                this_loop_break = true;
                this_loop_break_main = true;
            }
            if (receiver->decode(result))
            {
                receiver->resume();
                *menu_time = millis();
                if (result->value == up_button)
                {
                    secondValue = firstValue;
                    firstValue--;
                    if (firstValue < 0)
                    {
                        firstValue = (maxValue - 1);
                        secondValue = (maxValue - 2);
                        row_draw(DOWN, menuTitles[secondValue], menuTitles[firstValue], screen);
                    }
                    else row_draw(UP, menuTitles[firstValue], menuTitles[secondValue], screen);
                }
                else if (result->value == down_button)
                {
                    secondValue = firstValue;
                    firstValue++;
                    if (firstValue == maxValue)
                    {
                        firstValue = 0;
                        secondValue = 1;
                        row_draw(UP, menuTitles[firstValue], menuTitles[secondValue], screen);
                    }
                    else row_draw(DOWN, menuTitles[secondValue], menuTitles[firstValue], screen);
                }
                else if (result->value == ok_button)
                {
                    function_run = true;
                    this_loop_break = true;
                }
                else if (result->value == exit_button)
                {
                    this_loop_break = true;
                    this_loop_break_main = true;
                }
                else
                {
                    Serial.print("[W] Unknown button code: ");
                    Serial.println(result->value, HEX);
                }
            }
            if (this_loop_break) break;
        }
        
        if (function_run){
            function_run = false;
            (*doityourself)(firstValue);
        }
        
        if (this_loop_break_main) break;
    }
}

int writeAsNumber(LiquidCrystal *screen, IRrecv *receiver, decode_results *result, String title, int digit_number, 
                    unsigned long one,unsigned long two,unsigned long three,unsigned long four,unsigned long five,unsigned long six,
                     unsigned long seven, unsigned long eight, unsigned long nine, unsigned long zero, unsigned long left, unsigned long right,
                    unsigned long delete_button, unsigned long ok_button, unsigned long exit_button, unsigned long *menu_time, bool *close_command, void (*looping)())
{
    if (digit_number < 8)
    {
        screen->cursor();
        int cursor = 1;
        int value = 0;
        int digit[8] {11,10,10,10,10,10,10};

        row_draw(UP, title, "", screen);
        screen->setCursor(0, 1);
        screen->print(":");
        for( ; ; )
        {
            (*looping)();
            *menu_time = millis();
            if (receiver->decode(result))
            {
                receiver->resume();
                if (result->value == one){
                    digit[cursor] = 1;
                    screen->print("1");
                    cursor++;
                    if (cursor > digit_number) cursor = 1;
                    screen->setCursor(cursor, 1);

                }
                else if(result->value == two)  {
                    digit[cursor] = 2;
                    screen->print("2");
                    cursor++;
                    if (cursor > digit_number) cursor = 1;
                    screen->setCursor(cursor, 1);
                }
                else if(result->value == three){
                    digit[cursor] = 3;
                    screen->print("3");
                    cursor++;
                    if (cursor > digit_number) cursor = 1;
                    screen->setCursor(cursor, 1);
                }
                else if(result->value == four) {
                    digit[cursor] = 4;
                    screen->print("4");
                    cursor++;
                    if (cursor > digit_number) cursor = 1;
                    screen->setCursor(cursor, 1);
                }
                else if(result->value == five) {
                    digit[cursor] = 5;
                    screen->print("5");
                    cursor++;
                    if (cursor > digit_number) cursor = 1;
                    screen->setCursor(cursor, 1);
                }
                else if(result->value == six)  {
                    digit[cursor] = 6;
                    screen->print("6");
                    cursor++;
                    if (cursor > digit_number) cursor = 1;
                    screen->setCursor(cursor, 1);
                }
                else if(result->value == seven){
                    digit[cursor] = 7;
                    screen->print("7");
                    cursor++;
                    if (cursor > digit_number) cursor = 1;
                    screen->setCursor(cursor, 1);
                }
                else if(result->value == eight){
                    digit[cursor] = 8;
                    screen->print("8");
                    cursor++;
                    if (cursor > digit_number) cursor = 1;
                    screen->setCursor(cursor, 1);
                }
                else if(result->value == nine) {
                    digit[cursor] = 9;
                    screen->print("9");
                    cursor++;
                    if (cursor > digit_number) cursor = 1;
                    screen->setCursor(cursor, 1);
                }
                else if(result->value == zero) {
                    digit[cursor] = 0;
                    screen->print("0");
                    cursor++;
                    if (cursor > digit_number) cursor = 1;
                    screen->setCursor(cursor, 1);
                }
                else if(result->value == left) {
                    cursor--;
                    if (cursor < 1) cursor = digit_number;
                    screen->setCursor(cursor, 1);
                }
                else if(result->value == right){
                    if (digit[cursor] != 10) cursor++;
                    screen->setCursor(cursor, 1);
                }
                else if (result->value == delete_button)
                {
                    if (digit[cursor] != 10)
                    {
                        int not_null_value_number = 0;
                        for (int i = cursor + 1; i <= digit_number; i++ )
                        {
                            if (digit[i] != 10) not_null_value_number++;
                            else break;
                        }
                        for (int i = cursor; i <= (cursor + not_null_value_number); i++)
                        {
                            digit[i] = digit[i+1]; 
                        }
                    }
                    for (int i = cursor; i <= digit_number; i++)
                    {
                        screen->setCursor(i, 1);
                        if (digit[i] != 10) screen->print(digit[i]);
                        else screen->print(" ");
                    }
                    cursor--;
                    if (cursor < 1 ) cursor = 1;
                    screen->setCursor(cursor, 1);

                }
                else if(result->value == ok_button){
                    for (int i = 1; i <= digit_number; i++)
                    {
                        
                        if (digit[i] != 10)
                        {
                            value = value*10;
                            value += digit[i];
                        }
                        else
                        {
                           Serial.print(value); 
                           break;
                        } 
                        
                    }
                    break;
                }
                else if(result->value == exit_button){
                    break;
                }
                
                else
                {
                    Serial.println("[ERR] Unknown button code. (choiser_digit_writer_err)");
                }
            }
        }
        screen->noCursor();
        return value;
    }
    else return 300;
}

void choiser(LiquidCrystal *screen, int range, String titles[], int values[], IRrecv *receiver, decode_results *result, 
            unsigned long up_button,  unsigned long down_button,  unsigned long left_button,  unsigned long right_button, unsigned long ok_button, unsigned long exit_button, 
            int optional_value, int number_of_double_speed, unsigned long *menu_time, bool *close_command, void (*what_need_it_to_do)(int, int,int), void (*looping)())
{   // option set screen starter
    int choiser_selected_value = 0;
    bool loop_breaker = false;
    
    unsigned long time_of_wait = millis();
    int cache_value_direction;
    int cache_value_number_of_repeatition = 0;
    int cache_value_repeat_breaker = 0;

    row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
    for ( ; ; )
    {     
        (*looping)();
        if (*close_command) 
        {
            loop_breaker=true;
            (*what_need_it_to_do)(optional_value,choiser_selected_value,255);
        }
        if (receiver->decode(result))
        {
            receiver->resume();
            *menu_time = millis();
            if (result->value == up_button)
            {
                cache_value_direction = 255;
                choiser_selected_value--;
                if (choiser_selected_value < 0) choiser_selected_value = (range - 1);
                row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
            }
            else if (result->value == down_button)
            {
                cache_value_direction = 255;
                choiser_selected_value++;
                if(choiser_selected_value > (range - 1)) choiser_selected_value = 0;
                row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
            }
            else if (result->value == right_button)
            {
                cache_value_direction = UP;
                cache_value_number_of_repeatition = 1;
                (*what_need_it_to_do)(optional_value, choiser_selected_value, UP);
                row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
            }
            else if (result->value == left_button)
            {
                cache_value_direction = DOWN;
                cache_value_number_of_repeatition = 1;
                (*what_need_it_to_do)(optional_value, choiser_selected_value, DOWN);
                row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
            }
            else if (result->value == ok_button)
            {
                (*what_need_it_to_do)(optional_value, choiser_selected_value, CUSTOM); // sayıyla direk olarak yazma fonksiyonu tanımlanacak
                row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
            }
            else if (result->value == exit_button)
            {
                (*what_need_it_to_do)(optional_value,choiser_selected_value,255);
                loop_breaker = true;
            }
            else if (result->value == 0xFFFFFFFF)
            {
                //repeat func
                if (cache_value_direction == UP)
                {
                    if (cache_value_number_of_repeatition < number_of_double_speed)
                    {
                        cache_value_number_of_repeatition++;
                        (*what_need_it_to_do)(optional_value, choiser_selected_value, UP);
                        row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
                    }
                    else
                    {
                        cache_value_number_of_repeatition++;
                        (*what_need_it_to_do)(optional_value, choiser_selected_value, UPDOUBLESPEED);
                        row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
                    }
                } 
                else if(cache_value_direction == DOWN)
                {
                    if (cache_value_number_of_repeatition < number_of_double_speed)
                    {
                        cache_value_number_of_repeatition++;
                        (*what_need_it_to_do)(optional_value, choiser_selected_value, DOWN);
                        row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
                    }
                    else
                    {
                        cache_value_number_of_repeatition++;
                        (*what_need_it_to_do)(optional_value, choiser_selected_value, DOWNDOUBLESPEED);
                        row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
                    }
                }
            }
            else 
            {
                Serial.print("[W] Unknown button code: ");
                Serial.println(result->value, HEX);
            }
        }
  
        if (loop_breaker == true) break;
        
    }
}


void choiser(LiquidCrystal *screen, int range, String titles[], String values[], IRrecv *receiver, decode_results *result, 
            unsigned long up_button,  unsigned long down_button,  unsigned long left_button,  unsigned long right_button, unsigned long ok_button, unsigned long exit_button, 
            int optional_value, int number_of_double_speed, unsigned long *menu_time, bool *close_command, void (*what_need_it_to_do)(int, int,int), void (*looping)())
{   // option set screen starter
    int choiser_selected_value = 0;
    bool loop_breaker = false;
    
    unsigned long time_of_wait = millis();
    int cache_value_direction;
    int cache_value_number_of_repeatition = 0;
    int cache_value_repeat_breaker = 0;

    row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
    for ( ; ; )
    {     
        (*looping)();
        if (*close_command) 
        {
            loop_breaker=true;
            (*what_need_it_to_do)(optional_value,choiser_selected_value,255);
        }
        if (receiver->decode(result))
        {
            receiver->resume();
            *menu_time = millis();
            if (result->value == up_button)
            {
                cache_value_direction = 255;
                choiser_selected_value--;
                if (choiser_selected_value < 0) choiser_selected_value = (range - 1);
                row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
            }
            else if (result->value == down_button)
            {
                cache_value_direction = 255;
                choiser_selected_value++;
                if(choiser_selected_value > (range - 1)) choiser_selected_value = 0;
                row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
            }
            else if (result->value == right_button)
            {
                cache_value_direction = UP;
                cache_value_number_of_repeatition = 1;
                (*what_need_it_to_do)(optional_value, choiser_selected_value, UP);
                row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
            }
            else if (result->value == left_button)
            {
                cache_value_direction = DOWN;
                cache_value_number_of_repeatition = 1;
                (*what_need_it_to_do)(optional_value, choiser_selected_value, DOWN);
                row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
            }
            else if (result->value == ok_button)
            {
                (*what_need_it_to_do)(optional_value, choiser_selected_value, CUSTOM); // sayıyla direk olarak yazma fonksiyonu tanımlanacak
                row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
            }
            else if (result->value == exit_button)
            {
                (*what_need_it_to_do)(optional_value,choiser_selected_value,255);
                loop_breaker = true;
            }
            else if (result->value == 0xFFFFFFFF)
            {
                //repeat func
                if (cache_value_direction == UP)
                {
                    if (cache_value_number_of_repeatition < number_of_double_speed)
                    {
                        cache_value_number_of_repeatition++;
                        (*what_need_it_to_do)(optional_value, choiser_selected_value, UP);
                        row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
                    }
                    else
                    {
                        cache_value_number_of_repeatition++;
                        (*what_need_it_to_do)(optional_value, choiser_selected_value, UPDOUBLESPEED);
                        row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
                    }
                } 
                else if(cache_value_direction == DOWN)
                {
                    if (cache_value_number_of_repeatition < number_of_double_speed)
                    {
                        cache_value_number_of_repeatition++;
                        (*what_need_it_to_do)(optional_value, choiser_selected_value, DOWN);
                        row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
                    }
                    else
                    {
                        cache_value_number_of_repeatition++;
                        (*what_need_it_to_do)(optional_value, choiser_selected_value, DOWNDOUBLESPEED);
                        row_draw(UP, titles[choiser_selected_value], values[choiser_selected_value], screen);
                    }
                }
            }
            else 
            {
                Serial.print("[W] Unknown button code: ");
                Serial.println(result->value, HEX);
            }
        }
  
        if (loop_breaker == true) break;
        
    }
}


void timeDateSet(LiquidCrystal *screen, IRrecv *receiver, decode_results *result, tmElements_t *time, int time_or_date, 
                unsigned long up_button,  unsigned long down_button,  unsigned long left_button, unsigned long right_button, unsigned long ok_button, unsigned long exit_button,  unsigned long *menu_time, bool *close_command, void (*looping)())
{
    if (time_or_date == TIME)
    {
        int val_time[3];
        bool readable_time;
        if (RTC.read(*time))
        {
            val_time[0] = time->Hour;
            val_time[1] = time->Minute;
            val_time[2] = time->Second;
            readable_time = true;
        } 
        else 
        {
            val_time[0] = 0;
            val_time[1] = 0;
            val_time[2] = 0;
            readable_time == false;
        }
        int selected_value = 0;
        int max_val;
        __hourDraw(screen, val_time[0], val_time[1], val_time[2], selected_value);
        for ( ; ; ) 
        {
            (*looping)();
            if (receiver->decode(result))
            {
                receiver->resume();
                *menu_time = millis();
                if (result->value == up_button)
                {
                    val_time[selected_value]++;
                    if (selected_value == 0) max_val = 23;
                    else max_val = 59;
                    if (val_time[selected_value] > max_val) val_time[selected_value] = 0;
                    __hourDraw(screen, val_time[0], val_time[1], val_time[2], selected_value);
                }
                else if (result->value == down_button)
                {
                    val_time[selected_value]--;
                    if (selected_value == 0) max_val = 23;
                    else max_val = 59;
                    if (val_time[selected_value] < 0) val_time[selected_value] = max_val;
                    __hourDraw(screen, val_time[0], val_time[1], val_time[2], selected_value);
                }
                else if (result->value == left_button)
                {
                    selected_value--;
                    if (selected_value < 0 ) selected_value = 2;
                    __hourDraw(screen, val_time[0], val_time[1], val_time[2], selected_value);
                }
                else if (result->value == right_button)
                {
                    selected_value++;
                    if (selected_value > 2) selected_value = 0;
                    __hourDraw(screen, val_time[0], val_time[1], val_time[2], selected_value);
                }
                else if (result->value == ok_button)
                {
                   
                    if (readable_time) 
                    {
                        RTC.read(*time);
                        time->Hour = val_time[0];
                        time->Minute = val_time[1];
                        time->Second = val_time[2];
                        RTC.write(*time);
                    }
                    else
                    {
                        time->Hour   = val_time[0];
                        time->Minute = val_time[1];
                        time->Second = val_time[2];
                        time->Year = 0;
                        time->Day = 0;
                        time->Month = 0;
                        RTC.write(*time);
                    }
                    break;
                }
                else if (result->value == exit_button)
                {
                    break;
                }
                else 
                {
                    Serial.println("[ERR] Unknown button code;");
                }

            }
        }
    }
    else if (time_or_date == DATE)
    {
        int val_time[3];
        bool readable_time;
        if (RTC.read(*time))
        {
            val_time[0] = time->Day;
            val_time[1] = time->Month;
            val_time[2] = tmYearToCalendar(time->Year);
            readable_time = true;
        } 
        else 
        {
            val_time[0] = 1;
            val_time[1] = 1;
            val_time[2] = 2000;
            readable_time == false;
        }
        int selected_value = 0;
        int max_val;
        __dateDraw(screen, val_time[0], val_time[1], val_time[2], selected_value);
        for ( ; ; ) 
        {
            (*looping)();
            if (*close_command) break;
            if (receiver->decode(result))
            {
                receiver->resume();
                *menu_time = millis();
                if (result->value == up_button)
                {
                    val_time[selected_value]++;
                    if (selected_value != 2) if (val_time[selected_value] > max_val) val_time[selected_value] = 0;
                    else if (selected_value == 2) if (val_time[selected_value] > max_val) val_time[selected_value] = max_val;
                    __dateDraw(screen, val_time[0], val_time[1], val_time[2], selected_value);
                }
                else if (result->value == down_button)
                {

                    val_time[selected_value]--;
                    
                    if (selected_value != 2) if (val_time[selected_value] < 1) val_time[selected_value] = max_val;
                    else if (selected_value == 2) if (val_time[selected_value] < 2000) val_time[selected_value] = 1970;
                    __dateDraw(screen, val_time[0], val_time[1], val_time[2], selected_value);
                }
                else if (result->value == left_button)
                {
                    selected_value--;
                    if (selected_value < 0 ) selected_value = 2;
                    if (selected_value == 0)
                    {
                        if (val_time[1] == 1 && val_time[1] == 3 && val_time[1] == 5 && val_time[1] == 7 && val_time[1] == 8 && val_time[1] == 10 && val_time[1] == 12) max_val = 31;
                        else if (val_time[1] == 4 && val_time[1] == 6 && val_time[1] == 9 && val_time[1] == 11) max_val = 30;
                        else if (val_time[1] == 2)
                        {
                            if(val_time[3] % 4 == 0) max_val = 29;
                            else max_val = 28;
                        }
                    }
                    else if (selected_value == 1) max_val = 12;
                    else if (selected_value == 2) max_val = 2050;
                    __dateDraw(screen, val_time[0], val_time[1], val_time[2], selected_value);
                }
                else if (result->value == right_button)
                {
                    selected_value++;
                    if (selected_value > 2) selected_value = 0;
                    if (selected_value == 0)
                    {
                        if (val_time[1] == 1 && val_time[1] == 3 && val_time[1] == 5 && val_time[1] == 7 && val_time[1] == 8 && val_time[1] == 10 && val_time[1] == 12) max_val = 31;
                        else if (val_time[1] == 4 && val_time[1] == 6 && val_time[1] == 9 && val_time[1] == 11) max_val = 30;
                        else if (val_time[1] == 2)
                        {
                            if(val_time[3] % 4 == 0) max_val = 29;
                            else max_val = 28;
                        }
                    }
                    else if (selected_value == 1) max_val = 12;
                    else if (selected_value == 2) max_val = 2050;
                    __dateDraw(screen, val_time[0], val_time[1], val_time[2], selected_value);
                }
                else if (result->value == ok_button)
                {
                    
                    if (readable_time) 
                    {
                        RTC.read(*time);
                        time->Day = val_time[0];
                        time->Month = val_time[1];
                        time->Year = CalendarYrToTm(val_time[2]);
                        RTC.write(*time);
                    }
                    else
                    {
                        time->Hour   = 0;
                        time->Minute = 0;
                        time->Second = 0;
                        time->Day = val_time[0];
                        time->Month = val_time[1];
                        time->Year = CalendarYrToTm(val_time[2]);
                        RTC.write(*time);
                    }
                    break;
                }
                else if (result->value == exit_button)
                {
                    break;
                }
                else 
                {
                    Serial.println("[ERR] Unknown button code;");
                }

            }
        }
    }
}
