#ifndef RGBLEDIN_CAYINA_DEM_ATAM
#define RGBLEDIN_CAYINA_DEM_ATAM

#include <arduino.h>
#include "led.hpp"

class rgbled
{
    private:
        int r_pin,g_pin,b_pin;
        bool state = false;
        int r_lvl = 180, g_lvl = 180, b_lvl = 180;
        

    public:
        led red;
        led green;
        led blue;
        rgbled(int _r, int _g, int _b):r_pin(_r), g_pin(_g), b_pin(_b)
        {
           this->red.set_pin(_r);
           this->green.set_pin(_g);
           this->blue.set_pin(_b);
        }
        void set_state(bool);
        void set_level(int, int, int);
        bool get_state();
};

#endif