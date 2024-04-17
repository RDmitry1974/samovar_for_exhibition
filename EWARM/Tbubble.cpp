
#include "Twater_obj.hpp"
#include "Tbubble.hpp"
#include "Tled_color.hpp"
#include <stdlib.h>

// ----------------------------------------------------------------------------
const unsigned short Tbubble :: ADD_TBL[] = {7, 11, 14, 15, 16, 17, 18, 19, 19, 19, 18, 17, 16, 15, 14, 11};  
// ----------------------------------------------------------------------------
int Tbubble :: cycle_time;
int Tbubble :: active_strims_num = 0;
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// ������ ��������� ������ ��������
// time - ����� � ������ ��������
// ----------------------------------------------------------------------------
void Tbubble :: start_strim(int style_depended_delta, int start_coord)
{
    current_led = Tcontainer :: start_coordinate[start_coord].begin_led;
    num_line = Tcontainer :: start_coordinate[start_coord].start_line;
    is_enable = true;
 //       this->begin_led = begin_led;
 //       this->start_line = start_line;
//    Tbubble :: cycle_time = time;
    this->style_depended_delta = style_depended_delta;
    active_strims_num++;
}

// ----------------------------------------------------------------------------
// ���������� ����� �������� ������� ���������
// temp         ����������� ����
// q_heating    ���������� ������� ���������� � ���
// ----------------------------------------------------------------------------
int Tbubble :: active_strims_calc(float temp, float q_heating)
{
    if(temp < MIN_TEMPER_BUBBLES_PROCESS)
        return 0;         //������������� ��� ����������� ����� �����
    float tmp = temp * MAX_BUBBLES_STRIMS * TEMPER_BUBBLES_PART / Twater_obj :: T_MAX;              //������������ �� ����������� ���� � ����������
    float tmp1 = q_heating * MAX_BUBBLES_STRIMS * (1 - TEMPER_BUBBLES_PART) / SAMOVAR_MAX_HEATING;  //������������ �� ���������� ������� ����������� � ���
    int rez = (int)(tmp + tmp1);
    if(rez > MAX_BUBBLES_STRIMS)
        return MAX_BUBBLES_STRIMS;
    else
        return rez;
}

// ----------------------------------------------------------------------------
// ���������� �� ����� ����� � �������
// ----------------------------------------------------------------------------
/*bool is_contein_val(int* ind_ptr, int val, int ctr)
{
    int ind[MAX_BUBBLES_STRIMS];
    ind = ind_ptr;
    for(int i = 0; i < ctr; ++i)
        if(ind[i] == val)
            return true;
    return false;
}
// ----------------------------------------------------------------------------
int ind[MAX_BUBBLES_STRIMS];
*/
// ----------------------------------------------------------------------------
// ������������� ������ ���������
// ----------------------------------------------------------------------------
void Tbubble :: generat_bubbles(float temp, float q_heating)
{
    int active_strims_tmp = active_strims_calc(temp, q_heating);
    if(active_strims_num >= active_strims_tmp)
        return;         //����� ����� ���� �� ����
/*    for(int i = 0; i < active_strims_tmp; ++i)
    {
        int tt = 1 + rand() % active_strims_tmp;
        while(is_contein_val(ind, tt, i))
             tt = 1 + rand() % active_strims_tmp;
        ind[i] = tt;
    }*/
    int start_str_num;
    for(int i = 0; i < active_strims_tmp; ++i)
    {
        if(!Tcontainer :: bubbles_strims[i].is_enable)
        {
            start_str_num = 1 + rand() % MAX_BUBBLES_STRIMS;
            Tcontainer :: bubbles_strims[i].start_strim((rand() % 3) - 1, start_str_num);
   //         Tcontainer :: bubbles_strims[i].start_strim(0, start_str_num);
        }
    }
}
// ----------------------------------------------------------------------------
// ���������� �� ����� ������ ����� ���������
// 0 - ������ ������
// ----------------------------------------------------------------------------
unsigned int Tbubble :: calc_line_led(unsigned int num_led)
{
    unsigned int i;
    for(i = 1; i <= SAMOVAR_LED_LINE; ++i)
    {
        if(num_led < SAMOVAR_LED_VS_NUMBER_LINE_TBL[i])
            return i - 1;
    }
    return i - 1; 
}

// ----------------------------------------------------------------------------
// ���������� ����� ������� ��������
// max_line_container_light - ����� ��������� ���������� ����� � ����������
// ----------------------------------------------------------------------------
void Tbubble :: calc_new_pixel_bubble(unsigned int max_line_container_light)
{
    int tmp;
    unsigned int num_line_tmp;
    
    if(num_line >= (SAMOVAR_LED_LINE - 1))
    {   // ����� ���� ������. ����� ������ �����.
        active_strims_num--;
        is_enable = false;
        return;
    }
    if(num_line > (max_line_container_light - 1))
    {   // ��� ������������ ���� ����� ������ ������ ���� ���� ���������.
        active_strims_num--;
        is_enable = false;
        return;
    }
    tmp = current_led + ADD_TBL[num_line];
    num_line_tmp = calc_line_led(tmp);
    if(num_line_tmp == num_line)
    {   // ������ ����� �� ��� ���� �����������. ����� ������ �����.
        active_strims_num--;
        is_enable = false;
        return;
    }
    if((num_line_tmp - num_line) > 1)
    {   // ������ ����� �� ���� ���� �����������. ����� ������ �����.
        active_strims_num--;
        is_enable = false;
        return;
    }
    style_depended_delta *= -1;
    num_line = num_line_tmp;
    if( num_line_tmp == calc_line_led(tmp + style_depended_delta))
    {
       current_led = tmp + style_depended_delta;
    }
    else
    {
       current_led = tmp;
    }
}
// ----------------------------------------------------------------------------
// ���������� ������
// �������� �� ����� ��������. ��� ��������� ��������
// num_led - ����� ����������� � �������
// ----------------------------------------------------------------------------
void Tbubble :: drow_bubble(unsigned int max_line_container_light, Tled_color* leds, unsigned int num_led)
{
    if(!is_enable)
        return;
    --cycle_time_ctr;
    if(cycle_time_ctr > 0)
        return;
    cycle_time_ctr = Tbubble :: cycle_time;                      // ������������� ������������ ���� ��������
    int led_prep;
    if(num_line & 1)
    {   //���� �� ������ ������, ����������� ��
        led_prep = SAMOVAR_LED_VS_NUMBER_LINE_TBL[num_line] + SAMOVAR_LED_PER_LINE_TBL[num_line] + 1 - current_led + SAMOVAR_LED_VS_NUMBER_LINE_TBL[num_line];
    }
    else
        led_prep = current_led;
  //  ����� ���������� ������� � ���� ��������
#ifdef IZVRAT_MIRROR_LEDS
   // (leds + num_led - current_led)->val.v = brite + 1;
    (leds + num_led - led_prep)->val.v = brite + 1;
#else
    (leds + led_prep)->val.v = brite + 1;
#endif
    calc_new_pixel_bubble(max_line_container_light);
}

