#include "led.hpp"
#include <arduino.h>


void led::set_pin(int _pin)
{
    pin = _pin;
    pinMode(pin, OUTPUT);
}

void led::set_state(bool _state) // led on-off state set function
{
    if (state != _state){
        state = _state;
        if (state == true) analogWrite(pin, level);
        else if (state == false) analogWrite(pin, 0);
    }
}

void led::set_level(int _level) // led bright level set function
{
    level = _level;
    if (state == true) analogWrite(pin, level);
}

void led::refresh()
{
    if (state)
    {
        analogWrite(pin, level);
    }
    else
    {
        analogWrite(pin, 0);
    }
}

int led::get_pin() 
{
    return pin;
}

bool led::get_state()
{
    return state;
}

int led::get_level()
{
    return level;
}

