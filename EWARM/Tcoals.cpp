
#include "Tcoals.hpp"
#include "Tled_color.hpp"
#include <stdlib.h>

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
int Tcoals :: cycle_time_ctr = 40;
Tcoals::Tled_state Tcoals :: led_state[NUM_LED];

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Tcoals :: init()
{
    for(int k = 0; k < NUM_LED; ++k)
    {
        led_dim[BEGIN_LED + k].val.h = 0;
        led_dim[BEGIN_LED + k].val.s = 15;
        led_dim[BEGIN_LED + k].val.v = 1;
        led_state[k].is_light = false;
        led_state[k].tone_max = 11;
        led_state[k].is_up_tone = true;
    }
}
// ----------------------------------------------------------------------------
// Цикл мерцания углей
// ----------------------------------------------------------------------------
void Tcoals :: cycle(int adc_data)
{
    int tone1;
    cycle_time_ctr -= MAIN_CYCLE_TIME;
    if(cycle_time_ctr > 0)
        return;
    cycle_time_ctr = 40;                        // периодичность обслуживания этой сущности
    int brite = (adc_data + 140 - ADC_4mA_VALUE) * Tled_color::MAX_BRIHTNES / (ADC_20mA_VALUE - ADC_4mA_VALUE);
    if (brite > Tled_color::MAX_BRIHTNES)
        brite = Tled_color::MAX_BRIHTNES;
    else if (brite < 0)
        brite = 0;
    for(int k = 0; k < NUM_LED; ++k)
    {
        led_dim[BEGIN_LED + k].val.v = brite;
        if(!led_state[k].is_light)
        {
            tone1 = 1 + rand() % 15;
            if(tone1 > 10)
            {
                led_state[k].tone = tone1 - 11;
                led_state[k].is_light = true;
                led_state[k].is_up_tone = true;
            }
        }
        led_dim[BEGIN_LED + k].val.h = led_state[k].tone;
        if(led_state[k].is_light)
        {
            if(led_state[k].is_up_tone)
            {   //наращиваем тон
                if(led_state[k].tone >= led_state[k].tone_max)
                    led_state[k].is_up_tone = false;    //хорош! Пошли теперь вниз
                else
                    led_dim[BEGIN_LED + k].val.h = ++led_state[k].tone;
            }
            else
            {   //тушим тон
                if(led_state[k].tone > 0)
                    led_dim[BEGIN_LED + k].val.h = --led_state[k].tone;
                else
                    led_state[k].is_light = false;    //
            }
        }
    }
    for(int k = BEGIN_LED; k < (BEGIN_LED + NUM_LED); ++k)
        led_dim[k].drow_led(k);
}

