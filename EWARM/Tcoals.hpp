
#ifndef __TCOALS_HPP
#define __TCOALS_HPP

#include "main.h"
#include "hsv2rgb.hpp"


// ----------------------------------------------------------------------------
// Класс обслуживания углей
// ----------------------------------------------------------------------------
class Tcoals
{
public:
    static void init();
    static void cycle(int adc_data);
private:
#ifdef IZVRAT_MIRROR_LEDS
    static const int BEGIN_LED = 285;
#else
    static const int BEGIN_LED = 6;
#endif
    static const int NUM_LED = 7;
    static int cycle_time_ctr;                  // переодичность обслуживания этой сущности в мс
    struct Tled_state
    {
        unsigned char tone;                     // тон свечения СД в HSV
        unsigned char tone_max;                 // тон до которого будет разгораться СД
        bool is_light;                          // мерцает ли (изменяется ли его тон) СД
        bool is_up_tone;                        // нарастает ли тон свечения СД
    };
    static Tled_state led_state[];
};

#endif
