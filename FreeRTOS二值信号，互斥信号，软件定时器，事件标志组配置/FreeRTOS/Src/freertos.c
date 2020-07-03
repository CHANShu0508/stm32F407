/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "usart.h"
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
/* USER CODE BEGIN Variables */
EventGroupHandle_t Event_Handle =NULL;//�¼���־��
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osThreadId myTask03Handle;
osTimerId myTimer01Handle;					//�����ʱ��
osMutexId myMutex01Handle;					//�����ź���
osSemaphoreId myBinarySem01Handle;	//��ֵ�ź���

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
#define EVENT  (0x01 << 7)//�����¼������λ1  
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void Callback01(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];
  
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )  
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}                   
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of myMutex01 */
  osMutexDef(myMutex01);
  myMutex01Handle = osMutexCreate(osMutex(myMutex01));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of myBinarySem01 */
  osSemaphoreDef(myBinarySem01);
  myBinarySem01Handle = osSemaphoreCreate(osSemaphore(myBinarySem01), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of myTimer01 */
  osTimerDef(myTimer01, Callback01);
  myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 512);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  osThreadDef(myTask02, StartTask02, osPriorityAboveNormal, 0, 512);
  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* definition and creation of myTask03 */
  osThreadDef(myTask03, StartTask03, osPriorityHigh, 0, 512);
  myTask03Handle = osThreadCreate(osThread(myTask03), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  Event_Handle = xEventGroupCreate();	 
  if(NULL != Event_Handle)
    printf("Event_Handle �¼������ɹ�!\r\n");
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
		printf("����1�ȴ������ź���\n");
		osMutexWait(myMutex01Handle,osWaitForever);
//		osSemaphoreWait(myBinarySem01Handle,osWaitForever);  //�ȴ���ֵ�ź���
		printf("����1���յ������ź���\n");
		osDelay(5000);
		printf("����1�ͷŻ����ź���\n");
		osMutexRelease(myMutex01Handle);
//		osSemaphoreRelease(myBinarySem01Handle,osWaitForever); //�ͷŶ�ֵ�ź���
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
	EventBits_t r_event;  /* ����һ���¼����ձ��� */
  /* Infinite loop */
  for(;;)
  {
    /*******************************************************************
     * �ȴ������¼���־ 
     * 
     * ���xClearOnExit����ΪpdTRUE����ô��xEventGroupWaitBits()����֮ǰ��
     * �������ȴ�����������������ص�ԭ���ǳ�ʱ������ô���¼���������
     * ��uxBitsToWaitFor�е��κ�λ����������� 
     * ���xClearOnExit����ΪpdFALSE��
     * ���ڵ���xEventGroupWaitBits()ʱ����������¼��������õ�λ��
     *
     * xWaitForAllBits���xWaitForAllBits����ΪpdTRUE����uxBitsToWaitFor��
     * ������λ�����û�ָ���Ŀ�ʱ�䵽��ʱ��xEventGroupWaitBits()�ŷ��ء� 
     * ���xWaitForAllBits����ΪpdFALSE��������uxBitsToWaitFor�����õ��κ�
     * һ��λ��1 ��ָ���Ŀ�ʱ�䵽��ʱ��xEventGroupWaitBits()���᷵�ء� 
     * ����ʱ����xTicksToWait����ָ����          
      *********************************************************/
    r_event = xEventGroupWaitBits(Event_Handle,  /* �¼������� */
                                  EVENT,/* �����̸߳���Ȥ���¼� */
                                  pdTRUE,   /* �˳�ʱ����¼�λ */
                                  pdTRUE,   /* �������Ȥ�������¼� */
                                  portMAX_DELAY);/* ָ����ʱ�¼�,һֱ�� */
		if((r_event & EVENT) == EVENT) 
		{
			printf("\n\r��������¼���־��\n\r");
			xEventGroupClearBits(Event_Handle,EVENT);		
		}
//		printf("����2���ڽ���\n");
//    osDelay(1000);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
  /* Infinite loop */
  for(;;)
  {
		printf("\n\r���������ʱ��,1s����һ�λص�����\n\r");
		osTimerStart(myTimer01Handle,1000);
		printf("����3�ȴ������ź���\n");
		osMutexWait(myMutex01Handle,osWaitForever);
		printf("����3���յ������ź���\n");
		printf("����3�ͷŻ����ź���\n");
		osMutexRelease(myMutex01Handle);
    osDelay(1000);
		xEventGroupCreate();
  }
  /* USER CODE END StartTask03 */
}

/* Callback01 function */
void Callback01(void const * argument)		//�����ʱ���ص�����
{
  /* USER CODE BEGIN Callback01 */
	static uint8_t NumberCount;
	NumberCount++;
	if(NumberCount==5){
		NumberCount=0;
		xEventGroupSetBits(Event_Handle,EVENT);
	}
  printf("\n\r���������ʱ���ص�����\n\r");
  /* USER CODE END Callback01 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
