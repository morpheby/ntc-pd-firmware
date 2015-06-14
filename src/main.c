/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "oscillators.h"
#include "uart_base.h"
#include "messaging.h"
#include "flash_store.h"
#include "wdt.h"
#include "cn_inputs.h"
#include "mem_pool.h"

#include "modbus.h"
#include "ADC.h"
#include "electrical_params_calc.h"
#include "Timers_Init.h"
#include "math.h"
#include "D_I_O.h"
#include "izmer_V.h"


/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

#define RAM_START_ADDRESS               0x900
#define FLASH_START                     0x0F


 /*
#define KEY_1                           PORTCbits.RC1  //UP
#define KEY_2                           PORTCbits.RC2  //DOWN
#define KEY_3                           PORTAbits.RA2  //MENU
#define KEY_4                           PORTAbits.RA3  //BACK
*/
#define N1                              2
#define N2                              2
#define M                               43


// RAM Data
int BRG_VAL __attribute__ ((address(RAM_START_ADDRESS+0)));//REG 0

float ADC0 __attribute__  ((address(RAM_START_ADDRESS+2)));//REG 1, REG 2
float ADC1 __attribute__  ((address(RAM_START_ADDRESS+6)));//REG 3, REG 4
float ADC2 __attribute__  ((address(RAM_START_ADDRESS+10)));//REG 5, REG 6
float ADC3 __attribute__  ((address(RAM_START_ADDRESS+14)));//REG 7, REG 8
float ADC4 __attribute__  ((address(RAM_START_ADDRESS+18)));//REG 9, REG 10
float ADC5 __attribute__  ((address(RAM_START_ADDRESS+22)));//REG 11, REG 12
float ADC6 __attribute__  ((address(RAM_START_ADDRESS+26)));//REG 13, REG 14

float K0 __attribute__ ((address(RAM_START_ADDRESS+30)));//REG 15, REG 16
float K1 __attribute__ ((address(RAM_START_ADDRESS+34)));//REG 17, REG 18
float K2 __attribute__ ((address(RAM_START_ADDRESS+38)));//REG 19, REG 20
float K3 __attribute__ ((address(RAM_START_ADDRESS+42)));//REG 21, REG 22
float K4 __attribute__ ((address(RAM_START_ADDRESS+46)));//REG 23, REG 24
float K5 __attribute__ ((address(RAM_START_ADDRESS+50)));//REG 25, REG 26
float K6 __attribute__ ((address(RAM_START_ADDRESS+54)));//REG 27, REG 28

int OFS_ADC0 __attribute__        ((address(RAM_START_ADDRESS+58)));//REG 29
int OFS_ADC1 __attribute__        ((address(RAM_START_ADDRESS+60)));//REG 30
int OFS_ADC2 __attribute__        ((address(RAM_START_ADDRESS+62)));//REG 31
int OFS_ADC3 __attribute__        ((address(RAM_START_ADDRESS+64)));//REG 32
int OFS_ADC4 __attribute__        ((address(RAM_START_ADDRESS+66)));//REG 33
int OFS_ADC5 __attribute__        ((address(RAM_START_ADDRESS+68)));//REG 34
int OFS_ADC6 __attribute__        ((address(RAM_START_ADDRESS+70)));//REG 35

int n_a  __attribute__ ((address(RAM_START_ADDRESS+72)));           //REG 36
int n_b  __attribute__ ((address(RAM_START_ADDRESS+74)));           //REG 37
int n_c  __attribute__ ((address(RAM_START_ADDRESS+76)));           //REG 38
int ind_off __attribute__ ((address(RAM_START_ADDRESS+78)));        //REG 39

int N __attribute__ ((address(RAM_START_ADDRESS+80)));              //REG 40

int D_In  __attribute__ ((address(RAM_START_ADDRESS+82)));          //REG 41
int D_Out __attribute__ ((address(RAM_START_ADDRESS+84)));          //REG 42
int Ind_Delay __attribute__ ((address(RAM_START_ADDRESS+86)));      //REG 43

int profile __attribute__ ((address(RAM_START_ADDRESS+88)));        //REG 44
int ind_0_1 __attribute__ ((address(RAM_START_ADDRESS+90)));        //REG 45
int ind_1_1 __attribute__ ((address(RAM_START_ADDRESS+92)));        //REG 46
int ind_2_1 __attribute__ ((address(RAM_START_ADDRESS+94)));        //REG 47
int ind_0_2 __attribute__ ((address(RAM_START_ADDRESS+96)));        //REG 48
int ind_1_2 __attribute__ ((address(RAM_START_ADDRESS+98)));        //REG 49
int ind_2_2 __attribute__ ((address(RAM_START_ADDRESS+100)));       //REG 50
int ind_0_3 __attribute__ ((address(RAM_START_ADDRESS+102)));       //REG 51
int ind_1_3 __attribute__ ((address(RAM_START_ADDRESS+104)));       //REG 52
int ind_2_3 __attribute__ ((address(RAM_START_ADDRESS+106)));       //REG 53
int ind_0_4 __attribute__ ((address(RAM_START_ADDRESS+108)));       //REG 54
int ind_1_4 __attribute__ ((address(RAM_START_ADDRESS+110)));       //REG 55
int ind_2_4 __attribute__ ((address(RAM_START_ADDRESS+112)));       //REG 56

float P1   __attribute__  ((address(RAM_START_ADDRESS+114)));   //REG 57
float P2   __attribute__  ((address(RAM_START_ADDRESS+118)));   //REG 59
float P3   __attribute__  ((address(RAM_START_ADDRESS+122)));   //REG 61

float Q1   __attribute__ ((address(RAM_START_ADDRESS+126)));    //REG 63
float Q2   __attribute__ ((address(RAM_START_ADDRESS+130)));    //REG 65
float Q3   __attribute__ ((address(RAM_START_ADDRESS+134)));    //REG 67

float S1   __attribute__ ((address(RAM_START_ADDRESS+138)));    //REG 69
float S2   __attribute__ ((address(RAM_START_ADDRESS+142)));    //REG 71
float S3   __attribute__ ((address(RAM_START_ADDRESS+146)));    //REG 73

float cos_f1 __attribute__ ((address(RAM_START_ADDRESS+150)));    //REG 75
float cos_f2 __attribute__ ((address(RAM_START_ADDRESS+154)));    //REG 77
float cos_f3 __attribute__ ((address(RAM_START_ADDRESS+158)));    //REG 79

float AD4_AVG  __attribute__ ((address(RAM_START_ADDRESS+162)));       //REG 81
float AD5_AVG  __attribute__ ((address(RAM_START_ADDRESS+166)));       //REG 83
float AD6_AVG  __attribute__ ((address(RAM_START_ADDRESS+170)));       //REG 85

float AD4_RMS __attribute__ ((address(RAM_START_ADDRESS+174)));      //REG 87
float AD5_RMS __attribute__ ((address(RAM_START_ADDRESS+178)));      //REG 89
float AD6_RMS __attribute__ ((address(RAM_START_ADDRESS+182)));      //REG 91

int FLASH_WR __attribute__ ((address(RAM_START_ADDRESS+186)));      //REG 93
int FLASH_RD __attribute__ ((address(RAM_START_ADDRESS+188)));      //REG 94
int PROF_CHANGE_SOURCE __attribute__ ((address(RAM_START_ADDRESS+190)));//REG 95

unsigned int A0  __attribute__ ((address(RAM_START_ADDRESS+192)));      //REG 96
unsigned int A1  __attribute__ ((address(RAM_START_ADDRESS+194)));      //REG 97
unsigned int A2  __attribute__ ((address(RAM_START_ADDRESS+196)));      //REG 98
unsigned int A3  __attribute__ ((address(RAM_START_ADDRESS+198)));      //REG 99
unsigned int A4  __attribute__ ((address(RAM_START_ADDRESS+200)));      //REG 100
unsigned int A5  __attribute__ ((address(RAM_START_ADDRESS+202)));      //REG 101
unsigned int A6  __attribute__ ((address(RAM_START_ADDRESS+204)));      //REG 102

int ind_off_1 __attribute__ ((address(RAM_START_ADDRESS+206)));      //REG 103
int ind_off_2 __attribute__ ((address(RAM_START_ADDRESS+208)));      //REG 104
int ind_off_3 __attribute__ ((address(RAM_START_ADDRESS+210)));      //REG 105
int ind_off_4 __attribute__ ((address(RAM_START_ADDRESS+212)));      //REG 106

int  __attribute__ ((address(RAM_START_ADDRESS+214))) channel_num_U=0;      //REG 107
int  __attribute__ ((address(RAM_START_ADDRESS+216))) imp_kol=90;           //REG 108
int  __attribute__ ((address(RAM_START_ADDRESS+218))) V=0;                  //REG 109
int  __attribute__ ((address(RAM_START_ADDRESS+220))) D_Out_Init=0;         //REG 110
int  __attribute__ ((address(RAM_START_ADDRESS+222))) AD_12b=0;             //REG 111

int imp_kol_temp = 0;
int PARAM_SET;
int ind_off = 0x07;

//ÔÂÂÍÎ˛˜ÂÌËÂ ÔÓÙËÎÂÈ ËÌ‰ËÍ‡ˆËË ÔË ÔÓÏÓ˘Ë:
#define MODBUS                                  0x00
#define DISCRETE_INPUTS                         0x01
#define ADC                                     0x02

int _FLASH_STORE _FLASH_ACCESS flash_data_buf[21]; //ÍÓ˝ÙÙËˆËÂÌÚ˚ Ë ÓÙÙÒÂÚ˚

int PROF=1;
char MENU_LEVEL = 0;

extern char REG_num_0,REG_num_1,REG_num_2;//,SIZE_0,dot_pos_0;
extern float PARAM_1,PARAM_2,PARAM_3;
int* RamData, profile_sel;

int count_12_5us=0;
int count_1ms=0;
int count_1ms_2=0;

float* RamData_KOEF = &(K0);
float* RamData_ADC = &(ADC0);

int _temp_BAUD=0;

static long int arr[7]= {0, 0, 0, 0, 0, 0, 0}; //·ÛÙÂ ¿÷œ
static unsigned int init[7] = {0, 0, 0, 0, 0, 0, 0}; //·ÛÙÂ ¿÷œ
char i=0;

#define FILTER_COUNT 1000

unsigned int filter (unsigned int  value,char channel_num) //ÙËÎ¸Ú‡ˆËˇ ÁÌ‡˜ÂÌËÈ ‡ˆÔ
{
    if (init[channel_num] < FILTER_COUNT) {
        arr[channel_num] += value;
        ++init[channel_num];
        return 0;
    } else {
        arr[channel_num] += (-arr[channel_num]/FILTER_COUNT + value);
        return arr[channel_num]/FILTER_COUNT;
    }
//   unsigned  int middle=0;
//   unsigned  int a,b,c=0;
//
//    arr[channel_num][i] = value;
//
//    if (i == 2)
//    {
//        a=arr[channel_num][0];
//        b=arr[channel_num][1];
//        c=arr[channel_num][2];
//
//        if (((a >= b) && (a <= c)) || (( a >= c) && (a <= b)))       middle = a;
//        else
//            if (((b >= a) && (b <= c)) || ((b >= c) && (b <= a)))    middle = b;
//            else                                                   middle = c;
//
//       arr[channel_num][0]=b;
//       arr[channel_num][1]=c;
//       arr[channel_num][2]=0;
//       i=2;
//
//    }
//    else middle = value;
//
//    return middle;
}

void __attribute__((interrupt,no_auto_psv)) _QEIInterrupt()
{
     IFS3bits.QEIIF = 0;
}

void __attribute__((interrupt,no_auto_psv)) _ADC1Interrupt()
{
   //LATCbits.LATC6=1;//‰Îˇ ÚÂÒÚ‡

   A0=filter(ADC1BUF0,0);
   A1=filter(ADC1BUF1,1);
   A2=filter(ADC1BUF2,2);
//   A3=filter(ADC1BUF6,3);
//   A4=filter(ADC1BUF0,4);
//   A5=filter(ADC1BUF1,5);
//   A6=filter(ADC1BUF2,6);
   
   ADC0=((int)A0 - OFS_ADC0)*K0;   //AN6
   ADC1=((int)A1 - OFS_ADC1)*K1;   //AN3
   ADC2=((int)A2 - OFS_ADC2)*K2;   //AN4
//   ADC3=((int)A3 - OFS_ADC3)*K3;   //AN5
//   ADC4=((int)A4 - OFS_ADC4)*K4;   //AN0
//   ADC5=((int)A5 - OFS_ADC5)*K5;   //AN1
//   ADC6=((int)A6 - OFS_ADC6)*K6;   //AN2
 
//   if (i < 2) i++;
   
//   Data_Calc(ADC4,ADC5,ADC6);
   count_12_5us++;

   //LATCbits.LATC6=0;//‰Îˇ ÚÂÒÚ‡

   IFS0bits.AD1IF=0;
}

//80 kHz
void __attribute__((interrupt,no_auto_psv)) _T3Interrupt()
{
   IFS0bits.T3IF=0;                     //Ò·ÓÒ ÙÎ‡„‡ ÔÂ˚‚‡ÌËˇ
   TMR3=0;
}
unsigned int count_10ms=0;
//100 Hz PWM
void __attribute__((interrupt,no_auto_psv)) _T2Interrupt()
{
    count_10ms++;
    
    IFS0bits.T2IF=0;                     //Ò·ÓÒ ÙÎ‡„‡ ÔÂ˚‚‡ÌËˇ
    TMR2=0;

}

float K_v=0;
int V_temp,V_buf=0;
//1 kHz
void __attribute__((interrupt,no_auto_psv)) _T1Interrupt()
{
    count_1ms++;
    
    IFS0bits.T1IF = 0;	// Ò·ÓÒ ÙÎ‡„‡ ÔÂ˚‚‡ÌËˇ Ú‡ÈÏÂ‡
    TMR1= 0;		// Ó·ÌÛÎÂÌËÂ Ú‡ÈÏÂ‡
}

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main() {
    
    /* Initialize reset parser */
    system_init();

    mempool_init();
    
    cpu_ipl_set(0);
    
    /* Initialize Change-notification inputs */
    cni_init();
    
    /* Initialize oscillator switcher */
    osc_init();

    osc_select(oscBoosted);
    
    /* Initialize UART */
    uart_init();
    
    /* Initialize RTSP */
    flash_init();
    
    ADC_Init(1);     //ËÌËˆË‡ÎËÁ‡ˆËˇ ¿÷œ (‚ 12-·ËÚÌÓÏ ÂÊËÏÂ(1))
    DI_Init();      //ËÌËˆË‡ÎËÁ‡ˆËˇ ‰ËÒÍÂÚÌ˚ı ‚ıÓ‰Ó‚
    Encoder_Init(); //ËÌËˆË‡ÎËÁ‡ˆËˇ Í‚‡‰‡ÚÛÌÓ„Ó ‰ÂÍÓ‰Â‡
    
    RamData = &(BRG_VAL);
    
    FLASH_WR = 0;
    
    Ind_Delay = 100; 
    N = 4000;
    BRG_VAL = 19200;
   
    Init_Timer1();  //ËÌËˆË‡ÎËÁ‡ˆËˇ Ú‡ÈÏÂ‡ 1 (‰Îˇ ÔÓ‰Ò‚Â˜Ë‚‡ÌËˇ ËÌ‰ËÍ‡ÚÓÓ‚)
    Init_Timer2();  //ËÌËˆË‡ÎËÁ‡ˆËˇ Ú‡ÈÏÂ‡ 2 (‰Îˇ Á‡ÔËÒË ÁÌ‡˜ÂÌËÈ ‚ ÔÂÂÏÂÌÌ˚Â)
    //Init_Timer3();  //ËÌËˆË‡ÎËÁ‡ˆËˇ Ú‡ÈÏÂ‡ 3 ()

    char i=0;
    char AD_12b_temp=0;

    for (i = 0; i < 21; i++) RamData[FLASH_START+i]=flash_data_buf[i];

    IEC0bits.T1IE = 1; //‡ÁÂ¯ÂÌËÂ ÔÂ˚‚‡ÌËÈ ÓÚ Ú‡ÈÏÂ‡ 1
    
    while (1)
    {
     
//    if (imp_kol != imp_kol_temp) //ÔÂÂÒ˜ÂÚ ÍÓ˝ÙÙËˆËÂÌÚ‡ ‚ ÒÎÛ˜‡Â ËÁÏÂÌÂÌËˇ Ô‡‡ÏÂÚ‡ ÍÓ‰Ó‚Ó„Ó ‰ËÒÍ‡
//    {
//        K_v=((6.28*4)/(2*imp_kol)); // QEI_mode X2
//        imp_kol_temp = imp_kol;
//    }
//
//    if (count_12_5us >= N ) { Data_Calc_inc(channel_num_U); count_12_5us = 0;}

    if (FLASH_WR == 1)
    {
        for (i = 0; i < 21; i++) {
            if (flash_data_buf[i] != RamData[FLASH_START+i]) {
                flash_set(FLASH_GETPAGE(flash_data_buf), FLASH_GETAOFFSET(flash_data_buf, i),
                        RamData[FLASH_START+i]);
            }
        }
        flash_write();
        system_reset();
        FLASH_WR = 0;
    }
    Modbus_RTU();
    RS_Update();
    wdt_clr();
    }
}

