#ifndef LED_H
#define LED_H

#include <arduino.h>

class led
{
    private:
        int  pin;
        int  level = 180;
        bool state = false;
    public:
        led(int _pin = 255):pin(_pin)
        {
            if (pin != 255) pinMode(pin, OUTPUT);
        }
        void set_pin(int);
        void set_state(bool);
        void set_level(int);
        void refresh();
        int get_pin();
        bool get_state();
        int get_level();
};
#endif