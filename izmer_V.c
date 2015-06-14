#include <p33FJ32MC204.h>

void Encoder_Init()
{
	RPINR14bits.QEA1R	= 17;		// ������� ������ RP17 ������� ����� A
	RPINR14bits.QEB1R	= 18;		// ������� ������ RP16 ������� ����� B

        MAXCNT = 15000;                         // ����� �������� �������� ��� ���������� �������� ���� ������������
	QEICONbits.QEIM	= 0b101;                // ����� ������ ������ -
						// ����� �� ������������ (X2)
        DFLT1CONbits.CEID = 1;
        DFLT1CONbits.QEOUT = 1;

        //IEC3bits.QEIIE = 1;
}

