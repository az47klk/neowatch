#include "rgbled.hpp"
#include <arduino.h>
#include "led.hpp"

void rgbled::set_state(bool _state)
{
    if (state != _state) state = _state;
    this->red.set_state(_state);
    this->green.set_state(_state);
    this->blue.set_state(_state);
}

void rgbled::set_level(int __red, int __green, int __blue)
{
    this->red.set_level(__red);
    this->green.set_level(__green);
    this->blue.set_level(__blue);
}

bool rgbled::get_state(){
    return state;
}