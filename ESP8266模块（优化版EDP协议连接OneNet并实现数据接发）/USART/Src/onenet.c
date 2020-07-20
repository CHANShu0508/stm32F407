/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
	*
	*	�汾�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
	*				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
	
	*
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "main.h"

//�����豸


//Э���ļ�
#include "onenet.h"
#include "edpkit.h"

//Ӳ������
#include "usart.h"


//C��
#include <string.h>
#include <stdio.h>


#define DEVID	"611678262"

#define APIKEY	"0nvdlujSYYdK81esrgh0wRPSs80="
extern void USART2_Write(USART_TypeDef* USARTx, uint8_t *Data, uint8_t len);
//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};				//Э���
	
	unsigned char status = 1;
	

	if(EDP_PacketConnect1(DEVID, APIKEY, 256, &edpPacket) == 0)		//����devid �� apikey��װЭ���
	{
		
		USART2_Write(USART2,edpPacket._data, edpPacket._len);			//�ϴ�ƽ̨
	
		HAL_Delay(500);      //�ӳٵȴ�ʱ�����300ms һ��Ҫ�� ��Ȼ������
		EDP_DeleteBuffer(&edpPacket);								//ɾ��
		status=0;
	}
	
	return status;
	
}

unsigned char OneNet_FillBuf(char *buf,char *DataStream,int data)
{
	
	char text[16];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"%s\":%d",DataStream,data);
	strcat(buf, text);
	
	strcat(buf, "}");
	memset(text, 0, sizeof(text));
	return strlen(buf);

}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����DataStream ���������֣� data ��ֵ		
//==========================================================
void OneNet_SendData(char *DataStream,int data)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[128];
	
	short body_len = 0, i = 0;

	memset(buf, 0, sizeof(buf));
//	memset(Rx_Buff, 0, sizeof(Rx_Buff));
//	Rx_count=0;
	body_len = OneNet_FillBuf(buf,DataStream,data);													//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	if(body_len)
	{
		if(EDP_PacketSaveData(DEVID, body_len, NULL, kTypeSimpleJsonWithoutTime, &edpPacket) == 0)	//���
		{
			for(; i < body_len; i++)
				edpPacket._data[edpPacket._len++] = buf[i];
			
			USART2_Write(USART2,edpPacket._data, edpPacket._len);										//�ϴ����ݵ�ƽ̨
			
			EDP_DeleteBuffer(&edpPacket);															//ɾ��	
		}
	}
	
}
