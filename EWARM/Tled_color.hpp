
#ifndef __TLED_COLOR_HPP
#define __TLED_COLOR_HPP

#include "main.h"
#include "hsv2rgb.hpp"
#include "Tbubble.hpp"

extern unsigned short DMA_buf[][COLRS][8];
extern volatile unsigned char brite; 

// ----------------------------------------------------------------------------
// ������� ����� ������� �� HSV � RGB � ���������� ����� ��� 
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
        val.h = 0;  //��� 0 - 96
        val.s = 0;  //������������ 0 - 15. � ����������� ����, �������� S = 0 ����� �� ����� ����, � ����� ������ �������
        val.v = 0;  //������� 0 - 17, ��� V = 0 ������� �� ��������, ��� Vmax = 17 � �������� ����������� ����, � ����������� �� H � S.
    }
    HSV_t val;
    
    void drow_led(int num_led); 
    static void init_leds();
};

// ----------------------------------------------------------------------------
// ��������� ������� ��� ����� ����
// ----------------------------------------------------------------------------
class Tcommon
{
private:
//    static float const T1;
    
public:
    unsigned char calc_tone(float temp);     // ������� ����� �� �� �����������
};

// ----------------------------------------------------------------------------
// ����� ������������ ������� �������� (���������, ����������)
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
    int begin_led;                              // ���������� ����� ������� �� � �������
    int num_led;                                // ����� �� � �������
    int cycle_time_ctr;                         // ������������� ������������ ���� �������� � ��
    int front_pos;                              // ������� ������������ ��
    unsigned char general_tone;                 // ��� �������� �� � HSV
    unsigned char calc_saturatin_move_effect(int val); // ������������ ������������ ��� ������� ���������� ����
};

// ----------------------------------------------------------------------------
// ����� ������������ ������� � ��������� (�������, �����)
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
    static Tbubble bubbles_strims[MAX_BUBBLES_STRIMS]; //������ ���������
    static Tstart_coordinate start_coordinate[]; // ���������� ����� �������� ��������
    int line_index;                             // ������ ����� �� �� ������. (����� ��������� �����)
    void init();
    void cycle(float v, float temp);
private:
    int begin_led;                              // ���������� ����� ������� �� � �������
    int num_led;                                // ����� �� � �������
    int cycle_time_ctr;                         // ������������� ������������ ���� �������� � ��
    unsigned int num_led_line;                  // ����� �����(�����) �� � �������
    bool is_bubbles;                            // ���� �� � ������� ��������? :)
    float maxv;                                 // ������������ ����� �������
    const unsigned short* led_tbl;              // ��������� �� ������� "���������� �� �� ������ ������"
    unsigned char calc_line_index(float v);     // ������� ������� ����� �� �� ������
};

extern Tled_color led_dim[LEDS_NUM];

#endif
