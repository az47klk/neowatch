#include <LiquidCrystal.h>
#include <Wire.h>
#include <TimeLib.h>
#include <IRremote.h>
#include <DS1307RTC.h>
#include <EEPROM.h>
#include "led.hpp"
#include "rgbled.hpp"
#include "menu.hpp"
#include "timeWriter.hpp"

// BUTTON DEFINES
#define POWEROFF_BUTTON 0x205D38C7
#define MUTE_BUTTON 0x205D28D7
#define ONE_BUTTON 0x205D906F
#define TWO_BUTTON 0x205DB847
#define THREE_BUTTON 0x205DF807
#define FOUR_BUTTON 0x205DB04F
#define FIVE_BUTTON 0x205D9867
#define SIX_BUTTON 0x205DD827
#define SEVEN_BUTTON 0x205D8877
#define EIGHT_BUTTON 0x205DA857
#define NINE_BUTTON 0x205DE817
#define ZERO_BUTTON 0x205D48B7
#define LOOP_BUTTON 0x205D22DD
#define SCAN_BUTTON 0x205D00FF
#define MENU_BUTTON 0x205DCA35
#define EXIT_BUTTON 0x205D18E7
#define UP_BUTTON 0x205DC03F
#define DOWN_BUTTON 0x205D40BF
#define LEFT_BUTTON 0x205D708F
#define RIGHT_BUTTON 0x205D58A7
#define OK_BUTTON 0x205DE01F
#define INFO_BUTTON 0x205D6897
#define EPG_BUTTON 0x205DA05F
#define RED_BUTTON 0x205DF20D
#define GREEN_BUTTON 0x205DEA15
#define YELLOW_BUTTON 0x205DDA25
#define BLUE_BUTTON 0x205DD22D

// things
LiquidCrystal lcd(27, 25, 23, 22, 24, 26);
IRrecv recvr(3);
decode_results result;
tmElements_t tm;

led screen_backlight(2);
rgbled backlight(4,5,6);
rgbled keyboardLight(7,8,11);

// menu titles 
const int c_rainbow_effect = 5;
String rainbow_effect[c_rainbow_effect] = {"Durum:", "Ust:", "Alt:", "Gecikme:", "Parlaklik:"};

const int c_light_menu = 4;
String light_menu[c_light_menu] = {"Arka Isiklar", "Klavye Isigi", "Ekran Isigi", "Gokkusagi"};

const int c_func_menu = 10;
String func_menu[c_func_menu] = {"Onayar 0", "Onayar 1","Onayar 2","Onayar 3","Onayar 4","Onayar 5","Onayar 6","Onayar 7","Onayar 8","Onayar 9"};

const int c_func_opt = 6;
String func_opt[c_func_opt] = {"Uygula", "Yalniz arka isik", "Yalniz alt isik", "Arka isigi kaydet", "Alt isigi kaydet","Sifirla"};

const int c_power_manager_menu = 6;
String power_manager_menu[c_power_manager_menu] = {"Ek Guc Durumu:", "Ek Guc Zmnlmasi: ", "Oto Kapanma: ", "Kapanma Saati: ", "Oto Acilma: ", "Acilma Saati: "};

const int c_main_menu = 5;
String main_menu[c_main_menu] = {"Isiklar", "On Ayarlar", "Guc yoneticisi", "Saat", "Tarih"};

const int c_rgb_choiser = 4;
String rgb_choiser[c_rgb_choiser] = {"Durum:","Kirmizi:", "Yesil:", "Mavi:"};

const int c_screen_bg_choiser = 6;
String screen_bg_choiser[c_screen_bg_choiser] = {"Normal Mod:", "Bekleme Modu:", "Zaman Asimi: ", "GunDogumu Saati", "GunBatimi Saati" , "Menu Kapatma"};


//--------> variables
int cache_choiser_values[20];
String cache_choiser_values_string[20];



bool rainbow_state = false;
bool rainbow_state_top = false;
bool rainbow_state_bottom = false;
int rainbow_delay_sec = 30;
bool rainbow_ran = false;
int rainbow_runtime_state = 1;
int section1 = 0;
int section2 = 255;
int section3 = 0;
int section_max_value = 255;
bool top_old_value = false;
bool bottom_old_value = false;
unsigned long rainbow_time;

bool power = false;                         // working cycle breaker
bool transformator_relay = false;           // relay status
int relay_timer_time = 20;                  // turn off relay after 20 second 
bool power_auto_turn_off = false;           // auto turn off power state
int  power_auto_turn_off_hour = 5;          // auto turn off power time as hour
bool power_auto_turn_on = false;            // auto turn on power state
int power_auto_turn_on_hour = 19;           // auto turn on power time as hour
bool power_custom_state = false;

int menu_close_delay = 30;
unsigned long __menu_close_timeout = 0;
bool global_menu_close = false;

unsigned long relay_timer = 0;              // it is for time calculate for millis() function.
unsigned long backlight_timeout_timer = 0;  // for screen backlight timer

int normal_brightness = 255;                // screen normal mode backlight brightness
int stand_by_brightness = 10;               // screen stand by mode backlight brightness
int normal_to_standby_timeout = 5;          // screen timeout 
int sunrise_time = 6;                       // stand by mode disable time
int sunset_time = 18;                       // stand by mode enable time

char *command;

int selected_function = 255;

int functions[c_func_menu][2][4] = {{{0,0,0,0},{0,0,0,0}}, // function menu
                                    {{0,0,0,0},{0,0,0,0}},
                                    {{0,0,0,0},{0,0,0,0}},
                                    {{0,0,0,0},{0,0,0,0}},
                                    {{0,0,0,0},{0,0,0,0}},
                                    {{0,0,0,0},{0,0,0,0}},
                                    {{0,0,0,0},{0,0,0,0}},
                                    {{0,0,0,0},{0,0,0,0}},
                                    {{0,0,0,0},{0,0,0,0}},
                                    {{0,0,0,0},{0,0,0,0}}}; 

// func prototypes 
String int_to_on_off(int state)
    {if (state == 1) return "Acik";else return "Kapali";}
String int_to_on_off(bool state)
    {if (state == true) return "Acik";else return "Kapali";}
bool int_to_bool(int a){ if (a == 0) return false; else return true;}


void eeprom_reader();
void eeprom_writer();
void relay_controller(bool);
void rainbow_effect_function(int);
void global_loop();
void rainbow_effect_choiser(int,int,int);
void led_choiser_function(int,int,int);
void light_menu_func(int);
void function_commander(int);
void function_menu_func(int);
void power_menu_function(int, int, int);
void main_menu_func(int);


int loop_for_time_screen_func();

void setup() 
{
    // hardware preparation phase
    Serial.begin(115200);
    Serial1.begin(115200);
    Serial.println("--------- NEOWATCH ---------");
    Serial.println();
    lcd.begin(16,2);
    Serial.println("[OK] LCD begin successfully.");
    recvr.enableIRIn();
    Serial.println("[OK] IR receiver begin successfully.");
    Serial.println("[INFO] HPP end.");
    pinMode(12,OUTPUT);
}

void loop() 
{
    // stand-by phase
    if (recvr.decode(&result))
    {
        recvr.resume();
        if (result.value == 0x205D38C7) power = true;
    }

    if (power)
    {
        // startup 
        lcd.begin(16,2);
        lcd.clear();
        lcd.home();
        lcd.setCursor(3,0);
        lcd.print("NEOWATCH");
        Serial.println("[OK] Startup screen printed.");

        Serial.println("[INFO] SUP end.");
        lcd.clear();
        screen_backlight.set_state(true);
        unsigned long counter = millis(); // millis for time
        eeprom_reader();
        // working cycles 
        for( ; ; )
        {
            for (int i = 0; i <= 255; i++){
                Serial.print("Address ");
                Serial.print(i);
                Serial.print(" -> ");
                Serial.println(EEPROM.read(i));
            }
            timeWriter(&lcd, &tm, &loop_for_time_screen_writer);
            
            

            if (power == false) 
            {
                lcd.clear();
                digitalWrite(12, LOW);
                screen_backlight.set_state(false);
                keyboardLight.set_state(false);
                backlight.set_state(false);
                break;
            }
        }
    }
}

void eeprom_reader()
{
    if (EEPROM.read(0) == 1)
    {
        int s_value = 14;
        // screen values
        normal_brightness = EEPROM.read(1);
        stand_by_brightness = EEPROM.read(2);
        normal_to_standby_timeout = EEPROM.read(3);
        sunrise_time = EEPROM.read(4);
        sunset_time = EEPROM.read(5);

        // backlight values
        backlight.set_state(int_to_bool(EEPROM.read(6)));
        backlight.set_level(EEPROM.read(7),EEPROM.read(8),EEPROM.read(9));

        //keyboardLight values
        keyboardLight.set_state(int_to_bool(EEPROM.read(10)));
        keyboardLight.set_level(EEPROM.read(11),EEPROM.read(12),EEPROM.read(13));

        //functions
        for (int i = 0; i <= 9; i++)
        {
            for (int j = 0; j <= 1; j++)
            {
                for (int k = 0; k <= 3; k++)
                {
                    functions[i][j][k] = EEPROM.read(s_value);
                    s_value++;
                }
            }
        }

        // power options
        relay_controller(int_to_bool(EEPROM.read(95)));
        relay_timer_time = EEPROM.read(96);
        power_auto_turn_off = int_to_bool(EEPROM.read(97));
        power_auto_turn_off_hour = EEPROM.read(98);
        power_auto_turn_on = int_to_bool(EEPROM.read(99));
        power_auto_turn_on_hour = EEPROM.read(100);
        power_custom_state = int_to_bool(EEPROM.read(101));

        //rainbow effects
        rainbow_state = int_to_bool(EEPROM.read(102));
        rainbow_state_top = int_to_bool(EEPROM.read(103));
        rainbow_state_bottom = int_to_bool(EEPROM.read(104));
        rainbow_delay_sec = EEPROM.read(105);
        section_max_value = EEPROM.read(107);
        top_old_value = int_to_bool(EEPROM.read(108));
        bottom_old_value = int_to_bool(EEPROM.read(109));

        //menu auto close
        menu_close_delay = EEPROM.read(110);
    }
}

void eeprom_writer()
{
    int s_value = 14;
    EEPROM.write(0,1);
    //screen backlight
    EEPROM.write(1, normal_brightness);
    EEPROM.write(2, stand_by_brightness);
    EEPROM.write(3, normal_to_standby_timeout);
    EEPROM.write(4, sunrise_time);
    EEPROM.write(5, sunset_time);

    //backlight values
    EEPROM.write(6, backlight.get_state());
    EEPROM.write(7, backlight.red.get_level());
    EEPROM.write(8, backlight.green.get_level());
    EEPROM.write(9, backlight.blue.get_level());

    //keyboardlight values
    EEPROM.write(10, keyboardLight.get_state());
    EEPROM.write(11, keyboardLight.red.get_level());
    EEPROM.write(12, keyboardLight.green.get_level());
    EEPROM.write(13, keyboardLight.blue.get_level());

    //functions
    for (int i = 0; i <= 9; i++)
    {
        for (int j = 0; j <= 1; j++)
        {
            for (int k = 0; k <= 3; k++)
            {
                EEPROM.write(s_value, functions[i][j][k]);
                s_value++;
            }
        }
    }

    //power options
    EEPROM.write(95, transformator_relay);
    EEPROM.write(96, relay_timer_time);
    EEPROM.write(97, power_auto_turn_off);
    EEPROM.write(98, power_auto_turn_off_hour);
    EEPROM.write(99, power_auto_turn_on);
    EEPROM.write(100, power_auto_turn_on_hour);
    EEPROM.write(101, power_custom_state);

    //rainbow effects
    EEPROM.write(102, rainbow_state);
    EEPROM.write(103, rainbow_state_top);
    EEPROM.write(104, rainbow_state_bottom);
    EEPROM.write(105, rainbow_delay_sec);
    EEPROM.write(107, section_max_value);
    EEPROM.write(108, top_old_value);
    EEPROM.write(109, bottom_old_value);

    //menu auto close
    EEPROM.write(110, menu_close_delay);
    
}

void relay_controller(bool state)
{
    if (state){
        if (!transformator_relay)
        {
            transformator_relay = true;
            digitalWrite(12, HIGH);
        }
    }
    else
    {
        if (transformator_relay)
        {
            transformator_relay = false;
            digitalWrite(12, LOW);
        }
    }
}

void global_loop()
{
    rainbow_effect_function(rainbow_delay_sec);
    if (transformator_relay && !power_custom_state) if (!(keyboardLight.get_state()) && !(backlight.get_state())) if ((relay_timer_time != 0) && (millis() - relay_timer > relay_timer_time*1000)){
        relay_timer = millis();
        relay_controller(false);
    }
    if (millis() - __menu_close_timeout == menu_close_delay*1000)
    {
        global_menu_close = true;
    }
    
}

int loop_for_time_screen_writer(bool time_available)
{   
    
    global_loop();
    global_menu_close = false;
    if (time_available) {
        if (!(tm.Hour >= sunrise_time && tm.Hour <= sunset_time)){
            if (millis() - backlight_timeout_timer > normal_to_standby_timeout*1000) screen_backlight.set_level(stand_by_brightness); // screen timeout condition
            Serial1.println("Nane");
        }
        else screen_backlight.set_level(normal_brightness);
        if (transformator_relay && !power_custom_state && (power_auto_turn_off && (tm.Hour >= power_auto_turn_off_hour && tm.Hour < power_auto_turn_on_hour)))
        {
            relay_controller(false);
            backlight.set_state(false);
            keyboardLight.set_state(false);
        }
        if (!transformator_relay && (power_auto_turn_on && !(tm.Hour >= power_auto_turn_off_hour && tm.Hour < power_auto_turn_on_hour))){
            relay_controller(true);
            backlight.set_state(true);
            keyboardLight.set_state(true);
        }
    }
    else if (millis() - backlight_timeout_timer > normal_to_standby_timeout*1000) screen_backlight.set_level(stand_by_brightness);
    
    

    if (Serial1.available())
    {
        String command;
        Serial1.println(Serial.read());
        /*for ( ; ; )
        {
            if (Serial1.read() == 10) break;
            else 
            {
                command += char(Serial1.read());
            }
        }
        if (command == "manak") 
        {
            screen_backlight.set_level(255);
            delay(500);
            screen_backlight.set_level(stand_by_brightness);
        }*/
    }    

    if (recvr.decode(&result))
    {
        recvr.resume();
        switch (result.value)
        {
            case POWEROFF_BUTTON:
                power = false;
                eeprom_writer();
                return 47;
            break;
            case OK_BUTTON:
                lcd.begin(16,2);
                __menu_close_timeout = millis();
                Serial.println("[INFO] Main menu loaded.");
                screen_backlight.set_level(normal_brightness);
                menu(&lcd, c_main_menu, main_menu, &recvr, &result, UP_BUTTON, DOWN_BUTTON, OK_BUTTON, EXIT_BUTTON, &__menu_close_timeout, &global_menu_close, main_menu_func, global_loop);
                backlight_timeout_timer = millis();
                relay_timer = millis();
                eeprom_writer();
            break;
            case RED_BUTTON:
                relay_controller(false);             
                power_custom_state = false;
                eeprom_writer();
            break;
            case GREEN_BUTTON:
                relay_controller(true);
                power_custom_state = true;
                eeprom_writer();
            break;

            case ONE_BUTTON: 
                selected_function = 1;
                function_commander(0);
                relay_controller(true);
                selected_function = 255;
            break;

            case TWO_BUTTON: 
                selected_function = 2;
                function_commander(0);
                relay_controller(true);
                selected_function = 255;
            break;

            case THREE_BUTTON: 
                selected_function = 3;
                function_commander(0);
                relay_controller(true);
                selected_function = 255;
            break;

            case FOUR_BUTTON: 
                selected_function = 4;
                function_commander(0);
                relay_controller(true);
                selected_function = 255;
            break;

            case FIVE_BUTTON: 
                selected_function = 5;
                function_commander(0);
                relay_controller(true);
                selected_function = 255;
            break;

            case SIX_BUTTON: 
                selected_function = 6;
                function_commander(0);
                relay_controller(true);
                selected_function = 255;
            break;

            case SEVEN_BUTTON: 
                selected_function = 7;
                function_commander(0);
                relay_controller(true);
                selected_function = 255;
            break;

            case EIGHT_BUTTON: 
                selected_function = 8;
                function_commander(0);
                relay_controller(true);
                selected_function = 255;
            break;

            case NINE_BUTTON: 
                selected_function = 9;
                function_commander(0);
                relay_controller(true);
                selected_function = 255;
            break;

            case ZERO_BUTTON:
                selected_function = 0;
                function_commander(0);  
                relay_controller(true);              
                selected_function = 255;
            break;

            case LOOP_BUTTON:
                if(backlight.get_state()) backlight.set_state(false);
                else backlight.set_state(true);
                relay_timer = millis();
                relay_controller(true);
            break;

            case SCAN_BUTTON:
                if (keyboardLight.get_state()) keyboardLight.set_state(false);
                else keyboardLight.set_state(true);
                relay_timer = millis();
                relay_controller(true);
            break;

            case YELLOW_BUTTON:
                if (!rainbow_state)
                {
                    rainbow_state = true;
                    if (!rainbow_state_top) rainbow_state_top = true;
                    if (!rainbow_state_bottom) rainbow_state_bottom = true;
                }
                else
                {
                    rainbow_state = false;
                    rainbow_state_top = false;
                    rainbow_state_bottom = false;
                }
            break;

        }
    }
}

void main_menu_func(int a)
{
    switch(a)
    {
        case 0:
            menu(&lcd, c_light_menu, light_menu, &recvr, &result, UP_BUTTON, DOWN_BUTTON, OK_BUTTON, EXIT_BUTTON, &__menu_close_timeout, &global_menu_close, light_menu_func, global_loop);
        break;
        case 1:
            selected_function = 255;
            menu(&lcd, c_func_menu, func_menu, &recvr, &result, UP_BUTTON, DOWN_BUTTON, OK_BUTTON, EXIT_BUTTON, &__menu_close_timeout, &global_menu_close, function_menu_func, global_loop);
        case 2:
            cache_choiser_values_string[0] = int_to_on_off(power_custom_state);
            cache_choiser_values_string[1] = String(relay_timer_time);
            cache_choiser_values_string[2] = int_to_on_off(power_auto_turn_off);
            cache_choiser_values_string[3] = String(power_auto_turn_off_hour);
            cache_choiser_values_string[4] = int_to_on_off(power_auto_turn_on);
            cache_choiser_values_string[5] = String(power_auto_turn_on_hour);
            choiser(&lcd, c_power_manager_menu, power_manager_menu, cache_choiser_values_string, &recvr, &result, UP_BUTTON, DOWN_BUTTON, LEFT_BUTTON, RIGHT_BUTTON, OK_BUTTON, EXIT_BUTTON, 0, 5,  &__menu_close_timeout, &global_menu_close, power_menu_function, global_loop); // prototiplenmedi bile, haberin olsun.
        break;
        case 3:
            timeDateSet(&lcd, &recvr, &result, &tm, TIME, UP_BUTTON, DOWN_BUTTON, LEFT_BUTTON, RIGHT_BUTTON, OK_BUTTON, EXIT_BUTTON, &__menu_close_timeout, &global_menu_close, global_loop);
        break;
        case 4:
            timeDateSet(&lcd, &recvr, &result, &tm, DATE, UP_BUTTON, DOWN_BUTTON, LEFT_BUTTON, RIGHT_BUTTON, OK_BUTTON, EXIT_BUTTON, &__menu_close_timeout, &global_menu_close, global_loop);
        break;
        case 255:
            relay_timer = millis();
            backlight_timeout_timer = millis();
        break;
        default:
            Serial.println("[ERR] Unknown select item. (main_menu_err)");
    }
}

void power_menu_function(int optional_value, int choiser_selected_value, int direction)
{
    switch(choiser_selected_value)
    {
        case 0:
            if (direction == UP)
            {
                if (transformator_relay == false) 
                {
                    relay_controller(true);
                    power_custom_state = true;
                    cache_choiser_values_string[choiser_selected_value] = int_to_on_off(transformator_relay);
                }
            }
            else if (direction == DOWN)
            {
                if (transformator_relay == true)
                {
                    relay_controller(false);
                    power_custom_state = false;
                    cache_choiser_values_string[choiser_selected_value] = int_to_on_off(transformator_relay);
                }
            }
        break;

        case 1:
            if (direction == UP || direction == UPDOUBLESPEED)
            {
                
                if (direction == UP) relay_timer_time++;
                else if (direction == UPDOUBLESPEED) relay_timer_time += 10;

                if (relay_timer_time > 120) relay_timer_time = 120;

                cache_choiser_values_string[choiser_selected_value] = String(relay_timer_time);
                eeprom_writer();
            }
            else if (direction == DOWN || direction == DOWNDOUBLESPEED)
            {
                
                if (direction == DOWN) relay_timer_time--;
                else if (direction == DOWNDOUBLESPEED) relay_timer_time -= 10;

                if (relay_timer_time < 0) relay_timer_time = 0;

                cache_choiser_values_string[choiser_selected_value] = String(relay_timer_time);
                eeprom_writer();
            }
            else if (direction == CUSTOM)
            {
                int value = writeAsNumber(&lcd, &recvr, &result, power_manager_menu[choiser_selected_value], 3, ONE_BUTTON, TWO_BUTTON,THREE_BUTTON,FOUR_BUTTON,FIVE_BUTTON, SIX_BUTTON,SEVEN_BUTTON,EIGHT_BUTTON,NINE_BUTTON,ZERO_BUTTON,LEFT_BUTTON,RIGHT_BUTTON,MENU_BUTTON, OK_BUTTON,EXIT_BUTTON,  &__menu_close_timeout, &global_menu_close, global_loop);
                if (value >= 0 && value <= 120)
                {
                    relay_timer_time = value; 
                    cache_choiser_values_string[choiser_selected_value] = String(relay_timer_time);
                }
                eeprom_writer();
            }
        break;

        case 2:
            if (direction == UP)
            {
                if (power_auto_turn_off == false) 
                {
                    power_auto_turn_off = true;
                    cache_choiser_values_string[choiser_selected_value] = int_to_on_off(power_auto_turn_off);
                }
                eeprom_writer();
            }
            else if (direction == DOWN)
            {
                if (power_auto_turn_off == true)
                {
                    power_auto_turn_off = false;
                    cache_choiser_values_string[choiser_selected_value] = int_to_on_off(power_auto_turn_off);
                }
                eeprom_writer();
            }
        break;

        case 3:
            if (direction == UP || direction == UPDOUBLESPEED)
            {
                if (direction == UP) power_auto_turn_off_hour++;
                else if (direction == UPDOUBLESPEED) power_auto_turn_off_hour += 4;

                if (power_auto_turn_off_hour > 10) power_auto_turn_off_hour = 10;

                cache_choiser_values_string[choiser_selected_value] = String(power_auto_turn_off_hour);
                eeprom_writer();
            }
            else if (direction == DOWN || direction == DOWNDOUBLESPEED)
            {
                if (direction == DOWN) power_auto_turn_off_hour--;
                else if (direction == DOWNDOUBLESPEED) power_auto_turn_off_hour -= 4;

                if (power_auto_turn_off_hour < 0) power_auto_turn_off_hour = 0;

                cache_choiser_values_string[choiser_selected_value] = String(power_auto_turn_off_hour);
                eeprom_writer();
            }
            else if (direction == CUSTOM)
            {
                int value = writeAsNumber(&lcd, &recvr, &result, power_manager_menu[choiser_selected_value], 3, ONE_BUTTON, TWO_BUTTON,THREE_BUTTON,FOUR_BUTTON,FIVE_BUTTON, SIX_BUTTON,SEVEN_BUTTON,EIGHT_BUTTON,NINE_BUTTON,ZERO_BUTTON,LEFT_BUTTON,RIGHT_BUTTON,MENU_BUTTON, OK_BUTTON,EXIT_BUTTON, &__menu_close_timeout, &global_menu_close, global_loop);
                if (value >= 0 && value <= 10)
                {
                    power_auto_turn_off_hour = value; 
                    cache_choiser_values_string[choiser_selected_value] = String(power_auto_turn_off_hour);
                }
                eeprom_writer();
            }
        break;

        case 4:
            if (direction == UP)
            {
                if (power_auto_turn_on == false) 
                {
                    power_auto_turn_on = true;
                    cache_choiser_values_string[choiser_selected_value] = int_to_on_off(power_auto_turn_on);
                }
                eeprom_writer();
            }
            else if (direction == DOWN)
            {
                if (power_auto_turn_on == true)
                {
                    power_auto_turn_on = false;
                    cache_choiser_values_string[choiser_selected_value] = int_to_on_off(power_auto_turn_on);
                }
                eeprom_writer();
            }
        break;

        case 5:
            if (direction == UP || direction == UPDOUBLESPEED)
            {
                if (direction == UP) power_auto_turn_on_hour++;
                else if (direction == UPDOUBLESPEED) power_auto_turn_on_hour += 4;

                if (power_auto_turn_on_hour > 23) power_auto_turn_on_hour = 23;

                cache_choiser_values_string[choiser_selected_value] = String(power_auto_turn_on_hour);
                eeprom_writer();
            }
            else if (direction == DOWN || direction == DOWNDOUBLESPEED)
            {
                if (direction == DOWN) power_auto_turn_on_hour--;
                else if (direction == DOWNDOUBLESPEED) power_auto_turn_on_hour -= 4;

                if (power_auto_turn_on_hour < 15) power_auto_turn_on_hour = 15;

                cache_choiser_values_string[choiser_selected_value] = String(power_auto_turn_on_hour);
                eeprom_writer();
            }
            else if (direction == CUSTOM)
            {
                int value = writeAsNumber(&lcd, &recvr, &result, power_manager_menu[choiser_selected_value], 3, ONE_BUTTON, TWO_BUTTON,THREE_BUTTON,FOUR_BUTTON,FIVE_BUTTON, SIX_BUTTON,SEVEN_BUTTON,EIGHT_BUTTON,NINE_BUTTON,ZERO_BUTTON,LEFT_BUTTON,RIGHT_BUTTON,MENU_BUTTON, OK_BUTTON,EXIT_BUTTON, &__menu_close_timeout, &global_menu_close, global_loop);
                if (value >= 15 && value <= 23)
                {
                    power_auto_turn_on_hour = value; 
                    cache_choiser_values_string[choiser_selected_value] = String(power_auto_turn_on_hour);
                }
                eeprom_writer();
            }
        break;

        default: 
            Serial.println("[ERR] Unknown choiser selected value. (err_power_menu_func)");
    }
}

void light_menu_func(int selected_item)
{
    switch (selected_item)
    {
        case 0:
            cache_choiser_values[0] = backlight.get_state(); 
            cache_choiser_values[1] = backlight.red.get_level(); 
            cache_choiser_values[2] = backlight.green.get_level();
            cache_choiser_values[3] = backlight.blue.get_level();
            choiser(&lcd, c_rgb_choiser, rgb_choiser, cache_choiser_values, &recvr, &result, UP_BUTTON, DOWN_BUTTON, LEFT_BUTTON, RIGHT_BUTTON, OK_BUTTON, EXIT_BUTTON, selected_item, 10, &__menu_close_timeout, &global_menu_close, led_choiser_function, global_loop);
            eeprom_writer();
        break;
        case 1:
            cache_choiser_values[0] = keyboardLight.get_state(); 
            cache_choiser_values[1] = keyboardLight.red.get_level(); 
            cache_choiser_values[2] = keyboardLight.green.get_level();
            cache_choiser_values[3] = keyboardLight.blue.get_level();
            choiser(&lcd, c_rgb_choiser, rgb_choiser, cache_choiser_values, &recvr, &result, UP_BUTTON, DOWN_BUTTON, LEFT_BUTTON, RIGHT_BUTTON, OK_BUTTON, EXIT_BUTTON, selected_item, 10, &__menu_close_timeout, &global_menu_close, led_choiser_function, global_loop);
            eeprom_writer();
        break;
        case 2:
            cache_choiser_values[0] = normal_brightness;
            cache_choiser_values[1] = stand_by_brightness;
            cache_choiser_values[2] = normal_to_standby_timeout;
            cache_choiser_values[3] = sunrise_time;
            cache_choiser_values[4] = sunset_time;
            cache_choiser_values[5] = menu_close_delay;
            choiser(&lcd, c_screen_bg_choiser, screen_bg_choiser, cache_choiser_values, &recvr, &result, UP_BUTTON, DOWN_BUTTON, LEFT_BUTTON, RIGHT_BUTTON, OK_BUTTON, EXIT_BUTTON, selected_item, 10, &__menu_close_timeout, &global_menu_close, led_choiser_function, global_loop);
            eeprom_writer();
        break;
        case 3:
            cache_choiser_values_string[0] = int_to_on_off(rainbow_state);
            cache_choiser_values_string[1] = int_to_on_off(rainbow_state_top);
            cache_choiser_values_string[2] = int_to_on_off(rainbow_state_bottom);
            cache_choiser_values_string[3] = String(rainbow_delay_sec);
            cache_choiser_values_string[4] = String(section_max_value);
            choiser(&lcd, c_rainbow_effect, rainbow_effect, cache_choiser_values_string, &recvr, &result, UP_BUTTON, DOWN_BUTTON,LEFT_BUTTON,RIGHT_BUTTON,OK_BUTTON,EXIT_BUTTON, 0, 10,  &__menu_close_timeout, &global_menu_close, rainbow_effect_choiser, global_loop);
        break;
        default:
            Serial.println("[ERR] Unknown select item. (light_menu_err)");
    }
}

void rainbow_effect_choiser(int opt, int selected_value, int direction)
{
    if (direction == UP || direction == UPDOUBLESPEED)
    {
        switch (selected_value)
        {
            case 0:
                if (!rainbow_state)
                {
                    rainbow_state = true;
                    cache_choiser_values_string[selected_value] = int_to_on_off(rainbow_state);
                } 
            break;
            case 1:
                if (!rainbow_state_top)
                {
                    rainbow_state_top = true;
                    cache_choiser_values_string[selected_value] = int_to_on_off(rainbow_state_top);
                }
            break;
            case 2: 
                if (!rainbow_state_bottom)
                {
                    rainbow_state_bottom = true;
                    cache_choiser_values_string[selected_value] = int_to_on_off(rainbow_state_bottom);
                }
            break;
            case 3:
                if (direction == UP) rainbow_delay_sec++;
                else if (direction == UPDOUBLESPEED) rainbow_delay_sec+=10;

                if (rainbow_delay_sec > 10000) rainbow_delay_sec = 10000;
                cache_choiser_values_string[selected_value] = String(rainbow_delay_sec);
            break;
            case 4:
                if (direction == UP) section_max_value++;
                else if (direction == UPDOUBLESPEED) section_max_value+=10;
                if (section_max_value > 255)section_max_value = 255;
                cache_choiser_values_string[selected_value] = String(section_max_value);
            break;
        }
    }
    else if (direction == DOWN || direction == DOWNDOUBLESPEED)
    {
        switch (selected_value)
        {
            case 0:
                if (rainbow_state)
                {
                    rainbow_state = false;
                    cache_choiser_values_string[selected_value] = int_to_on_off(rainbow_state);
                } 
            break;
            case 1:
                if (rainbow_state_top)
                {
                    rainbow_state_top = false;
                    cache_choiser_values_string[selected_value] = int_to_on_off(rainbow_state_top);
                }
            break;
            case 2: 
                if (rainbow_state_bottom)
                {
                    rainbow_state_bottom = false;
                    cache_choiser_values_string[selected_value] = int_to_on_off(rainbow_state_bottom);
                }
            break;
            case 3:
                if (direction == DOWN) rainbow_delay_sec--;
                else if (direction == DOWNDOUBLESPEED) rainbow_delay_sec-=10;

                if (rainbow_delay_sec <0) rainbow_delay_sec = 0;
                cache_choiser_values_string[selected_value] = String(rainbow_delay_sec);
            break;
            case 4:
                if (direction == DOWN) section_max_value--;
                else if (direction == DOWNDOUBLESPEED) section_max_value-=10;

                if (section_max_value < 0) section_max_value = 0;
                cache_choiser_values_string[selected_value] = String(section_max_value);
            break;
        }
    }
}

void led_choiser_function(int led_num, int option, int direction)
{
    if (led_num == 0 || led_num == 1) 
    {                 
        if (option == 0)
        {
            if (direction == UP)
            {
                if (cache_choiser_values[0] == 0)
                {
                    cache_choiser_values[0] = 1;
                    relay_controller(true);
                    if (led_num == 0)
                    {
                        backlight.set_state(true);
                    }
                    else if(led_num == 1)
                    {
                        keyboardLight.set_state(true);
                    }
                    
                }
            }
            else if (direction == DOWN)
            {
                if (cache_choiser_values[0] == 1)
                {
                    cache_choiser_values[0] = 0;
                    if (led_num == 0) backlight.set_state(false);
                    else if (led_num == 1) keyboardLight.set_state(false);
                }
            }
        }
    
        else
        {
            if (direction == UP)
            {
                cache_choiser_values[option]++;
                if (cache_choiser_values[option] > 255)
                {
                    cache_choiser_values[option] = 255;
                }
                if (led_num == 0)
                {
                    backlight.set_level(cache_choiser_values[1],cache_choiser_values[2],cache_choiser_values[3]);
                    if (backlight.get_state()) relay_controller(true);
                }
                else if (led_num == 1)
                {
                    keyboardLight.set_level(cache_choiser_values[1],cache_choiser_values[2],cache_choiser_values[3]);
                    if (keyboardLight.get_state()) relay_controller(true);
                }
            }
            else if (direction == UPDOUBLESPEED)
            {
                cache_choiser_values[option] += 10;
                if (cache_choiser_values[option] > 255)
                {
                    cache_choiser_values[option] = 255;
                }
                if (led_num == 0)
                {
                    backlight.set_level(cache_choiser_values[1],cache_choiser_values[2],cache_choiser_values[3]);
                    if (backlight.get_state()) relay_controller(true);
                }
                else if (led_num == 1)
                {
                    keyboardLight.set_level(cache_choiser_values[1],cache_choiser_values[2],cache_choiser_values[3]);
                    if (keyboardLight.get_state()) relay_controller(true);
                }
            }
            else if (direction == DOWNDOUBLESPEED)
            {
                cache_choiser_values[option] -= 10;
                if (cache_choiser_values[option] < 0)
                {
                    cache_choiser_values[option] = 0;
                }
                if (led_num == 0)
                    backlight.set_level(cache_choiser_values[1],cache_choiser_values[2],cache_choiser_values[3]);
                else if (led_num == 1)
                    keyboardLight.set_level(cache_choiser_values[1],cache_choiser_values[2],cache_choiser_values[3]);  
            } 
            else if (direction == DOWN)
            {
                cache_choiser_values[option]--;
                if (cache_choiser_values[option] < 0)
                {
                    cache_choiser_values[option] = 0;
                }
                if (led_num == 0)
                    backlight.set_level(cache_choiser_values[1],cache_choiser_values[2],cache_choiser_values[3]);
                else if (led_num == 1)
                    keyboardLight.set_level(cache_choiser_values[1],cache_choiser_values[2],cache_choiser_values[3]);
            }
            else if (direction == CUSTOM)
            {
               int returned_value = writeAsNumber(&lcd, &recvr, &result, rgb_choiser[option], 3, ONE_BUTTON, TWO_BUTTON,THREE_BUTTON,FOUR_BUTTON,FIVE_BUTTON, SIX_BUTTON,SEVEN_BUTTON,EIGHT_BUTTON,NINE_BUTTON,ZERO_BUTTON,LEFT_BUTTON,RIGHT_BUTTON,MENU_BUTTON, OK_BUTTON,EXIT_BUTTON, &__menu_close_timeout, &global_menu_close, global_loop);
                if (returned_value >= 0 && returned_value <= 255)
                {
                    cache_choiser_values[option] = returned_value;
                }
                else if(returned_value > 255)
                {
                    returned_value = 255;
                    cache_choiser_values[option] = returned_value;
                }
                if (led_num == 0)
                    backlight.set_level(cache_choiser_values[1],cache_choiser_values[2],cache_choiser_values[3]);
                else if (led_num == 1)
                    keyboardLight.set_level(cache_choiser_values[1],cache_choiser_values[2],cache_choiser_values[3]);
            }
        }
    }
    else if(led_num == 2){ 
        if (direction == 255) screen_backlight.set_level(normal_brightness);
        if (option == 0 || option == 1){ /// screen backlight normal mode brightness set menu
            if (direction == UP) 
            {
                cache_choiser_values[option]++;
                if (cache_choiser_values[option] > 255) cache_choiser_values[option] = 255;
                if (option == 0) normal_brightness = cache_choiser_values[option];
                else if (option == 1) stand_by_brightness = cache_choiser_values[option];
                screen_backlight.set_level(cache_choiser_values[option]);
            }
            else if (direction == UPDOUBLESPEED) 
            {
                cache_choiser_values[option] += 10;
                if (cache_choiser_values[option] > 255) cache_choiser_values[option] = 255;
                if (option == 0) normal_brightness = cache_choiser_values[option];
                else if (option == 1) stand_by_brightness = cache_choiser_values[option];
                screen_backlight.set_level(cache_choiser_values[option]);
            }
            else if (direction == DOWN) 
            {
                cache_choiser_values[option]--;
                if (cache_choiser_values[option] < 5) cache_choiser_values[option] = 5;
                if (option == 0) normal_brightness = cache_choiser_values[option];
                else if (option == 1) stand_by_brightness = cache_choiser_values[option];
                screen_backlight.set_level(cache_choiser_values[option]);
            }
            else if (direction == DOWNDOUBLESPEED)
            {
                cache_choiser_values[option] -= 10;
                if (cache_choiser_values[option] < 5) cache_choiser_values[option] = 5;
                if (option == 0) normal_brightness = cache_choiser_values[option];
                else if (option == 1) stand_by_brightness = cache_choiser_values[option];
                screen_backlight.set_level(cache_choiser_values[option]);
            }
            else if (direction == CUSTOM)
            {
                int custom_value = writeAsNumber(&lcd, &recvr, &result, screen_bg_choiser[option], 3, ONE_BUTTON, TWO_BUTTON,THREE_BUTTON,FOUR_BUTTON,FIVE_BUTTON, SIX_BUTTON,SEVEN_BUTTON,EIGHT_BUTTON,NINE_BUTTON,ZERO_BUTTON,LEFT_BUTTON,RIGHT_BUTTON,MENU_BUTTON, OK_BUTTON,EXIT_BUTTON, &__menu_close_timeout, &global_menu_close, global_loop);
                if (custom_value > 255) custom_value = 255;
                if (option == 0) normal_brightness = cache_choiser_values[option];
                else if (option == 1) stand_by_brightness = cache_choiser_values[option];
            }
        }
       
        else if (option == 2)
        {
            if (direction == UP) 
            {
                cache_choiser_values[option]++;
                if (cache_choiser_values[option] > 120) cache_choiser_values[option] = 120;
                normal_to_standby_timeout = cache_choiser_values[option];
            }
            else if (direction == UPDOUBLESPEED) 
            {
                cache_choiser_values[option] += 10;
                if (cache_choiser_values[option] > 120) cache_choiser_values[option] = 120;
                normal_to_standby_timeout = cache_choiser_values[option];               
            }
            else if (direction == DOWN) 
            {
                cache_choiser_values[option]--;
                if (cache_choiser_values[option] < 1) cache_choiser_values[option] = 1;
                normal_to_standby_timeout = cache_choiser_values[option];                
            }
            else if (direction == DOWNDOUBLESPEED)
            {
                cache_choiser_values[option] -= 10;
                if (cache_choiser_values[option] < 1) cache_choiser_values[option] = 1;
                normal_to_standby_timeout = cache_choiser_values[option];                
            }
            else if (direction == CUSTOM)
            {
                int custom_value = writeAsNumber(&lcd, &recvr, &result, screen_bg_choiser[option], 3, ONE_BUTTON, TWO_BUTTON,THREE_BUTTON,FOUR_BUTTON,FIVE_BUTTON, SIX_BUTTON,SEVEN_BUTTON,EIGHT_BUTTON,NINE_BUTTON,ZERO_BUTTON,LEFT_BUTTON,RIGHT_BUTTON,MENU_BUTTON, OK_BUTTON,EXIT_BUTTON, &__menu_close_timeout, &global_menu_close, global_loop);
                if (custom_value > 120) custom_value = 120;
                normal_to_standby_timeout = cache_choiser_values[option];
            }
        }

        else if (option == 3 || option == 4)
        {
            if (direction == UP || direction == UPDOUBLESPEED) 
            {
               if (direction == UP) cache_choiser_values[option]++;
               else if (direction == UPDOUBLESPEED) cache_choiser_values[option] += 10;

               if (cache_choiser_values[option] > 23) cache_choiser_values[option] = 23;

               if (option == 3)
               {
                    if (cache_choiser_values[option] >= sunset_time) cache_choiser_values[option] = sunset_time - 1;
                    sunrise_time = cache_choiser_values[option];
               }
               else if (option == 4) sunset_time = cache_choiser_values[option];
                
            }
           
            else if (direction == DOWN || direction == DOWNDOUBLESPEED) 
            {
                if (direction == DOWN) cache_choiser_values[option]--;
                else if (direction == DOWNDOUBLESPEED) cache_choiser_values[option] -= 3;

                if (cache_choiser_values[option] < 0) cache_choiser_values[option] = 0;

                if (option == 3) sunrise_time = cache_choiser_values[option];
                else if (option == 4) 
                {
                    if (cache_choiser_values[option] <= sunrise_time) cache_choiser_values[option] = sunrise_time + 1;
                    sunset_time = cache_choiser_values[option];
                }
            }
            
        }
        else if (option == 5)
        {
            if (direction == UP || direction == UPDOUBLESPEED)
            {
                if (direction == UP) cache_choiser_values[option]++;
                else if (direction == UPDOUBLESPEED) cache_choiser_values[option]+=10;

                if (cache_choiser_values[option] > 600) cache_choiser_values[option] = 600;

                menu_close_delay = cache_choiser_values[option];
            }
            else if (direction == DOWN || direction == DOWNDOUBLESPEED)
            {
                if (direction == DOWN) cache_choiser_values[option]--;
                else if (direction == DOWNDOUBLESPEED) cache_choiser_values[option]-=10;
                
                if (cache_choiser_values[option] < 0) cache_choiser_values[option] = 0;

                menu_close_delay = cache_choiser_values[option];
            }
            else if (direction == CUSTOM)
            {
                int a = writeAsNumber(&lcd, &recvr, &result, screen_bg_choiser[option], 3, ONE_BUTTON, TWO_BUTTON,THREE_BUTTON,FOUR_BUTTON,FIVE_BUTTON, SIX_BUTTON,SEVEN_BUTTON,EIGHT_BUTTON,NINE_BUTTON,ZERO_BUTTON,LEFT_BUTTON,RIGHT_BUTTON,MENU_BUTTON, OK_BUTTON,EXIT_BUTTON, &__menu_close_timeout, &global_menu_close, global_loop);
                if (a > 600) a = 600;
                cache_choiser_values[option] = a;
                menu_close_delay = cache_choiser_values[option];
            }
        }
    }
}

void function_menu_func(int a)
{
    selected_function = a;
    Serial.println(selected_function);
    menu(&lcd, c_func_opt, func_opt, &recvr, &result, UP_BUTTON, DOWN_BUTTON, OK_BUTTON, EXIT_BUTTON, &__menu_close_timeout, &global_menu_close, function_commander, global_loop);
    
}

void function_commander(int command) // function options menu
{
        switch (command)
        {
            case 0: // apply for both lights
                if (functions[selected_function][0][0] == 0) backlight.set_state(false);
                else backlight.set_state(true);

                if (backlight.red.get_level() != functions[selected_function][0][1])  backlight.red.set_level(functions[selected_function][0][1]);
                if (backlight.green.get_level() != functions[selected_function][0][2])backlight.green.set_level(functions[selected_function][0][2]);
                if (backlight.blue.get_level() != functions[selected_function][0][3]) backlight.blue.set_level(functions[selected_function][0][3]);
                     
                if (functions[selected_function][1][0] == 0) keyboardLight.set_state(false);
                
                else keyboardLight.set_state(true);

                if (keyboardLight.red.get_level() != functions[selected_function][1][1])  keyboardLight.red.set_level(functions[selected_function][1][1]);
                if (keyboardLight.green.get_level() != functions[selected_function][1][2])keyboardLight.green.set_level(functions[selected_function][1][2]);
                if (keyboardLight.blue.get_level() != functions[selected_function][1][3]) keyboardLight.blue.set_level(functions[selected_function][1][3]);
            break;

            case 1: // apply for top lights
               if (functions[selected_function][0][0] == 0) backlight.set_state(false);
               else backlight.set_state(true);
                
               if (backlight.red.get_level() != functions[selected_function][0][1])  backlight.red.set_level(functions[selected_function][0][1]);
               if (backlight.green.get_level() != functions[selected_function][0][2])backlight.green.set_level(functions[selected_function][0][2]);
               if (backlight.blue.get_level() != functions[selected_function][0][3]) backlight.blue.set_level(functions[selected_function][0][3]);
                
               Serial.print("[INFO] R: ");
               Serial.print(backlight.red.get_level());
               Serial.print("  G: ");
               Serial.print(backlight.green.get_level());
               Serial.print("  B: ");
               Serial.println(backlight.blue.get_level());
            break;

            case 2: // apply for bottom lights
                if (functions[selected_function][1][0] == 0) keyboardLight.set_state(false);
                else keyboardLight.set_state(true);
                
                if (keyboardLight.red.get_level() != functions[selected_function][1][1])  keyboardLight.red.set_level(functions[selected_function][1][1]);
                if (keyboardLight.green.get_level() != functions[selected_function][1][2])keyboardLight.green.set_level(functions[selected_function][1][2]);
                if (keyboardLight.blue.get_level() != functions[selected_function][1][3]) keyboardLight.blue.set_level(functions[selected_function][1][3]);
                
                keyboardLight.set_level(functions[selected_function][1][1], functions[selected_function][1][2],functions[selected_function][1][3]);
                Serial.print("[INFO] R: ");
                Serial.print(keyboardLight.red.get_level());
                Serial.print("  G: ");
                Serial.print(keyboardLight.green.get_level());
                Serial.print("  B: ");
                Serial.println(keyboardLight.blue.get_level());
            break;

            case 3:  // save for top lights
                functions[selected_function][0][0] = backlight.get_state();
                if (backlight.red.get_level() != functions[selected_function][0][1])   functions[selected_function][0][1] = backlight.red.get_level();
                if (backlight.green.get_level() != functions[selected_function][0][2]) functions[selected_function][0][2] = backlight.green.get_level();
                if (backlight.blue.get_level() != functions[selected_function][0][3])  functions[selected_function][0][3] = backlight.blue.get_level();
            break;

            case 4: // save for bottom lights
                functions[selected_function][1][0] = keyboardLight.get_state();
                if (keyboardLight.red.get_level() != functions[selected_function][1][1])   functions[selected_function][1][1] = keyboardLight.red.get_level();
                if (keyboardLight.green.get_level() != functions[selected_function][1][2]) functions[selected_function][1][2] = keyboardLight.green.get_level();
                if (keyboardLight.blue.get_level() != functions[selected_function][1][3])  functions[selected_function][1][3] = keyboardLight.blue.get_level();
            break;

            case 5: // reset
                for (int x = 0; x <= 1; x++)
                    for(int y; y <= 4; y++)
                    {
                        functions[selected_function][x][y] = 0;
                    }
            break;
            
            default:
                Serial.println("[ERR] Unknown select item. (function_options_menu_err)");
        }
}

void rainbow_effect_function(int delay_int)
{
    if(rainbow_state)
    {
        if (!rainbow_ran){
            if (rainbow_state_top && backlight.get_state())
            {
                backlight.set_state(false);
            }
            if (rainbow_state_bottom && keyboardLight.get_state())
            {
                keyboardLight.set_state(false);
            }
            section2 = section_max_value;
            rainbow_ran = true;
            relay_controller(true);
            power_custom_state = true;
            rainbow_time = millis();
            
        }

        if (rainbow_runtime_state == 1)
        {
            if (millis() - rainbow_time > rainbow_delay_sec)
            {
                rainbow_time = millis();
                section1++;
            }
            if (section1 > section_max_value)
            {
                section1 = section_max_value;
                rainbow_runtime_state = 2;
            } 
        }
        else if(rainbow_runtime_state == 2)
        {
            if (millis() - rainbow_time > rainbow_delay_sec)
            {
                rainbow_time = millis();
                section3--;
            }
            if (section3 < 0)
            {
                section3 = 0;
                rainbow_runtime_state = 3;
            } 
        }
        else if(rainbow_runtime_state == 3)
        {
            if (millis() - rainbow_time > rainbow_delay_sec)
            {
                rainbow_time = millis();
                section2++;
            }
            if (section2 > section_max_value)
            {
                section2 = section_max_value;
                rainbow_runtime_state = 4;
            }
            
        }
        else if(rainbow_runtime_state == 4)
        {
            if (millis() - rainbow_time > rainbow_delay_sec)
            {
                rainbow_time = millis();
                section1--;
            }
            if (section1 < 0) 
            {
                section1 = 0;
                rainbow_runtime_state = 5;
            }
            
        }
        else if(rainbow_runtime_state == 5)
        {
            if (millis() - rainbow_time > rainbow_delay_sec)
            {
                rainbow_time = millis();
                section3++;
            }
            if (section3 > section_max_value)
            {
                section3 = section_max_value;
                rainbow_runtime_state = 6;
            }
            
        }
        else if(rainbow_runtime_state == 6)
        {
            if (millis() - rainbow_time > rainbow_delay_sec)
            {
                rainbow_time = millis();
                section2--;
            }
            if (section2 < 0) 
            {
                section2 = 0;
                rainbow_runtime_state = 1;
            }
        }
        
        if (rainbow_state_top)
        {
            analogWrite(backlight.red.get_pin(),section1);
            analogWrite(backlight.blue.get_pin(),section2);
            analogWrite(backlight.green.get_pin(),section3);
        }
        else 
        {
            if (top_old_value) 
            {
                top_old_value = false;
                backlight.set_state(true);
            }
            backlight.red.refresh();
            backlight.green.refresh();
            backlight.blue.refresh();
        }
        if (rainbow_state_bottom)
        {
            analogWrite(keyboardLight.red.get_pin(),section3);
            analogWrite(keyboardLight.blue.get_pin(),section1);
            analogWrite(keyboardLight.green.get_pin(),section2);
        }
        else
        {
            if (bottom_old_value)
            {
                bottom_old_value = false;
                keyboardLight.set_state(true);
            }
            keyboardLight.red.refresh();
            keyboardLight.green.refresh();
            keyboardLight.blue.refresh();
        }
    }
    else 
    {
        if (rainbow_ran)
        {
            section1 = 0;
            section2 = 255;
            section3 = 0;
            rainbow_runtime_state = 1;
            power_custom_state = false;
            rainbow_ran = false;
            if (top_old_value)
            {
                top_old_value = false;
                backlight.set_state(true);
            }
            if (bottom_old_value) 
            {
                bottom_old_value = false;
                keyboardLight.set_state(true);
            }
            backlight.red.refresh();
            backlight.green.refresh();
            backlight.blue.refresh();

            keyboardLight.red.refresh();
            keyboardLight.green.refresh();
            keyboardLight.blue.refresh();
        }
    }
}