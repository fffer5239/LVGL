/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "lcd.h"
#include "key_led.h"
#include "key.h"
#include "touch.h"
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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief   清空屏幕并在右上角显示"RST"
 * @param   无
 * @retval  无
 */
static void load_draw_dialog(void)
{
    lcd_clear(WHITE);                                                /* 清屏 */
    lcd_show_string(lcddev.width - 24, 0, 200, 16, 16, "RST", BLUE); /* 显示清屏区域 */
}

/**
 * @brief   画粗线
 * @param   x1: 起点X坐标
 * @param   y1: 起点Y坐标
 * @param   x2: 终点X坐标
 * @param   y2: 终点Y坐标
 * @param   size: 线条粗细程度
 * @param   color: 线的颜色
 * @retval  无
 */
static void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color)
{
    uint16_t t;
    int xerr = 0;
    int yerr = 0;
    int delta_x;
    int delta_y;
    int distance;
    int incx, incy, row, col;
    
    if ((x1 < size) || (x2 < size) || (y1 < size) || (y2 < size))
    {
        return;
    }
    
    delta_x = x2 - x1;                          /* 计算坐标增量 */
    delta_y = y2 - y1;
    row = x1;
    col = y1;
    if (delta_x > 0)
    {
        incx = 1;                               /* 设置单步方向 */
    }
    else if (delta_x == 0)
    {
        incx = 0;                               /* 垂直线 */
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    
    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;                               /* 水平线 */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    
    if (delta_x > delta_y)
    {
        distance = delta_x;                     /* 选取基本增量坐标轴 */
    }
    else
    {
        distance = delta_y;
    }
    
    for (t=0; t<=(distance + 1); t++)           /* 画线输出 */
    {
        lcd_fill_circle(row, col, size, color); /* 画点 */
        xerr += delta_x;
        yerr += delta_y;
        
        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }
        
        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief   电阻触摸屏测试
 * @param   无
 * @retval  无
 */
void rtp_test(void)
{
    uint8_t key;
    uint8_t i = 0;
    
    while (1)
    {
        key = key_scan(0);
        tp_dev.scan(0);
        
        if (tp_dev.sta & TP_PRES_DOWN)                                          /* 触摸屏被按下 */
        {
            if ((tp_dev.x[0] < lcddev.width) && (tp_dev.y[0] < lcddev.height))
            {
                if ((tp_dev.x[0] > (lcddev.width - 24)) && (tp_dev.y[0] < 16))
                {
                    load_draw_dialog();                                         /* 清除 */
                }
                else
                {
                    tp_draw_big_point(tp_dev.x[0], tp_dev.y[0], RED);           /* 画点 */
                }
            }
        }
        else
        {
            delay_ms(10);                                                       /* 没有按键按下的时候 */
        }
        
        if (key == KEY0_PRES)                                                   /* KEY0按下，则执行校准程序 */
        {
            printf("KEY0_PRES\n");
            tp_adjust();                                                        /* 屏幕校准 */
            tp_save_adjust_data();
            load_draw_dialog();
        }
        
        i++;
        if ((i % 20) == 0)
        {
            // LED2_TOGGLE();
             HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
        }
    }
}

/* 10个触控点的颜色（电容触摸屏用） */
static const uint16_t POINT_COLOR_TBL[10] = {RED, GREEN, BLUE, BROWN, YELLOW,
                                             MAGENTA, CYAN, LIGHTBLUE, BRRED, GRAY};
uint32_t timer_cnt = 0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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
  MX_FSMC_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  printf("Hello World!\n");
  DWT_Init(); // 初始化DWT
//   Key_Init();
// //  Led_Init();
  lcd_init();
  key_init(); 

    tp_dev.init();                      /* 初始化触摸屏 */

    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "TOUCH TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);

    /* 电阻屏显示触摸校准提示 */
    // if ((tp_dev.touchtype & 0x80) == 0)
    // {
    //     lcd_show_string(30, 110, 200, 16, 16, "Press KEY0 to Adjust", RED);
    // }
    // delay_ms(1500);
    // load_draw_dialog();
    
    // if ((tp_dev.touchtype & 0x80) == 0)
    // {
    //     /* 电阻屏测试 */
    //     rtp_test();
    // }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    // HAL_Delay(1000);
    if (timer_cnt % 1000 == 0)
    {
        // printf("timer_cnt = %d\r\n", timer_cnt);
       timer_cnt = 0;
       printf("timer_int\r\n");
    }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
//		HAL_GPIO_WritePin(GPIOA, LED2_Pin|LED3_Pin, GPIO_PIN_RESET);
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
