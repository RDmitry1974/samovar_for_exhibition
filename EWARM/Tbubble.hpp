
#ifndef __TBUBBLE_HPP
#define __TBUBBLE_HPP

#include "main.h"

class Tled_color;

struct Tstart_coordinate
{
    int begin_led;                              // физический номер первого СД в объекте (с него пузырек начнет свое движение вверх)
    int start_line;                             // номер строки контейнера первого СД
};

// ----------------------------------------------------------------------------
// Пузырьки
// ----------------------------------------------------------------------------
class Tbubble
{
public:
    Tbubble()
//    Tbubble(int begin_led, int start_line)
    {
 //       this->begin_led = begin_led;
 //       this->start_line = start_line;
        is_enable = false;
        style_depended_delta = 0;
    }
//    static const unsigned int MAX_BUBBLES_STRIMS = 15;                        // максимальное число потоков пузырьков
    
    static const unsigned short ADD_TBL[];      // таблица смещения для перехода на следующую строку для формирования верт линии
    static void generat_bubbles(float temp, float q_heating);                   // сгенерировать потоки пузырьков   
    void drow_bubble(unsigned int max_line_container_light, Tled_color* leds, unsigned int num_led);  // отрисовать пузырёк
    void start_strim(int style_depended_delta, int start_coord);                // начать отрисовку потока пузырька
private:
    static int cycle_time;                      // время переключения пузырька в мс
    static int active_strims_num;               // число активных потоков пузырька от 0 до MAX_BUBBLES_STRIMS
    static int active_strims_calc(float temp, float q_heating);                        // Рассчитать число активных потоков пузырьков
//    int begin_led;                              // физический номер первого СД в объекте (с него пузырек начнет свое движение вверх)
//    int start_line;                             // номер строки контейнера первого СД
    int current_led;                            // физический номер зажженого СД
    int cycle_time_ctr;                         // счетчик времени переключения пузырька в мс
    bool is_enable;                             // работает (отрисовывается) этот поток пузырька
    int style_depended_delta;                   // направление закручивания потока пузырька (-1, 0 или +1)
    unsigned int num_line;                      // номер текущей строки
    unsigned int calc_line_led(unsigned int num_led);   // рассчитать на какой строке горит светодиод
    void calc_new_pixel_bubble(unsigned int max_line_container_light);          // рассчитать новый пиксель пузырька
};

#endif
