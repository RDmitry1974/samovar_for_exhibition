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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stm32l0xx_hal_conf.h"
#include <string.h>
#include "Tled_color.hpp"
#include "Tcoals.hpp"
#include "Tbubble.hpp"
#include "Twater_obj.hpp"
#include "stm32l0xx_ll_gpio.h"
      
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
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_DAC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


// ----------------------------------------------------------------------------
const unsigned short CUP_LED_PER_LINE_TBL[] = {0, 5, 11, 17};    //количество СИД в строчках изображения чашки (нарасающим итогом)
const unsigned short SAMOVAR_LED_PER_LINE_TBL[] = {5, 9, 13, 15, 15, 17, 17, 19, 19, 19, 19, 17, 17, 15, 15, 13, 9};    //количество СИД в строчках изображения самовара
const unsigned short SAMOVAR_LED_VS_NUMBER_LINE_TBL[] = {0, 5, 14, 27, 42, 57, 74, 91, 110, 129, 148, 167, 184, 201, 216, 231, 244, 253};    //количество СИД самовара в зависимости от номера строки 

#ifdef IZVRAT_MIRROR_LEDS
// Облажался с ключем на светодиодах. Исправил перемычками на плате. Придется править прошивкой.
Tcontainer samovar_led = Tcontainer(0, 253, SAMOVAR_LED_LINE, V_MAX_SAMOVAR, SAMOVAR_LED_VS_NUMBER_LINE_TBL, true);     // СИД. Самовар
Tcontainer cup_led = Tcontainer(253, 17, CUP_LED_LINE, V_MAX_CUP, CUP_LED_PER_LINE_TBL, false);                    // СИД. Чашка
Tstreamlet out_streamlet = Tstreamlet(270, 15);     // СИД. Выходной поток
// СИД. Угли идут статическим классом
Tstreamlet input_streamlet = Tstreamlet(292, 6);    // СИД. Входной поток
#else

Tstreamlet input_streamlet = Tstreamlet(0, 6);      // СИД. Входной поток
// СИД. Угли идут статическим классом
Tstreamlet out_streamlet = Tstreamlet(13, 15);      // СИД. Выходной поток
Tcup_led cup_led = Tcup_led(28, 17);                // СИД. Чашка
Tsamovar_led samovar_led = Tsamovar_led(45, 253);   // СИД. Самовар
#endif



const int Twater_obj :: T_MIN = 20.0;               //начальная температура жидкости. Она же для поступающей жидкости
const int Twater_obj :: T_MAX = 100.0;              //начальная температура жидкости. Она же для поступающей жидкости
const float Twater_obj :: C_WATER = 20.0;           //удельная теплоемкость жидкости
const float Twater_obj :: DELTA_TEMP = 0.1;         //скорость остывания жидкости( градусов на квант времени)
Twater_obj samovar = Twater_obj(K_LOST_SAMOVAR, V_MAX_SAMOVAR, V_INP_SAMOVAR, V_OUT_SAMOVAR);
Twater_obj cup =     Twater_obj(K_LOST_CUP, V_MAX_CUP, V_INP_CUP, V_OUT_CUP);
bool is_input_valve_open = false;                   // открыт ли входной клапан
bool is_output_valve_open = false;                  // открыт ли выходной клапан (нажата ли кнопка "Налить")
bool is_cup_sink = false;                           // нажата ли кнопка "Испить"
float q_samovar_heating = 0;                        // кол. теплоты подводимое к самовару

/*------------------------------------------------------------------------------
  Ожидание наступления времени
 ------------------------------------------------------------------------------*/
unsigned int my_delay_until_ctr;
void my_delay_until(void)
{
    while(1)
    {
        if((HAL_GetTick() - my_delay_until_ctr) >= MAIN_CYCLE_TIME)
        {
            my_delay_until_ctr = HAL_GetTick();
            return;
        }
    }
}
/*------------------------------------------------------------------------------
  Timer2 генерирует импульсы на светодиодную полосу
  Канал 3 таймера используется в режиме Compare с загрузкой по DMA регистра CCR3 для формирования битовых сигналов
 ------------------------------------------------------------------------------*/
void Timer2_init(void)
{
    TIM_TypeDef *tim = TIM2;
    RCC_TypeDef *rcc = RCC;

    rcc->APB1RSTR |= BIT(0);    // Сброс таймера 2
    rcc->APB1RSTR &= ~BIT(0);   
    rcc->APB1ENR |= BIT(0);     // Разрешаем тактирование таймера 2
    tim->CR1 = BIT(7);          // ARPE: Auto-reload preload enable | 1: TIMx_ARR register is buffered.
    tim->CR2 = 0;               
    tim->PSC = 0;               // Предделитель генерирует частоту 16 МГц
    tim->ARR = 41 - 1;          // Перегрузка таймера каждые 1.25 мкс
    tim->CCMR1 = 0
 //              + LSHIFT(6, 4) // OC1M: Output compare 1 mode | 110: PWM mode 1 - In upcounting, channel 1 is active as long as TIMx_CNT<TIMx_CCR1 else inactive.
               + LSHIFT(7, 4) // OC1M: Output compare 1 mode | 111: PWM mode 2 - In upcounting, channel 1 is inactive as long as TIMx_CNT<TIMx_CCR1 else inactive.
               + LSHIFT(1, 3) // OC1PE: Output compare 1 preload enable
               + LSHIFT(1, 2) // OC1FE: Output compare 1 fast enable | 00: The minimum delay to activate CC1 output when an edge occurs on the trigger input is 5 clock cycles
               + LSHIFT(0, 0) // CC1S: Capture/Compare 1 selection | 00: CC1 channel is configured as output
    ; 
    tim->CNT = 0;
    tim->CCR1 = 0;
    tim->DIER = BIT(9);        // Bit 9 CC1DE: Capture/Compare 1 DMA request enable. Разрешаем запросы DMA
    tim->CR1 |= BIT(0);         // Запускаем таймер
    tim->CCER = BIT(0) | BIT(1);         // Разрешаем работы выхода, чтобы возникали сигналы для DMA
}

/*------------------------------------------------------------------------------
  Инициализация канала 5 DMA1 Stream 7
  Используется для пересылки шаблоной битов потока управления светодиодной лентой на WS2812B в таймер TMR2 работающий в режиме генерации PWM 
 ------------------------------------------------------------------------------*/
void DMA1_Stream7_Mem_to_TMR2_init(void)
{
    DMA_Channel_TypeDef *dma_ch = DMA1_Channel5;
    RCC_TypeDef *rcc = RCC;
    
    rcc->AHBENR |= BIT(0);               // Разрешаем DMA1
    
    dma_ch->CCR = 0;    // Выключаем стрим
    dma_ch->CPAR = (unsigned int)&(TIM2->CCR1);  // Назначаем адрес регистра данных 
    dma_ch->CMAR = (unsigned long)&DMA_buf;
    dma_ch->CNDTR = (LEDS_NUM + 2) * COLRS * 8;
    dma_ch->CCR =
        LSHIFT(0, 14) + // MEM2MEM:Memory to memory mode | 0: Memory to memory mode disabled | 0: Memory to memory mode disabled
        LSHIFT(3, 12) + // PL[1:0]:Channel priority level | 11: Very high                  
        LSHIFT(1, 10) + // MSIZE[1:0]:Memory size | 01: 16-bit. Memory data size
        LSHIFT(1, 8) +  // PSIZE[1:0]:Peripheral size | 01: 16-bit. Peripheral data size
                        //               LSHIFT(0, 10) + // MSIZE[1:0]:Memory size | 00: 8-bit. Memory data size
                        //               LSHIFT(0, 8) +  // PSIZE[1:0]:Peripheral size | 00: 8-bit. Peripheral data size
        LSHIFT(1, 7) +  // MINC:Memory increment mode | 1: Memory increment mode enabled
        LSHIFT(0, 6) +  // PINC:Peripheral increment mode | 0: Peripheral increment mode disabled
                                //             LSHIFT(1, 5) +  // CIRC:Circular mode | 1: Circular mode disabled
        LSHIFT(0, 5) +  // CIRC:Circular mode | 0: Circular mode disabled
        LSHIFT(1, 4) +  // DIR:Data transfer direction | 1: Read from memory
        LSHIFT(0, 3) +  // TEIE:Transfer error interrupt enable | 0: interrupt disabled
        LSHIFT(0, 2) +  // HTIE:Half transfer interrupt enable | 0 : interrupt disabled
        LSHIFT(0, 1) +  // TCIE:Transfer complete interrupt enable | 0 : interrupt disabled
        LSHIFT(0, 0) +  // EN: Channel enable | 1: Channel enabled
        0;
    DMA1_CSELR->CSELR = LSHIFT(8, 16);  // 1000: DMA channel 5 remapped to TIM2_CH1
    dma_ch->CCR |= BIT(0); //  1: Stream enabled
}

void DMA1_Stream7_Mem_to_TMR2_restart(void)
{
    TIM_TypeDef *tim = TIM2;
    DMA_Channel_TypeDef *dma_ch = DMA1_Channel5;
    tim->CR1 &= ~BIT(0);                            // Стопим таймер     
    dma_ch->CCR &= ~BIT(0);                         // Stream disabled
    dma_ch->CNDTR = (LEDS_NUM + 2) * COLRS * 8;
    dma_ch->CCR |= BIT(0);                          // Stream enabled
    tim->CR1 |= BIT(0);                             // Запускаем таймер
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    int led_redraw_ctr;
    int adc_data;
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
  MX_ADC_Init();
  MX_DAC_Init();
  /* USER CODE BEGIN 2 */
    LL_ADC_Enable(ADC1);
    LL_ADC_REG_StartConversion(ADC1);
    LL_DAC_Enable(DAC, LL_DAC_CHANNEL_1);
    LL_DAC_ConvertData12RightAligned(DAC, LL_DAC_CHANNEL_1, 0);
    LL_DAC_Enable(DAC, LL_DAC_CHANNEL_2);
    LL_DAC_ConvertData12RightAligned(DAC, LL_DAC_CHANNEL_2, 0);
    Timer2_init();
    DMA1_Stream7_Mem_to_TMR2_init();
    Tled_color :: init_leds();
    Tcoals :: init();
    input_streamlet.init();
    out_streamlet.init();
    cup_led.init();
    samovar_led.init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    led_redraw_ctr = 10;
    adc_data = 0;
//    samovar.v = V_MAX_SAMOVAR;  //фор тестовых прогонов
    while (1)
    {
        my_delay_until();                                                       //ожидание начала следующего цикла
        //эта часть кода выполняется раз в MAIN_CYCLE_TIME
        while (!LL_ADC_IsActiveFlag_EOS(ADC1))                                  //дождаться окончания преобразования АЦП
            __no_operation();
        LL_ADC_ClearFlag_EOS(ADC1);                                             //сбросит нужно руками флаг
        adc_data = LL_ADC_REG_ReadConversionData32(ADC1);                       //считать данные из АЦП
        LL_ADC_REG_StartConversion(ADC1);                                       //Перезапустить новый опрос АЦП
        
        is_input_valve_open = !LL_GPIO_IsInputPinSet (GPIOD, LL_GPIO_PIN_15);
        is_output_valve_open = !LL_GPIO_IsInputPinSet (GPIOD, LL_GPIO_PIN_14);
        is_output_valve_open = is_output_valve_open && ((cup.v + 0.01) < V_MAX_CUP);
        is_cup_sink = !LL_GPIO_IsInputPinSet (GPIOD, LL_GPIO_PIN_13);
        if(adc_data <= ADC_4mA_VALUE)
            q_samovar_heating = 0;
        else
            q_samovar_heating = (float)(adc_data - ADC_4mA_VALUE) * SAMOVAR_MAX_HEATING / (ADC_20mA_VALUE - ADC_4mA_VALUE);
        input_streamlet.cycle(is_input_valve_open, Twater_obj :: T_MIN);
        Tcoals :: cycle(adc_data);
        samovar.cycle(is_input_valve_open, is_output_valve_open, q_samovar_heating, Twater_obj :: T_MIN);
        samovar_led.cycle(samovar.v, samovar.temp);
        
        int dac_val = (unsigned int)(DAC_4mA_VALUE + samovar.v * (DAC_20mA_VALUE - DAC_4mA_VALUE) / V_MAX_SAMOVAR);
        LL_DAC_ConvertData12RightAligned(DAC, LL_DAC_CHANNEL_2, dac_val);
        dac_val = (unsigned int)(DAC_4mA_VALUE + (samovar.temp - Twater_obj::T_MIN) * (DAC_20mA_VALUE - DAC_4mA_VALUE) / (Twater_obj::T_MAX - Twater_obj::T_MIN));
        LL_DAC_ConvertData12RightAligned(DAC, LL_DAC_CHANNEL_1, dac_val);
        
        if(samovar.v < 0.0001)
            is_output_valve_open = false;                                       //коли в самоваре пусто - в чашку не лить!
        out_streamlet.cycle(is_output_valve_open, samovar.temp);
        cup.cycle(is_output_valve_open, is_cup_sink, 0, samovar.temp);
        cup_led.cycle(cup.v, cup.temp);
        led_redraw_ctr -= MAIN_CYCLE_TIME;
        if(led_redraw_ctr <= 0)
        {
            led_redraw_ctr = 30;       //10mS
            DMA1_Stream7_Mem_to_TMR2_restart();                                 //обновить светодиоды
        }
        
    /* USER CODE END WHILE */

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{
  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
  LL_ADC_InitTypeDef ADC_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
  
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**ADC GPIO Configuration  
  PA0   ------> ADC_IN0 
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /** Configure Regular Channel 
  */
  LL_ADC_REG_SetSequencerChAdd(ADC1, LL_ADC_CHANNEL_0);
  /** Common config 
  */
  ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
  ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
  ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
  ADC_REG_InitStruct.Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN;
  LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
  LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_1CYCLE_5);
  LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);
  LL_ADC_REG_SetSequencerScanDirection(ADC1, LL_ADC_REG_SEQ_SCAN_DIR_FORWARD);
  LL_ADC_SetCommonFrequencyMode(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_FREQ_MODE_HIGH);
  LL_ADC_DisableIT_EOC(ADC1);
  LL_ADC_DisableIT_EOS(ADC1);
  LL_ADC_EnableInternalRegulator(ADC1);
  ADC_InitStruct.Clock = LL_ADC_CLOCK_SYNC_PCLK_DIV2;
  ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
  ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
  ADC_InitStruct.LowPowerMode = LL_ADC_LP_MODE_NONE;
  LL_ADC_Init(ADC1, &ADC_InitStruct);
  /* USER CODE BEGIN ADC_Init 2 */
  //  ADC1_COMMON->CCR |= ADC_CR_ADVREGEN;    //внутренняя опора
  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{
  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  LL_DAC_InitTypeDef DAC_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);
  
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**DAC GPIO Configuration  
  PA4   ------> DAC_OUT1
  PA5   ------> DAC_OUT2 
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_4;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */
  /** DAC channel OUT1 config 
  */
  DAC_InitStruct.TriggerSource = LL_DAC_TRIG_SOFTWARE;
  DAC_InitStruct.WaveAutoGeneration = LL_DAC_WAVE_AUTO_GENERATION_NONE;
  DAC_InitStruct.OutputBuffer = LL_DAC_OUTPUT_BUFFER_ENABLE;
  LL_DAC_Init(DAC, LL_DAC_CHANNEL_1, &DAC_InitStruct);
  /** DAC channel OUT2 config 
  */
  LL_DAC_Init(DAC, LL_DAC_CHANNEL_2, &DAC_InitStruct);
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOH);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOE);
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOD);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

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
