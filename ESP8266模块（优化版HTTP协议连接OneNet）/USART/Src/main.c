/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
uint8_t my_re_buf2;
uint8_t aRxBuffer;			//�����жϻ���
//char RST_Start[]="AT+RST\r\n";
//char MODE_Start[]="AT+CWMODE=1\r\n";
//char WIFI_Start[]="AT+CWJAP=\"HUAWEI-3AHFVD\",\"13550695909\"\r\n";
//char Connected_Start[]="AT+CIPMUX=0\r\n";
//char TCP_Start[]="AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n";
//char TX_Start[]="AT+CIPMODE=1\r\n";
//char Data[]="AT+CIPSEND\r\n";
//char TX_Data[]="POST /devices/610177377/datapoints HTTP/1.1\r\napi-key:Nhw=N=8b0eElqPR0k2Ultdyh0tA=\r\nHost:api.heclouds.com\r\nContent-Length:63\r\n\r\n{\"datastreams\":[{\"id\":\"sys_time\",\"datapoints\":[{\"value\":50}]}]}";

#define AT          "AT\r\n"	
#define CWMODE      "AT+CWMODE=1\r\n"		//STA+APģʽ
#define RST         "AT+RST\r\n"
#define CIFSR       "AT+CIFSR\r\n"
#define CWJAP       "AT+CWJAP=\"HUAWEI-3AHFVD\",\"13550695909\"\r\n"	//
//#define CIPSTART    "AT+CIPSTART=\"TCP\",\"183.230.40.39\",876\r\n"	//EDP������ 183.230.40.39/876
#define CIPSTART    "AT+CIPSTART=\"TCP\",\"183.230.40.33\",80\r\n"		//HTTP������183.230.40.33/80
#define CIPMODE0    "AT+CIPMODE=0\r\n"		//��͸��ģʽ
#define CIPMODE1    "AT+CIPMODE=1\r\n"		//͸��ģʽ
#define CIPSEND     "AT+CIPSEND\r\n"
#define CIPSTATUS   "AT+CIPSTATUS\r\n"		//����״̬��ѯ

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#define MAX_RCV_LEN  500
#define API_KEY     "Nhw=N=8b0eElqPR0k2Ultdyh0tA="		//��Ҫ����Ϊ�û��Լ��Ĳ���
#define DEV_ID      "610177377"							          //��Ҫ����Ϊ�û��Լ��Ĳ���
unsigned char  usart2_rcv_buf[MAX_RCV_LEN];
char count;
/*
*  @brief USART2���ڽ���״̬��ʼ��
*/
void USART2_Clear(void)
{

		memset(usart2_rcv_buf, 0, sizeof(usart2_rcv_buf));
    count=0;
}

/*
*  @brief USART2���ڷ���api
*/
void USART2_Write(USART_TypeDef* USARTx, uint8_t *Data, uint8_t len)
{
	HAL_UART_Transmit(&huart2, Data, len,0xff);
  while(HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_TX);//���UART���ͽ���   
}

/*
 *  @brief USART2���ڷ���AT������
 *  @para  cmd  AT����
 *  @para  result Ԥ�ڵ���ȷ������Ϣ
 *  @para  timeOut��ʱʱ��,ms
 */
void SendCmd(char* cmd, char* result, int timeOut)
{
    while(1)
    {
        USART2_Clear();
        USART2_Write(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
        HAL_Delay(timeOut);
        printf("%s %d cmd:%s,rsp:%s\n", __func__, __LINE__, cmd, usart2_rcv_buf);
        if((NULL != strstr((const char *)usart2_rcv_buf, result)))	//�ж��Ƿ���Ԥ�ڵĽ��
        {
						USART2_Clear();
            break;
        }
        else
        {		
						USART2_Clear();
            HAL_Delay(100);
        }
    }
}



/*
 *  @brief ESP8266ģ���ʼ��
 */
void ESP8266_Init(void)
{
    SendCmd(AT, "OK", 1000);		//ģ����Ч�Լ��
    SendCmd(CWMODE, "OK", 2000);	//ģ�鹤��ģʽ
    SendCmd(RST, "", 2000);		//ģ������
    SendCmd(CIFSR, "OK", 1000);		//��ѯ������Ϣ
    SendCmd(CWJAP, "OK", 2000);		//������Ҫ���ӵ�WIFI�ȵ�SSID������
    SendCmd(CIPSTART, "OK", 2000);	//TCP����
    SendCmd(CIPMODE1, "OK", 1000);	//����͸��ģʽ
		SendCmd(CIPSEND, ">", 1000);	//����͸��ģʽ
		USART2_Clear();

}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
  * @brief   ��HTTP POST����
  * @param   pkt   ���Ļ���ָ��
  * @param   key   API_KEY������Main.c�ļ��У���Ҫ�����Լ����豸�޸�
  *	@param 	 devid �豸ID��������main.c�ļ��У���Ҫ�����Լ����豸�޸�
  *	@param 	 dsid  ������ID
  *	@param 	 val   �ַ�����ʽ�����ݵ��ֵ
  * @retval  �������ĳ���
  */
uint32_t HTTP_PostPkt(char *pkt, char *key, char *devid, char *dsid, char *val)
{
    char dataBuf[100] = {0};
    char lenBuf[10] = {0};
    *pkt = 0;

    sprintf(dataBuf, ",;%s,%s", dsid, val);     //���÷ָ��ַ�����ʽ:type = 5
    sprintf(lenBuf, "%d", strlen(dataBuf));

    strcat(pkt, "POST /devices/");
    strcat(pkt, devid);
    strcat(pkt, "/datapoints?type=5 HTTP/1.1\r\n");

    strcat(pkt, "api-key:");
    strcat(pkt, key);
    strcat(pkt, "\r\n");

    strcat(pkt, "Host:api.heclouds.com\r\n");

    strcat(pkt, "Content-Length:");
    strcat(pkt, lenBuf);
    strcat(pkt, "\r\n\r\n");

    strcat(pkt, dataBuf);

    return strlen(pkt);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	char HTTP_Buf[400];     //HTTP���Ļ�����
	char tempStr[]="30";       //�ַ�����ʽ�¶�
	int len;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);
	USART2_Clear();
	ESP8266_Init();
	HAL_Delay(1000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		
		len = HTTP_PostPkt(HTTP_Buf, API_KEY, DEV_ID, "temp", tempStr); //HTTP���;
		USART2_Write(USART2, (unsigned char *)(HTTP_Buf), len);			//���ķ���
		printf("send HTTP msg:\r\n%s\r\n", HTTP_Buf);
		USART2_Clear();
		HAL_Delay(5000);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	UNUSED(huart);
	if(huart==&huart2)
	{
//		my_re_buf2 = aRxBuffer;
		usart2_rcv_buf[count++]=aRxBuffer;
//		HAL_UART_Transmit(&huart1, (uint8_t *)&my_re_buf2, 1,0xff);
//		while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);//���UART���ͽ���
		
		HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer, 1);
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
