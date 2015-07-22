
#include "ADC.h"
#include "system.h"
#include "board-config.h"
#include <math.h>

static float ADC4Sum;
static float ADC5Sum;
static float ADC6Sum;

static float ADC4SumSq;
static float ADC5SumSq;
static float ADC6SumSq;

int iterationsCount;

void ADC_Init(_Bool ad_12b) {
    // Setup pins as inputs
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;
    TRISBbits.TRISB3 = 1;
    TRISCbits.TRISC0 = 1;

    // Configure as digital inputs
    AD1PCFGLbits.PCFG8 = 1;
    AD1PCFGLbits.PCFG7 = 1;
    
    // Configure as analog inputs
    AD1PCFGLbits.PCFG0 = 0;  //M0
    AD1PCFGLbits.PCFG2 = 0;  //M2
    AD1PCFGLbits.PCFG1 = 0;  //M1
    AD1PCFGLbits.PCFG3 = 0;  //A0
    AD1PCFGLbits.PCFG4 = 0;  //A1
    AD1PCFGLbits.PCFG5 = 0;  //A2
    AD1PCFGLbits.PCFG6 = 0;  //A3
    AD1PCFGLbits.PCFG7 = 1;  //DI2
    AD1PCFGLbits.PCFG8 = 1;  //DI3

    if (ad_12b) {
        //12 bit mode
        AD1CON1bits.AD12B = 1;
    } else {
        // 10 bit mode
        AD1CON1bits.AD12B = 0;
    }
    
    // TODO: annotate registers from documentation
    AD1CON1bits.FORM = 0b00;
    AD1CON1bits.SSRC = 0b111;
    AD1CON1bits.SIMSAM = 1;
    AD1CON1bits.ASAM = 1;
    
    AD1CON2bits.VCFG = 0b111;
    AD1CON2bits.CHPS = 0b11;

    if (ad_12b) {
        AD1CON2bits.SMPI = ADC_0_ENABLE + ADC_1_ENABLE + ADC_2_ENABLE
                         + ADC_3_ENABLE + ADC_4_ENABLE + ADC_5_ENABLE
                         + ADC_6_ENABLE;
    } else {
        //Increments the DMA address or generates interrupt after completion
        // of every 2nd sample/conversion operation
        AD1CON2bits.SMPI = 0b0001;
    }
    
    if (ad_12b) {
        // ALWAYS MUX A
        AD1CON2bits.ALTS = 0;
    } else {
        // MUX A , MUX B
        AD1CON2bits.ALTS = 1;
    }

    // Enable input scans for AN0-AN6 according to board-config.h definitions
    AD1CSSLbits.CSS0 = ADC_0_ENABLE;
    AD1CSSLbits.CSS1 = ADC_1_ENABLE;
    AD1CSSLbits.CSS2 = ADC_2_ENABLE;
    AD1CSSLbits.CSS3 = ADC_3_ENABLE;
    AD1CSSLbits.CSS4 = ADC_4_ENABLE;
    AD1CSSLbits.CSS5 = ADC_5_ENABLE;
    AD1CSSLbits.CSS6 = ADC_6_ENABLE;
    
    if (ad_12b) {
        //SCAN AN0-AN6
        AD1CON2bits.CSCNA = 1; 
    } else {
        AD1CON2bits.CSCNA = 0;
    }

    AD1CON3bits.ADRC = 0;
    if (ad_12b) {
        AD1CON3bits.ADCS = 0x0A;
    }else {
        //Tad=Tcy(24+1) Tad = 0.63uS
        AD1CON3bits.ADCS=0x18;
    }
    
    //SAMPLE TIME = 31*Tad
    AD1CON3bits.SAMC=0b11111;
    
    //MUXA CONFIGURATION
    
     // CH1,2,3 negative input is Vref- (MUXA)
    AD1CHS123bits.CH123NA=0b00;
    // AN3,AN4,AN5 positive inputs of CH1,2,3(MUXA)
    AD1CHS123bits.CH123SA=1;

    //CH0 negative input is Vref- (MUXA)
    AD1CHS0bits.CH0NA=0;
    //AN6 is  positive input of CH0(MUXA)
    AD1CHS0bits.CH0SA=0b00110;

    //MUXB CONFIGURATION
    
    // CH1,2,3 negative input is Vref- (MUXB)
    AD1CHS123bits.CH123NB=0b00;
    // AN0,AN1,AN2 positive inputs of CH1,2,3(MUXB)
    AD1CHS123bits.CH123SB=0;

    // Enable interrupts
    IEC0bits.AD1IE=1;
    
    // Clear interrupt flag
    IFS0bits.AD1IF=0;
    
    // Set interrupt priority
    IPC3bits.AD1IP=7;
    
    // Start ADC
    AD1CON1bits.ADON=1;

    setFilterType(AVG_VALUE_FILTER);
}

void __attribute__((interrupt,no_auto_psv)) _ADC1Interrupt() {
    MB.A0=doFilter(ADC1BUF0,0);
    MB.A1=doFilter(ADC1BUF1,1);
    MB.A2=doFilter(ADC1BUF2,2);
    MB.A3=doFilter(ADC1BUF3,3);
    MB.A4=doFilter(ADC1BUF4,4);
    MB.A5=doFilter(ADC1BUF5,5);
    MB.A6=doFilter(ADC1BUF6,6);
   
    MB.ADC0 = (MB.A0 - MB.OFS_ADC0)*MB.K0;
    MB.ADC1 = (MB.A1 - MB.OFS_ADC1)*MB.K1;
    MB.ADC2 = (MB.A2 - MB.OFS_ADC2)*MB.K2;
    MB.ADC3 = (MB.A3 - MB.OFS_ADC3)*MB.K3;
    MB.ADC4 = (MB.A4 - MB.OFS_ADC4)*MB.K4;
    MB.ADC5 = (MB.A5 - MB.OFS_ADC5)*MB.K5;
    MB.ADC6 = (MB.A6 - MB.OFS_ADC6)*MB.K6;
        
    ADC4Sum += MB.ADC4;
    ADC5Sum += MB.ADC5;
    ADC6Sum += MB.ADC6;
    
    ADC4SumSq += (MB.ADC4*MB.ADC4);
    ADC5SumSq += (MB.ADC5*MB.ADC5);
    ADC6SumSq += (MB.ADC6*MB.ADC6);
    
    iterationsCount++;
    if(iterationsCount>MB.N)
    {
        MB.AD4_AVG = ADC4Sum / MB.N;
        MB.AD5_AVG = ADC5Sum / MB.N;
        MB.AD6_AVG = ADC6Sum / MB.N;
                
        MB.AD4_RMS = sqrt(ADC4SumSq / MB.N);
        MB.AD5_RMS = sqrt(ADC5SumSq / MB.N);
        MB.AD6_RMS = sqrt(ADC6SumSq / MB.N);
        
        iterationsCount = 0;
        ADC4Sum = 0;
        ADC5Sum = 0;
        ADC6Sum = 0;
        
        ADC4SumSq = 0;
        ADC5SumSq = 0;
        ADC6SumSq = 0;
    }
      
   IFS0bits.AD1IF=0;
}