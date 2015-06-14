#include <p33FJ32MC204.h>

void Encoder_Init()
{
	RPINR14bits.QEA1R	= 17;		// Задание выводу RP17 функции входа A
	RPINR14bits.QEB1R	= 18;		// Задание выводу RP16 функции входа B

        MAXCNT = 15000;                         // сброс счетного регистра при превышении скорости выше максимальной
	QEICONbits.QEIM	= 0b101;                // Режим работы модуля -
						// сброс по переполнению (X2)
        DFLT1CONbits.CEID = 1;
        DFLT1CONbits.QEOUT = 1;

        //IEC3bits.QEIIE = 1;
}

