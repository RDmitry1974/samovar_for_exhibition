
#ifndef __TWATER_OBJ_HPP
#define __TWATER_OBJ_HPP

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif


class Twater_obj
{
public:
    static int const T_MIN;             //��������� ����������� ��������. ��� �� ��� ����������� ��������
    static int const T_MAX;             //������������ ����������� ��������.
    static float const DELTA_TEMP;      //�������� ��������� ��������( �������� �� ����� �������).
    float temp;                         //����������� ��������
    float v;                            //����� ��������
    
    Twater_obj(float k_lost, float v_max, float v_in, float v_out)
    {
        this->k_lost = k_lost;
        this->v_max = v_max;
        this->v_in  = v_in;
        this->v_out = v_out;
        temp = T_MIN;
        v = 0;
    }
    
    //-----------------------------------------------------------------------------
    // ��������� �-��. ������ ���������� ��� � ����� �������
    // is_inp       ��������� �� �������� � �����?
    // is_out       ���������� �� �������� �� ������?
    // q_heating    ������� ���������
    // t_in         ����������� ��������� ��������   
    // http://www.math24.ru/%D0%B7%D0%B0%D0%BA%D0%BE%D0%BD-%D0%BE%D1%85%D0%BB%D0%B0%D0%B6%D0%B4%D0%B5%D0%BD%D0%B8%D1%8F-%D0%BD%D1%8C%D1%8E%D1%82%D0%BE%D0%BD%D0%B0.html
    // https://mathforyou.net/online/calculus/ode/
    //-----------------------------------------------------------------------------
    void cycle(bool is_inp, bool is_out, float q_heating, float t_in)    //�
    {
        if(is_out)
        {
            v -= v_out;
            if(v < 0)
                v = 0;
        }
        if(is_inp)
        {
            temp = (temp * v + t_in * v_in) / (v + v_in);       //����������� ����� �������
            v += v_in;                                          //����� ����� �������
            if(v > v_max)
                v = v_max;
        }
        if(v < 0.0001)
        {
            temp -= DELTA_TEMP; // �������, ���� ��� ����������� ��������, ��� ����������� �� ������������ ����� � T_MIN
            if(temp < T_MIN)
                temp = T_MIN;           
        }
        else
        { 
            float z = q_heating / (C_WATER * v) + T_MIN;
            temp = z + (temp - T_MIN) * exp(k_lost / (C_WATER * v));       
        }
        if(temp > T_MAX)
            temp = T_MAX;           //���� �� ������� ���� �� �������������
 //       if(temp > T_MAX + 5)
 //           temp = T_MAX + 5;           //���� �� ������� ���� �� �������������
    }
    
private:
    static float const C_WATER;         //�������� ������������ ��������
    float v_max;                        //������������ ����� ��������
    float k_lost;                       //�������� ����������� ������  
    float v_in;                         //����� �� ����� ������� ����������� ��������
    float v_out;                        //����� �� ����� ������� ��������� ��������
};
    
#ifdef __cplusplus
}
#endif

#endif
