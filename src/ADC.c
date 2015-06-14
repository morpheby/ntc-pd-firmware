
#include "ADC.h"
#include "system.h"

void ADC_Init(_Bool ad_12b) {
    // Setup pins as inputs
    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;
    TRISBbits.TRISB3 = 1;
    TRISCbits.TRISC0 = 1;

    // Configure as digital inputs
    AD1PCFGLbits.PCFG6 = 1;
    AD1PCFGLbits.PCFG5 = 1;
    AD1PCFGLbits.PCFG4 = 1;
    AD1PCFGLbits.PCFG3 = 1;
    AD1PCFGLbits.PCFG8 = 1;
    AD1PCFGLbits.PCFG7 = 1;
    
    // Configure as analog inputs
    AD1PCFGLbits.PCFG2 = 0;  //M2
    AD1PCFGLbits.PCFG1 = 0;  //M1
    AD1PCFGLbits.PCFG0 = 0;  //M0

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
        AD1CON2bits.SMPI = 0x03;
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

    // Enable input scans for AN0-AN2
    AD1CSSLbits.CSS0 = 1;
    AD1CSSLbits.CSS1 = 1;
    AD1CSSLbits.CSS2 = 1;
    
    // Disable for AN3-AN6
    AD1CSSLbits.CSS3 = 0;
    AD1CSSLbits.CSS4 = 0;
    AD1CSSLbits.CSS5 = 0;
    AD1CSSLbits.CSS6 = 0;

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
}



