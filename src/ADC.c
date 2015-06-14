#include "ADC.h"
#include <P33FJ32MC204.h>

//extern float ADC0,ADC1,ADC2,ADC3,ADC4,ADC5,ADC6,K0,K1,K2,K3,K4,K5,K6;
//extern int   OFS_ADC0,OFS_ADC1,OFS_ADC2,OFS_ADC3,OFS_ADC4,OFS_ADC5,OFS_ADC6;
//extern unsigned int A0,A1,A2,A3,A4,A5,A6;


void ADC_Init(char ad_12b)
{
    TRISBbits.TRISB1=1;   //настройка портов на входы
    TRISBbits.TRISB2=1;   //
    TRISBbits.TRISB3=1;
    TRISCbits.TRISC0=1;

   // AD1PCFGLbits.PCFG=0b000000000; //AN3,AN4,AN5,AN6 - как аналоговые входы
    AD1PCFGLbits.PCFG6=1;
    AD1PCFGLbits.PCFG5=1;
    AD1PCFGLbits.PCFG4=1;
    AD1PCFGLbits.PCFG3=1;
    AD1PCFGLbits.PCFG2=0;  //M2
    AD1PCFGLbits.PCFG1=0;  //M1
    AD1PCFGLbits.PCFG0=0;  //M0

    AD1PCFGLbits.PCFG8=1; //RC1,RC2-как цифровые порты
    AD1PCFGLbits.PCFG7=1; //

    if (ad_12b) AD1CON1bits.AD12B=1;    //12 битный, 1 канальный р-м АЦП
    else        AD1CON1bits.AD12B=0;    // 10 битный, 4-х канальный режим АЦП

    AD1CON1bits.FORM=0b00;  // data out 0000 00dd dddd dddd
    AD1CON1bits.SSRC=0b111;  // сброс бита выборки начинает преобразование
                             // и заканчивает выборку
    AD1CON1bits.SIMSAM=1;    //одновременное преобразование каналов 0,1,2,3
    AD1CON1bits.ASAM=1;      // преобразование начинается сразу же после
                             // завершения предыдущего
    //AD1CON1bits.DONE= бит статуса преобразования
    //1-преобразование завершено,0-процесс преобразования все еще идет
    
    AD1CON2bits.VCFG=0b111;  // Vref+  ---> AVdd, Vref- ---> AVss
    AD1CON2bits.CHPS=0b11;   // одновременное преобразование каналов 0,1,2,3

    if (ad_12b) AD1CON2bits.SMPI=0x03;
    else        AD1CON2bits.SMPI=0b0001; //Increments the DMA address or generates interrupt after completion of every 2nd
                             //sample/conversion operation

    if (ad_12b) AD1CON2bits.ALTS=0;     // ALWAYS MUX A
    else        AD1CON2bits.ALTS=1;     // MUX A , MUX B

    AD1CSSLbits.CSS0=1; //AN0 input scan enable
    AD1CSSLbits.CSS1=1; //AN1 input scan enable
    AD1CSSLbits.CSS2=1; //AN2 input scan enable
    AD1CSSLbits.CSS3=0; //AN3 input scan enable
    AD1CSSLbits.CSS4=0; //AN4 input scan enable
    AD1CSSLbits.CSS5=0; //AN5 input scan enable
    AD1CSSLbits.CSS6=0; //AN6 input scan enable

    if (ad_12b) AD1CON2bits.CSCNA=1; //SCAN AN0-AN6
    else        AD1CON2bits.CSCNA=0;

//    AD1CON3bits.ADRC=1;      // ADC internal RC clock источник тактовой частоты
    if (ad_12b) AD1CON3bits.ADCS=0x0A;
//    if (ad_12b) AD1CON3bits.ADCS=0x02;
    else        AD1CON3bits.ADCS=0x18;       //Tad=Tcy(24+1) Tad = 0.63uS
    
//    AD1CON3bits.SAMC=12; //SAMPLE TIME = 12*Tad
    AD1CON3bits.SAMC=0b11111; //SAMPLE TIME = 31*Tad
    //MUXA CONFIGURATION
    
    AD1CHS123bits.CH123NA=0b00; // CH1,2,3 negative input is Vref- (MUXA)
    AD1CHS123bits.CH123SA=1;    // AN3,AN4,AN5 positive inputs of CH1,2,3(MUXA)

    AD1CHS0bits.CH0NA=0;        //CH0 negative input is Vref- (MUXA)
    AD1CHS0bits.CH0SA=0b00110;  //AN6 is  positive input of CH0(MUXA)

    //MUXB CONFIGURATION
    AD1CHS123bits.CH123NB=0b00; // CH1,2,3 negative input is Vref- (MUXB)
    AD1CHS123bits.CH123SB=0;    // AN0,AN1,AN2 positive inputs of CH1,2,3(MUXB)

    IEC0bits.AD1IE=1;           //разрешаем прерывание от АЦП
    IFS0bits.AD1IF=0;           //очистка флага прерывания
    IPC3bits.AD1IP=7;//5;           //приоритет прерывания 7(высший)
    AD1CON1bits.ADON=1;         // вкл. модуль АЦП
}



