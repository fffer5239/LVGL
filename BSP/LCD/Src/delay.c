/**
 * @file    delay.h
 * @brief   延时函数头文件
 * @author  Dr. GAO
 * @date    2025-10-07
 * @version V1.0
 * @website https://genbotter.taobao.com
 * @email   mailto:yanzenggao@163.com
 * @note    该文件适用于GenBotter Motor-1电机开发板, 且使用DWT实现微秒级延时, STM32F407IGT6
 */
#include "delay.h"


/**
  * @brief  Initialize DWT Cycle Counter for delay_us()
  * @note   Must be called after SystemClock_Config()
  */
void DWT_Init(void)
{
    if ((CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk) == 0U)
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/**
  * @brief  Microsecond delay using DWT cycle counter
  * @param  us: delay time in microseconds
  * @note   SystemCoreClock must be correctly configured
  */
void delay_us(uint32_t us)
{
    uint32_t startTick = DWT->CYCCNT;
    uint32_t delayTicks = us * (SystemCoreClock / 1000000U);

    while ((DWT->CYCCNT - startTick) < delayTicks)
    {
        __NOP(); // 可选：防止编译器过度优化
    }
}


/**
 * @brief     延时nms
 * @param     nms: 要延时的ms数 (0< nms <= (2^32 / fac_us / 1000))(fac_us一般等于系统主频, 自行套入计算)
 * @retval    无
 */
void delay_ms(uint16_t nms)
{
    
#if SYS_SUPPORT_OS  /* 如果需要支持OS, 则根据情况调用os延时以释放CPU */
    if (delay_osrunning && delay_osintnesting == 0)     /* 如果OS已经在跑了,并且不是在中断里面(中断里面不能任务调度) */
    {
        if (nms >= g_fac_ms)                            /* 延时的时间大于OS的最少时间周期 */
        {
            delay_ostimedly(nms / g_fac_ms);            /* OS延时 */
        }

        nms %= g_fac_ms;                                /* OS已经无法提供这么小的延时了,采用普通方式延时 */
    }
#endif

    delay_us((uint32_t)(nms * 1000));                   /* 普通方式延时 */
}