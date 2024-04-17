
#ifndef __TLED_COLOR_HPP
#define __TLED_COLOR_HPP

#include "main.h"
#include "hsv2rgb.hpp"
#include "Tbubble.hpp"

extern unsigned short DMA_buf[][COLRS][8];
extern volatile unsigned char brite; 

// ----------------------------------------------------------------------------
// Перевод цвета пикселя из HSV в RGB и заполнение видео ОЗУ 
// ----------------------------------------------------------------------------
class Tled_color
{
public:
    static const int MAX_TONE = 96;
    static const int MAX_SATURATION = 15;
    static const int MAX_BRIHTNES = 17;
    static const int MIN_BLUE_TONE = 65;
    Tled_color()
    {
        val.h = 0;  //тон 0 - 96
        val.s = 0;  //насыщенность 0 - 15. С отсутствием тона, значения S = 0 дадут не серый цвет, а белый разной яркости
        val.v = 0;  //яркость 0 - 17, при V = 0 пиксель не светится, при Vmax = 17 — светится максимально ярко, в зависимости от H и S.
    }
    HSV_t val;
    
    void drow_led(int num_led); 
    static void init_leds();
};

// ----------------------------------------------------------------------------
// Маленький классик для общих нужд
// ----------------------------------------------------------------------------
class Tcommon
{
private:
//    static float const T1;
    
public:
    unsigned char calc_tone(float temp);     // Рассчет цвета СД от температуры
};

// ----------------------------------------------------------------------------
// Класс обслуживания потоков жидкости (втекающий, вытекающий)
// ----------------------------------------------------------------------------
class Tstreamlet : public Tcommon
{
public:
    Tstreamlet(int begin_led, int num_led)
    {
        this->begin_led = begin_led;
        this->num_led = num_led;
    }
    void init();
    void cycle(bool is_flow, float temp);
private:
    int begin_led;                              // физический номер первого СД в объекте
    int num_led;                                // число СД в объекте
    int cycle_time_ctr;                         // переодичность обслуживания этой сущности в мс
    int front_pos;                              // позиция притененного СД
    unsigned char general_tone;                 // тон свечения СД в HSV
    unsigned char calc_saturatin_move_effect(int val); // Рассчитывает насыщенность для эффекта протекания жижи
};

// ----------------------------------------------------------------------------
// Класс обслуживания объемов с жидкостью (самовар, чашка)
// ----------------------------------------------------------------------------
class Tcontainer : public Tcommon
{
public:
    Tcontainer(const int begin_led, const int num_led, const unsigned int num_led_line, const float maxv, const unsigned short* led_tbl, const bool is_bubbles)
    {
        this->begin_led = begin_led;
        this->num_led = num_led;
        this->num_led_line = num_led_line;
        this->maxv = maxv;
        this->led_tbl = led_tbl;
        this->is_bubbles = is_bubbles;
        line_index = 0;
    }
    static Tbubble bubbles_strims[MAX_BUBBLES_STRIMS]; //потоки пузырьков
    static Tstart_coordinate start_coordinate[]; // координаты начал движения пузырька
    int line_index;                             // индекс числа СД от объема. (Число зажженных строк)
    void init();
    void cycle(float v, float temp);
private:
    int begin_led;                              // физический номер первого СД в объекте
    int num_led;                                // число СД в объекте
    int cycle_time_ctr;                         // переодичность обслуживания этой сущности в мс
    unsigned int num_led_line;                  // число линий(строк) СД в объекте
    bool is_bubbles;                            // есть ли в объекте пузырьки? :)
    float maxv;                                 // максимальный объем емкости
    const unsigned short* led_tbl;              // указатель на таблицу "Количество СД от номера строки"
    unsigned char calc_line_index(float v);     // Рассчет индекса числа СД от объема
};

extern Tled_color led_dim[LEDS_NUM];

#endif
