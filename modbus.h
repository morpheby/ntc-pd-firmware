

void RS_Send(int* buf,unsigned char size,unsigned char ADR, char Op_Code);

char RS_CheckCRC();
void RS_Reset();
void RS_Update();

unsigned short Crc16(unsigned char * pcBlock, unsigned short len);

