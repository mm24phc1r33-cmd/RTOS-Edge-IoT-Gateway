/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : RTOS Edge IoT Gateway - STAGE 10.1
  *
  * STM32F401RE Nucleo
  *
  * USART1:
  * PA9  -> TX -> ESP32 GPIO16 RX2
  * PA10 <- RX <- ESP32 GPIO17 TX2
  *
  * USART2:
  * PA2  -> TX -> USB-TTL RX
  * PA3  <- RX <- USB-TTL TX
  *
  * Stage 10.1:
  * STM32 <-> ESP32 ACK Protocol
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "cmsis_os2.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "SEGGER_SYSVIEW.h"
#include "SEGGER_RTT.h"


/* ========================================================================== */
/*                            UART HANDLES                                    */
/* ========================================================================== */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;


/* ========================================================================== */
/*                         SENSOR STRUCTURE                                   */
/* ========================================================================== */

typedef struct
{
    int temperature;
    int humidity;
    int status;
    int error_count;

} SensorData_t;


/* ========================================================================== */
/*                         RTOS OBJECTS                                       */
/* ========================================================================== */

osThreadId_t defaultTaskHandle;
osThreadId_t sensorTaskHandle;
osThreadId_t communicationTaskHandle;
osThreadId_t monitorTaskHandle;

osMessageQueueId_t sensorQueueHandle;


/* ========================================================================== */
/*                         TASK ATTRIBUTES                                    */
/* ========================================================================== */

const osThreadAttr_t defaultTask_attributes =
{
    .name = "LED_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityNormal
};


const osThreadAttr_t sensorTask_attributes =
{
    .name = "Sensor_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityNormal
};


const osThreadAttr_t communicationTask_attributes =
{
    .name = "Communication_Task",
    .stack_size = 768 * 4,
    .priority = (osPriority_t)osPriorityAboveNormal
};


const osThreadAttr_t monitorTask_attributes =
{
    .name = "Monitor_Task",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityLow
};


/* ========================================================================== */
/*                         FUNCTION PROTOTYPES                                */
/* ========================================================================== */

void SystemClock_Config(void);

static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

static void DWT_Init(void);
static void SystemView_Init(void);

static void UART_Send(const char *message);
static void ESP_UART_Send(const char *message);

static void ProcessESP32Message(char *message);

void StartDefaultTask(void *argument);
void StartSensorTask(void *argument);
void StartCommunicationTask(void *argument);
void StartMonitorTask(void *argument);


/* ========================================================================== */
/*                              MAIN                                          */
/* ========================================================================== */

int main(void)
{
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

    MX_USART1_UART_Init();

    MX_USART2_UART_Init();


    /* ====================================================================== */
    /*                         SYSTEMVIEW                                      */
    /* ====================================================================== */

    SystemView_Init();


    /* ====================================================================== */
    /*                         STARTUP                                        */
    /* ====================================================================== */

    UART_Send("\r\n");
    UART_Send("========================================\r\n");
    UART_Send("       RTOS EDGE IoT GATEWAY\r\n");
    UART_Send("              STAGE 10.1\r\n");
    UART_Send("========================================\r\n");

    UART_Send("\r\n");

    UART_Send("STM32F401RE NUCLEO\r\n");

    UART_Send("\r\n");

    UART_Send("USART1 -> ESP32 UART2\r\n");
    UART_Send("PA9  -> ESP32 GPIO16 RX2\r\n");
    UART_Send("PA10 <- ESP32 GPIO17 TX2\r\n");

    UART_Send("\r\n");

    UART_Send("USART2 -> USB-TTL / X-CTU\r\n");
    UART_Send("PA2  -> TTL RX\r\n");
    UART_Send("PA3  <- TTL TX\r\n");

    UART_Send("\r\n");

    UART_Send("Baud = 115200\r\n");

    UART_Send("\r\n");

    UART_Send("SIMULATED SENSOR\r\n");
    UART_Send("Temperature = 28 C\r\n");
    UART_Send("Humidity = 62 %\r\n");

    UART_Send("\r\n");

    UART_Send("STAGE 10.1\r\n");
    UART_Send("STM32 <-> ESP32 ACK PROTOCOL\r\n");

    UART_Send("\r\n");


    /* ====================================================================== */
    /*                         RTOS INITIALIZE                                 */
    /* ====================================================================== */

    osKernelInitialize();


    /* ====================================================================== */
    /*                         SENSOR QUEUE                                    */
    /* ====================================================================== */

    sensorQueueHandle = osMessageQueueNew(
        5,
        sizeof(SensorData_t),
        NULL
    );


    if (sensorQueueHandle == NULL)
    {
        UART_Send(
            "ERROR: Sensor Queue Creation Failed\r\n"
        );

        Error_Handler();
    }


    /* ====================================================================== */
    /*                         TASK CREATION                                   */
    /* ====================================================================== */

    defaultTaskHandle = osThreadNew(
        StartDefaultTask,
        NULL,
        &defaultTask_attributes
    );


    sensorTaskHandle = osThreadNew(
        StartSensorTask,
        NULL,
        &sensorTask_attributes
    );


    communicationTaskHandle = osThreadNew(
        StartCommunicationTask,
        NULL,
        &communicationTask_attributes
    );


    monitorTaskHandle = osThreadNew(
        StartMonitorTask,
        NULL,
        &monitorTask_attributes
    );


    UART_Send(
        "RTOS TASKS CREATED\r\n"
    );


    UART_Send(
        "Starting FreeRTOS...\r\n"
    );


    UART_Send(
        "========================================\r\n\r\n"
    );


    /* ====================================================================== */
    /*                         START RTOS                                      */
    /* ====================================================================== */

    osKernelStart();


    while (1)
    {
    }
}


/* ========================================================================== */
/*                           DWT INIT                                         */
/* ========================================================================== */

static void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    DWT->CYCCNT = 0;

    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}


/* ========================================================================== */
/*                         SYSTEMVIEW INIT                                    */
/* ========================================================================== */

static void SystemView_Init(void)
{
    DWT_Init();

    SEGGER_SYSVIEW_Conf();

    SEGGER_SYSVIEW_Start();
}


/* ========================================================================== */
/*                         USB-TTL UART                                       */
/* ========================================================================== */

static void UART_Send(const char *message)
{
    HAL_UART_Transmit(
        &huart2,
        (uint8_t *)message,
        strlen(message),
        HAL_MAX_DELAY
    );
}


/* ========================================================================== */
/*                         ESP32 UART                                         */
/* ========================================================================== */

static void ESP_UART_Send(const char *message)
{
    HAL_UART_Transmit(
        &huart1,
        (uint8_t *)message,
        strlen(message),
        HAL_MAX_DELAY
    );
}


/* ========================================================================== */
/*                         PROCESS ESP32 MESSAGE                              */
/* ========================================================================== */

static void ProcessESP32Message(char *message)
{
    if (message == NULL)
    {
        return;
    }


    if (strlen(message) == 0)
    {
        return;
    }


    /* ====================================================================== */
    /*                         ESP32 READY                                    */
    /* ====================================================================== */

    if (strcmp(message, "ESP32_READY") == 0)
    {
        UART_Send(
            "ESP32 -> STM32 : ESP32_READY\r\n"
        );

        UART_Send(
            "ESP32 STATUS: READY\r\n"
        );


        SEGGER_SYSVIEW_PrintfHost(
            "ESP32 READY"
        );


        ESP_UART_Send(
            "STM32_READY\r\n"
        );


        UART_Send(
            "STM32 -> ESP32 : STM32_READY\r\n"
        );
    }


    /* ====================================================================== */
    /*                         ACK                                           */
    /* ====================================================================== */

    else if (strcmp(message, "ACK,SENSOR_DATA") == 0)
    {
        UART_Send(
            "ESP32 -> STM32 : ACK,SENSOR_DATA\r\n"
        );


        UART_Send(
            "ACK RECEIVED\r\n"
        );


        SEGGER_SYSVIEW_PrintfHost(
            "ACK RECEIVED FROM ESP32"
        );
    }


    /* ====================================================================== */
    /*                         INVALID PACKET                                  */
    /* ====================================================================== */

    else if (strcmp(message, "INVALID_PACKET") == 0)
    {
        UART_Send(
            "ESP32 -> STM32 : INVALID_PACKET\r\n"
        );


        UART_Send(
            "ERROR: ESP32 REJECTED PACKET\r\n"
        );


        SEGGER_SYSVIEW_PrintfHost(
            "ESP32 REJECTED SENSOR PACKET"
        );
    }


    /* ====================================================================== */
    /*                         UNKNOWN MESSAGE                                 */
    /* ====================================================================== */

    else
    {
        UART_Send(
            "ESP32 -> STM32 : UNKNOWN MESSAGE\r\n"
        );


        UART_Send(
            message
        );


        UART_Send(
            "\r\n"
        );


        SEGGER_SYSVIEW_PrintfHost(
            "UNKNOWN ESP32 MESSAGE"
        );
    }
}


/* ========================================================================== */
/*                           LED TASK                                         */
/* ========================================================================== */

void StartDefaultTask(void *argument)
{
    UART_Send(
        "LED TASK: Started\r\n"
    );


    SEGGER_SYSVIEW_PrintfHost(
        "LED TASK STARTED"
    );


    for (;;)
    {
        HAL_GPIO_TogglePin(
            GPIOA,
            GPIO_PIN_5
        );


        SEGGER_SYSVIEW_PrintfHost(
            "LED TASK: Toggle"
        );


        osDelay(1000);
    }
}


/* ========================================================================== */
/*                           SENSOR TASK                                      */
/* ========================================================================== */

void StartSensorTask(void *argument)
{
    SensorData_t sensorData;


    UART_Send(
        "SENSOR TASK: Started\r\n"
    );


    SEGGER_SYSVIEW_PrintfHost(
        "SENSOR TASK STARTED"
    );


    for (;;)
    {
        /* ====================================================== */
        /* Simulated sensor values                               */
        /* ====================================================== */

        sensorData.temperature = 28;

        sensorData.humidity = 62;

        sensorData.status = 1;

        sensorData.error_count = 0;


        /* ====================================================== */
        /* Put data into queue                                    */
        /* ====================================================== */

        if (osMessageQueuePut(
                sensorQueueHandle,
                &sensorData,
                0,
                0) == osOK)
        {
            UART_Send(
                "SENSOR: Data placed into Queue\r\n"
            );


            SEGGER_SYSVIEW_PrintfHost(
                "SENSOR DATA -> QUEUE"
            );
        }
        else
        {
            UART_Send(
                "SENSOR ERROR: Queue Full\r\n"
            );


            SEGGER_SYSVIEW_PrintfHost(
                "SENSOR QUEUE ERROR"
            );
        }


        /* Sensor every 5 seconds */
        osDelay(5000);
    }
}


/* ========================================================================== */
/*                      COMMUNICATION TASK                                    */
/* ========================================================================== */

void StartCommunicationTask(void *argument)
{
    uint8_t rxByte;

    char rxBuffer[150];

    uint8_t rxIndex = 0;

    SensorData_t sensorData;

    char txBuffer[150];


    UART_Send(
        "COMMUNICATION TASK: Started\r\n"
    );


    SEGGER_SYSVIEW_PrintfHost(
        "COMMUNICATION TASK STARTED"
    );


    for (;;)
    {
        /* ====================================================== */
        /* Receive data from ESP32                               */
        /* ====================================================== */

        if (HAL_UART_Receive(
                &huart1,
                &rxByte,
                1,
                10) == HAL_OK)
        {
            if (rxByte == '\n')
            {
                rxBuffer[rxIndex] = '\0';


                if (rxIndex > 0)
                {
                    ProcessESP32Message(
                        rxBuffer
                    );
                }


                rxIndex = 0;
            }


            else if (rxByte == '\r')
            {
                /* Ignore CR */
            }


            else
            {
                if (rxIndex < sizeof(rxBuffer) - 1)
                {
                    rxBuffer[rxIndex++] =
                        rxByte;
                }
                else
                {
                    rxIndex = 0;

                    UART_Send(
                        "ERROR: RX BUFFER OVERFLOW\r\n"
                    );


                    SEGGER_SYSVIEW_PrintfHost(
                        "RX BUFFER OVERFLOW"
                    );
                }
            }
        }


        /* ====================================================== */
        /* Get sensor packet from queue                           */
        /* ====================================================== */

        if (osMessageQueueGet(
                sensorQueueHandle,
                &sensorData,
                NULL,
                0) == osOK)
        {
            /* ================================================== */
            /* Create packet                                     */
            /* ================================================== */

            snprintf(
                txBuffer,
                sizeof(txBuffer),
                "SENSOR_DATA,TEMP=%d,HUM=%d,STATUS=OK,ERROR=%d\r\n",
                sensorData.temperature,
                sensorData.humidity,
                sensorData.error_count
            );


            /* ================================================== */
            /* Send packet to ESP32                              */
            /* ================================================== */

            ESP_UART_Send(
                txBuffer
            );


            /* ================================================== */
            /* Debug output                                      */
            /* ================================================== */

            UART_Send(
                "STM32 -> ESP32 : "
            );


            UART_Send(
                txBuffer
            );


            UART_Send(
                "WAITING FOR ACK...\r\n"
            );


            SEGGER_SYSVIEW_PrintfHost(
                "SENSOR PACKET SENT - WAITING ACK"
            );
        }


        osDelay(10);
    }
}


/* ========================================================================== */
/*                           MONITOR TASK                                     */
/* ========================================================================== */

void StartMonitorTask(void *argument)
{
    UART_Send(
        "MONITOR TASK: Started\r\n"
    );


    SEGGER_SYSVIEW_PrintfHost(
        "MONITOR TASK STARTED"
    );


    for (;;)
    {
        UART_Send(
            "MONITOR: RTOS Gateway Running\r\n"
        );


        SEGGER_SYSVIEW_PrintfHost(
            "MONITOR: GATEWAY RUNNING"
        );


        osDelay(3000);
    }
}


/* ========================================================================== */
/*                         SYSTEM CLOCK                                      */
/* ========================================================================== */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


    __HAL_RCC_PWR_CLK_ENABLE();


    __HAL_PWR_VOLTAGESCALING_CONFIG(
        PWR_REGULATOR_VOLTAGE_SCALE2
    );


    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSI;


    RCC_OscInitStruct.HSIState =
        RCC_HSI_ON;


    RCC_OscInitStruct.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;


    RCC_OscInitStruct.PLL.PLLState =
        RCC_PLL_NONE;


    if (HAL_RCC_OscConfig(
            &RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }


    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;


    RCC_ClkInitStruct.SYSCLKSource =
        RCC_SYSCLKSOURCE_HSI;


    RCC_ClkInitStruct.AHBCLKDivider =
        RCC_SYSCLK_DIV1;


    RCC_ClkInitStruct.APB1CLKDivider =
        RCC_HCLK_DIV1;


    RCC_ClkInitStruct.APB2CLKDivider =
        RCC_HCLK_DIV1;


    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}


/* ========================================================================== */
/*                         USART1 INIT                                       */
/* ========================================================================== */

static void MX_USART1_UART_Init(void)
{
    huart1.Instance =
        USART1;


    huart1.Init.BaudRate =
        115200;


    huart1.Init.WordLength =
        UART_WORDLENGTH_8B;


    huart1.Init.StopBits =
        UART_STOPBITS_1;


    huart1.Init.Parity =
        UART_PARITY_NONE;


    huart1.Init.Mode =
        UART_MODE_TX_RX;


    huart1.Init.HwFlowCtl =
        UART_HWCONTROL_NONE;


    huart1.Init.OverSampling =
        UART_OVERSAMPLING_16;


    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}


/* ========================================================================== */
/*                         USART2 INIT                                       */
/* ========================================================================== */

static void MX_USART2_UART_Init(void)
{
    huart2.Instance =
        USART2;


    huart2.Init.BaudRate =
        115200;


    huart2.Init.WordLength =
        UART_WORDLENGTH_8B;


    huart2.Init.StopBits =
        UART_STOPBITS_1;


    huart2.Init.Parity =
        UART_PARITY_NONE;


    huart2.Init.Mode =
        UART_MODE_TX_RX;


    huart2.Init.HwFlowCtl =
        UART_HWCONTROL_NONE;


    huart2.Init.OverSampling =
        UART_OVERSAMPLING_16;


    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}


/* ========================================================================== */
/*                           GPIO INIT                                       */
/* ========================================================================== */

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};


    __HAL_RCC_GPIOA_CLK_ENABLE();


    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_5,
        GPIO_PIN_RESET
    );


    GPIO_InitStruct.Pin =
        GPIO_PIN_5;


    GPIO_InitStruct.Mode =
        GPIO_MODE_OUTPUT_PP;


    GPIO_InitStruct.Pull =
        GPIO_NOPULL;


    GPIO_InitStruct.Speed =
        GPIO_SPEED_FREQ_LOW;


    HAL_GPIO_Init(
        GPIOA,
        &GPIO_InitStruct
    );
}


/* ========================================================================== */
/*                         ERROR HANDLER                                     */
/* ========================================================================== */

void Error_Handler(void)
{
    __disable_irq();


    while (1)
    {
        HAL_GPIO_TogglePin(
            GPIOA,
            GPIO_PIN_5
        );


        HAL_Delay(200);
    }
}


#ifdef USE_FULL_ASSERT

void assert_failed(
    uint8_t *file,
    uint32_t line)
{
}

#endif
