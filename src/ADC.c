
#include "ADC.h"
#include "system.h"
#include "board-config.h"
#include "ipl-config.h"

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
    IPC3bits.AD1IP=IPL_ADC_DATAR;
    
    // Start ADC
    AD1CON1bits.ADON=1;
}

void __attribute__((interrupt,no_auto_psv)) _ADC1Interrupt() {
    filter_put(ADC1BUF0,0);
    filter_put(ADC1BUF1,1);
    filter_put(ADC1BUF2,2);
    filter_put(ADC1BUF3,3);
    filter_put(ADC1BUF4,4);
    filter_put(ADC1BUF5,5);
    filter_put(ADC1BUF6,6);
      
    IFS0bits.AD1IF=0;
}