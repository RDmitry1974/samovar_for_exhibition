
#ifndef __TCOALS_HPP
#define __TCOALS_HPP

#include "main.h"
#include "hsv2rgb.hpp"


// ----------------------------------------------------------------------------
// ����� ������������ �����
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
    static int cycle_time_ctr;                  // ������������� ������������ ���� �������� � ��
    struct Tled_state
    {
        unsigned char tone;                     // ��� �������� �� � HSV
        unsigned char tone_max;                 // ��� �� �������� ����� ����������� ��
        bool is_light;                          // ������� �� (���������� �� ��� ���) ��
        bool is_up_tone;                        // ��������� �� ��� �������� ��
    };
    static Tled_state led_state[];
};

#endif
