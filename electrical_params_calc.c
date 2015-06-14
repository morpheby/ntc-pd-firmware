
#include <p33FJ32MC204.h>
#include <math.h>

float AD3_s,AD2_s,AD1_s,AD0_s,AD4_s,AD5_s,AD6_s=0;
float P_sum1,P_sum2,P_sum3=0;
float AD4_s_d,AD5_s_d,AD6_s_d=0; //суммма средних,действующих

extern int N;
extern float AD4_AVG, AD5_AVG, AD6_AVG;
extern float AD4_RMS, AD5_RMS, AD6_RMS;
extern float P1, P2, P3;
extern float Q1, Q2, Q3;
extern float S1, S2, S3;
extern float cos_f1, cos_f2, cos_f3;

void Data_Calc(float _ADC4,float _ADC5, float _ADC6)
{

   AD4_s = AD4_s + _ADC4;                //вычисляем сумму(для определения ср. знач. величин)
   AD5_s = AD5_s + _ADC5;
   AD6_s = AD6_s + _ADC6;

   //P_sum1 = P_sum1 + (ADC4 * ADC5);       //сумма мгновенных значений мощности
   P_sum2 = P_sum2 + (_ADC4 * _ADC6);
   P_sum3 = P_sum3 + (_ADC5 * _ADC6);

   AD4_s_d = AD4_s_d + (_ADC4 * _ADC4);   //вычисляем сумму квадратов(для опред. действ. знач. величин)
   AD5_s_d = AD5_s_d + (_ADC5 * _ADC5);
   AD6_s_d = AD6_s_d + (_ADC6 * _ADC6);

}


void Data_Calc_inc(char U_pin)
{
    AD4_AVG = AD4_s / N;             //среднее значение измеряемых величин
    AD5_AVG = AD5_s / N;
    AD6_AVG = AD6_s / N;

    AD4_s = 0;
    AD5_s = 0;
    AD6_s = 0;

    P1 = P_sum1 / N;           //среднее значение активной мощности
    P2 = P_sum2 / N;
    P3 = P_sum3 / N;

    P_sum1 = 0;
    P_sum2 = 0;
    P_sum3 = 0;

    AD4_RMS = sqrt(AD4_s_d / N);   //действ. значения величин
    AD5_RMS = sqrt(AD5_s_d / N);   //sqrt(AD5_s_d / N);
    AD6_RMS = sqrt(AD6_s_d / N);

    AD4_s_d = 0;
    AD5_s_d = 0;
    AD6_s_d = 0;

    if (U_pin == 5)
    {
        S3 = AD5_RMS * AD6_RMS;
        if   (P3 >= S3)
        {
            Q3 = 0;
            cos_f3 = 1;
        }
        else
        {
            Q3 = sqrt(S3 * S3 - P3 * P3); //значения реактивной мощности ?????
            cos_f3 = P3 / S3;
        }
    }

    if (U_pin == 4)
    {
        S2 = AD4_RMS * AD6_RMS;
        if   (P2 >= S2)
        {
            Q2 = 0;
            cos_f2 = 1;
        }
        else
        {
            Q2 = sqrt(S2*S2 - P2*P2);
            cos_f2 = P2 / S2;
        }
    }

}
