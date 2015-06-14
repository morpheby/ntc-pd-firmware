
#include "system.h"

#define FD                              80000
#define FS                              1000


#define DEV1                    8
#define FO1                     FCY/DEV1
#define pT_1                    FO1/FS

void Init_Timer1() {
   // TODO: annotate registers from documentation
   T1CON = 0;
   T1CONbits.TCKPS = 0b01;
   IFS0bits.T1IF = 0;
   //IEC0bits.T1IE = 1;
   IPC0bits.T1IP = 3;
   TMR1 = 0x0000;
   PR1 = 4949;
   T1CONbits.TON = 1;
}

#define pT_2                    FO1/100
void Init_Timer2() {
   // TODO: annotate registers from documentation
   T2CON = 0;
   T2CONbits.TCKPS = 0b01;
   IFS0bits.T2IF = 0;
   IEC0bits.T2IE = 1;
   IPC1bits.T2IP = 3;
   TMR2 = 0x0000;
   PR2 = pT_2;
   T2CONbits.T32=0;
   T2CONbits.TON = 1;
}

#define DEV3                    8
#define FO3                     FCY/DEV3
#define pT_3                    FO3/FD
void Init_Timer3() {
   // TODO: annotate registers from documentation
   T3CON = 0;
   T3CONbits.TCKPS = 0b01;
   IFS0bits.T3IF = 0;
   IEC0bits.T3IE = 3;
   TMR3 = 0x0000;
   PR3 = pT_1;
   //T2CONbits.T32=0;
   T3CONbits.TON = 1;
}

