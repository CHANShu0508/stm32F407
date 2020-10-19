#include "VL53L1.h"

extern uint8_t Uart6_Rx_Cnt;
		
/***				��ȡ������ֵ����ȡ�ɹ���Ϊ����ֵ��ʧ���򷵻�NULL  ***/

int Get_Distance_VL53L1(unsigned char Uart6_RxBuffer[])
{
	int Data;
	unsigned char *p;
	if((Uart6_RxBuffer[Uart6_Rx_Cnt-1] == 'm') && (Uart6_RxBuffer[Uart6_Rx_Cnt-2] == 'm'))
	{
		Data = 0;
		p=(unsigned char*)strstr((char*)Uart6_RxBuffer,"d:");
		while(*p!='m')
		{
			if(*p>='0'&&*p<='9')
				Data=Data*10+(*p-'0');
			p++;		
		}
		printf("����:%dmm\r\n",Data);
		return Data;
	}
	else 
		return -1;
}
