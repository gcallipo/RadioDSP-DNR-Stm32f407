/**
  ******************************************************************************
  * @file    io_audio.h
  * @author  Giuseppe Callipo - IK8YFW - ik8yfw@libero.it
  * @version V2.0.0
  * @date    25-09-2018
  * @brief   IO Routine
  *
  *
  * NOTE: This file is part of RadioDSP project.
  *       See main.c file for additional project informations.
  ******************************************************************************/

#ifndef IO_AUDIO_H_INCLUDED
#define IO_AUDIO_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
#include "arm_math.h"
#include "general.h"

/********************************************************************
 *
 *                   DMA ADC BASIC DEFINE
 *
 ********************************************************************/
#define ADC1_DR_Address          ((u32)0x4001204C)
#define DAC_DHR12R1_ADDRESS      0x40007408

/********************************************************************
 *
 *                   VARIABLES
 *
 ********************************************************************/

extern int ict;
extern uint8_t iCmdDnr;
extern uint8_t iCmdScp;
static int semaphorePlot = 0;

float32_t maxValue;				            /* Max FFT value is stored here */
uint32_t maxIndex;				            /* Index in Output array where max value is */

float32_t maxValue_OLD;				            /* Max FFT value is stored here */
uint32_t  maxIndex_OLD;				            /* Index in Output array where max value is */

extern float32_t FFTBufferMagPlot[BLOCK_SIZE];
extern float32_t FFTBufferMagPlot_OLD[BLOCK_SIZE];
extern float32_t FLTBufferTmp[BLOCK_SIZE*2];

/********************************************************************
 *
 *                   DISPLAY FNC
 *
 ********************************************************************/
void GPIO_Configuration(void);
void Delay(__IO uint32_t nCount);

void ADC_Configuration(void);
void ADC1_CH6_DMA_Config(uint32_t);
void DAC1_Config(uint32_t);
void TIM2_Config(void);

/* Private variables ---------------------------------------------------------*/
q15_t ADC_ConvertedValue[2][BLOCK_SIZE];
q15_t OutputBuffer[2][BLOCK_SIZE];

#ifdef __cplusplus
}
#endif


#endif /* IO_AUDIO_H_INCLUDED */

/**************************************END OF FILE****/
