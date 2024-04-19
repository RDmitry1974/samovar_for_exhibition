
#ifndef __TBUBBLE_HPP
#define __TBUBBLE_HPP

#include "main.h"

class Tled_color;

struct Tstart_coordinate
{
    int begin_led;                              // ���������� ����� ������� �� � ������� (� ���� ������� ������ ���� �������� �����)
    int start_line;                             // ����� ������ ���������� ������� ��
};

// ----------------------------------------------------------------------------
// ��������
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
//    static const unsigned int MAX_BUBBLES_STRIMS = 15;                        // ������������ ����� ������� ���������
    
    static const unsigned short ADD_TBL[];      // ������� �������� ��� �������� �� ��������� ������ ��� ������������ ���� �����
    static void generat_bubbles(float temp, float q_heating);                   // ������������� ������ ���������   
    void drow_bubble(unsigned int max_line_container_light, Tled_color* leds, unsigned int num_led);  // ���������� ������
    void start_strim(int style_depended_delta, int start_coord);                // ������ ��������� ������ ��������
private:
    static int cycle_time;                      // ����� ������������ �������� � ��
    static int active_strims_num;               // ����� �������� ������� �������� �� 0 �� MAX_BUBBLES_STRIMS
    static int active_strims_calc(float temp, float q_heating);                        // ���������� ����� �������� ������� ���������
//    int begin_led;                              // ���������� ����� ������� �� � ������� (� ���� ������� ������ ���� �������� �����)
//    int start_line;                             // ����� ������ ���������� ������� ��
    int current_led;                            // ���������� ����� ��������� ��
    int cycle_time_ctr;                         // ������� ������� ������������ �������� � ��
    bool is_enable;                             // �������� (��������������) ���� ����� ��������
    int style_depended_delta;                   // ����������� ������������ ������ �������� (-1, 0 ��� +1)
    unsigned int num_line;                      // ����� ������� ������
    unsigned int calc_line_led(unsigned int num_led);   // ���������� �� ����� ������ ����� ���������
    void calc_new_pixel_bubble(unsigned int max_line_container_light);          // ���������� ����� ������� ��������
};

#endif
