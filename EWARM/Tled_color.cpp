
#include "Tled_color.hpp"
#include "Twater_obj.hpp"
#include <stdlib.h>

unsigned short DMA_buf[LEDS_NUM+2][COLRS][8];

Tled_color led_dim[LEDS_NUM];

const unsigned short ONE_VAL = 26;
const unsigned short NULL_VAL = 13;
//const int Tcommon :: T1 = (Twater_obj::T_MAX - Twater_obj::T_MIN) / (1 + Tled_color::MAX_YELLOW_TONE / (Tled_color::MAX_TONE - Tled_color::MIN_BLUE_TONE));
// ----------------------------------------------------------------------------
volatile unsigned char brite = 1;               //шоб в отладчике можно было яркость крутить

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Tled_color :: drow_led(int num_led)
{
    unsigned int clr;
    RGB_t tmp = hsv2rgb(val);
    clr = tmp.g;
    for(int i = 0; i < 8; ++i)
    {
        if( (clr >> i) & 1)
            DMA_buf[num_led][0][7 - i] = ONE_VAL;
        else
            DMA_buf[num_led][0][7 - i] = NULL_VAL;
    }
    clr = tmp.r;
    for(int i = 0; i < 8; ++i)
    {
        if( (clr >> i) & 1)
            DMA_buf[num_led][1][7 - i] = ONE_VAL;
        else
            DMA_buf[num_led][1][7 - i] = NULL_VAL;
    }
    clr = tmp.b;
    for(int i = 0; i < 8; ++i)
    {
        if( (clr >> i) & 1)
            DMA_buf[num_led][2][7 - i] = ONE_VAL;
        else
            DMA_buf[num_led][2][7 - i] = NULL_VAL;
    }
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void Tled_color :: init_leds()
{
    srand(2332556);
    for(int k = 0; k < (LEDS_NUM); ++k)
    {
        led_dim[k].drow_led(k);
    }
    for(int k = LEDS_NUM; k < (LEDS_NUM + 2); ++k)
        for(int j = 0; j < COLRS; ++j)
            for(int i = 0; i < 8; ++i)
                DMA_buf[k][j][i] = 0;
}

// ----------------------------------------------------------------------------
// Рассчет цвета СД от температуры
// ----------------------------------------------------------------------------
unsigned char Tcommon :: calc_tone(float temp)
{
    unsigned char satur;                                                        //насыщенность СД в HSV
    
    if(temp < Twater_obj::T_MIN)
        satur = Tled_color::MAX_SATURATION;                                     //максимально холодный цвет
    else if(temp > Twater_obj::T_MAX)       
        satur = 1;                                                              //максимально горячий цвет
    else       
        satur = lround(Tled_color::MAX_SATURATION - (temp - Twater_obj::T_MIN) * (Tled_color::MAX_SATURATION - 1) / (Twater_obj::T_MAX - Twater_obj::T_MIN)); 
    return satur;
}
/*unsigned char Tcommon :: calc_tone(float temp)
{
    unsigned char tone;                                                        //тон свечения СД в HSV
    
    if(temp < Twater_obj::T_MIN)
        tone = Tled_color::MIN_BLUE_TONE;                                   //максимально холодный цвет
    else if(temp > Twater_obj::T_MAX)       
        tone = Tled_color::MAX_TONE;                                 //максимально горячий цвет
    else       
        tone = lround(Tled_color::MIN_BLUE_TONE + (temp - Twater_obj::T_MIN) * (Tled_color::MAX_TONE - Tled_color::MIN_BLUE_TONE) / (Twater_obj::T_MAX - Twater_obj::T_MIN)); 
    return tone;
}*/

// ----------------------------------------------------------------------------
void Tstreamlet :: init()
{
    front_pos = 0;
}

// ----------------------------------------------------------------------------
// Рассчитывает насыщенность для эффекта протекания жижи
// ----------------------------------------------------------------------------
unsigned char  Tstreamlet :: calc_saturatin_move_effect(int val)
{
    if(val == 0)
        return 1;
    else if(val < 0)
        return val * (-1);
    else
        return (unsigned char)val;
}
// ----------------------------------------------------------------------------
// Цикл протекания жижи
// ----------------------------------------------------------------------------
void Tstreamlet :: cycle(bool is_flow, float temp)
{
    unsigned char brite_tmp;
    
    cycle_time_ctr -= MAIN_CYCLE_TIME;
    if(cycle_time_ctr > 0)
        return;
    cycle_time_ctr = 150;                              // периодичность обслуживания этой сущности
    general_tone = calc_tone(temp);
    if(is_flow)
        brite_tmp = brite;
    else
        brite_tmp = 0;
    for(int k = 0; k < num_led; ++k)
    {
        led_dim[begin_led + k].val.v = brite_tmp;
        led_dim[begin_led + k].val.h = Tled_color::MIN_BLUE_TONE;//general_tone;
        led_dim[begin_led + k].val.s = general_tone;//15;
    }
    //шоб несколько светодиодиков бегали с шагом в 6 шт.
    for (int tmp_pos = num_led - 1 - front_pos; tmp_pos >= 0; tmp_pos -= 6)
    {
        led_dim[begin_led + tmp_pos].val.s = calc_saturatin_move_effect(general_tone - 9);
        if((tmp_pos + 1) < num_led)
            led_dim[begin_led + tmp_pos + 1].val.s = calc_saturatin_move_effect(general_tone - 7);
        if((tmp_pos + 2) < num_led)
            led_dim[begin_led + tmp_pos + 2].val.s = calc_saturatin_move_effect(general_tone - 5);
 /*     это для случая когда от тепмературы меняется тон СД  
        led_dim[begin_led + tmp_pos].val.s = 6;
        if((tmp_pos + 1) < num_led)
            led_dim[begin_led + tmp_pos + 1].val.s = 8;
        if((tmp_pos + 2) < num_led)
            led_dim[begin_led + tmp_pos + 2].val.s = 10;*/
    }
#ifdef IZVRAT_MIRROR_LEDS
    if(++front_pos >= 6)
        front_pos = 0;
#else
    if(--front_pos < 0)
        front_pos = 6 - 1;
#endif
    for(int k = begin_led; k < (begin_led + num_led); ++k)
        led_dim[k].drow_led(k);
}

// ----------------------------------------------------------------------------
// задание массива пузырьков. Номер начального СД и строка в котором он расположен
// ----------------------------------------------------------------------------
Tbubble Tcontainer :: bubbles_strims[MAX_BUBBLES_STRIMS];// = {Tbubble()};

// ----------------------------------------------------------------------------
// координаты начал движения пузырька
// ----------------------------------------------------------------------------
Tstart_coordinate Tcontainer :: start_coordinate[] = {{0, 0},  {1, 0},  {2, 0},  {3, 0}, {4, 0}, 
                                                      {5, 1},  {6, 1},  {12, 1},  {13, 1}, 
                                                      {14, 2},  {15, 2}, {25, 2}, {26, 2}, 
                                                      {27, 3}, {41, 3}, 
                                                      {57, 5}, {73, 5}};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Tcontainer :: init()
{
  //  front_pos = 0;
}

// ----------------------------------------------------------------------------
// Рассчет числа строк СД от объема
// ----------------------------------------------------------------------------
unsigned char Tcontainer :: calc_line_index(float v)
{
    signed int nl;  
    
    nl = lround(v * (num_led_line) / maxv);
    if(nl > num_led_line)
        nl = num_led_line;  //на одну строчку боле. Ибо нулевой индекс - 0 зажженных СИД
    return (unsigned char)nl;
}

// ----------------------------------------------------------------------------
// Цикл объемов с жидкостью
// ----------------------------------------------------------------------------
void Tcontainer :: cycle(float v, float temp)
{
    unsigned char general_tone;                 // тон свечения СД в HSV
    
    cycle_time_ctr -= MAIN_CYCLE_TIME;
    if(cycle_time_ctr > 0)
        return;
    cycle_time_ctr = 200;//period;                      // периодичность обслуживания этой сущности
    general_tone = calc_tone(temp);                     //рассчет насыщенности СД от температуры
    line_index = calc_line_index(v);                    //рассчет индекса числа СД от объема
    
#ifdef IZVRAT_MIRROR_LEDS
    for(int k = num_led - *(led_tbl + line_index); k < num_led; ++k)
    {   // зажечь нужные СД
        led_dim[begin_led + k].val.v = brite;
        led_dim[begin_led + k].val.h = Tled_color::MIN_BLUE_TONE;//general_tone;
        led_dim[begin_led + k].val.s = general_tone;//15;
    }
    for(int k = 0; k < num_led - *(led_tbl + line_index); ++k)
    {   // потушить НЕ нужные СД
        led_dim[begin_led + k].val.v = 0;
        led_dim[begin_led + k].val.h = 0;
        led_dim[begin_led + k].val.s = 0;
    }
#else
    for(int k = 0; k < *(led_tbl + line_index); ++k)
    {   // зажечь нужные СД
        led_dim[begin_led + k].val.v = brite;
        led_dim[begin_led + k].val.h = general_tone;
        led_dim[begin_led + k].val.s = 15;
    }
    for(int k = *(led_tbl + line_index); k < num_led; ++k)
    {   // потушить НЕ нужные СД
        led_dim[begin_led + k].val.v = 0;
        led_dim[begin_led + k].val.h = 0;
        led_dim[begin_led + k].val.s = 0;
    }
#endif
    if(is_bubbles && (line_index > 1))
    {
        Tbubble :: generat_bubbles(temp, q_samovar_heating);
        for(int i = 0; i < MAX_BUBBLES_STRIMS; ++i)
            bubbles_strims[i].drow_bubble(line_index, &(led_dim[begin_led]), num_led);
    }
    for(int k = begin_led; k < (begin_led + num_led); ++k)
        led_dim[k].drow_led(k);
}
