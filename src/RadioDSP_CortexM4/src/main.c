/**
  ******************************************************************************
  * @file    main.c
  * @author  Giuseppe Callipo - IK8YFW - ik8yfw@libero.it
  * @version V1.0.0
  * @date    28-12-2018
  * @brief   Main routine file
  *
  ******************************************************************************
  *
  *                     THE RADIO DSP v. 2 - PROJECT - CORTEX M4
  *
  * This project RadioDSP define a experimental open platform to build
  * Real Time filtering and audio digital signal elaboration from
  * a source audio signal in output form radio transceiver or receiver.
  * The RadioDSP firmware define some FIR filter and routines to perform
  * real time Noise Reduction based on various algorithm.
  * The target hardware platform is Stm32f407 - Arm Cortex-M4 based processor.
  *
  * The functions built in the RadioDSP are:
  *  ... TODO ...
  *
  * NOTE: this is an experimental project and the functions can be changed
  * without any advise.
  *
  *
  * The RadioDSP use some parts of the ARM Cortex Microcontroller Software
  * Interface Standard (CMSIS).
  * The RadioDSP use some parts of STM32F103 examples free and available in
  * the opensource community.
  *
  * The RadioDSP openSource software is released under the license:
  *              Common Creative - Attribution 3.0
  ******************************************************************************/

#include "stm32f4xx_conf.h"
#include "io_audio.h"
#include "io_display.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include <math.h>
#include <stdio.h>
#include <complex.h>

#include "defines.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_spi.h"
#include "tm_stm32f4_ili9341.h"
#include "tm_stm32f4_fonts.h"

#include "tm_stm32f4_rotary_encoder.h"

#include "morseDecode.h"
#include "filter_noise_reduction.h"
#include "filter_audio_segnal.h"

#include "general.h"

//#include "tm_stm32f4_timer_properties.h"

/* Rotary encoder data */
static TM_RE_t RE1_Data;

float32_t FFTBufferMagPlot[BLOCK_SIZE];
float32_t FFTBufferMagPlot_OLD[BLOCK_SIZE];
float32_t FLTBufferTmp[BLOCK_SIZE*2];

unsigned short iCurBuffIdx =0;
int            ict=0;
int            iLastict=0;
uint8_t        switchPush=1;
uint8_t        menu_voice=0;
uint8_t        iCmdDnr=0;
uint8_t        iCmdFlt=0;
uint8_t        iCmdScp=0;
uint8_t        iCmdFnc=0;
uint8_t        rowCt=0;
uint8_t        slowCt =4;

arm_fir_instance_f32 S,S2, S3,S4;
volatile uint32_t ticks;

/* return the system clock as milliseconds */
uint32_t millis(void) {
  return ticks;
}

/*- Time milliseconds timer handler */
void TIM5_IRQHandler()
{
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
        ticks++;
    }
}

int main(void)
{
    /*- Initialize Display*/
    initializeDisplay();

    /* Call FIR init function to initialize the instance structure. */
    arm_fir_init_f32(&S,  NUM_TAPS,   (float32_t *)&firCoeffs32[0],   &firStateF32[0],   BLOCK_SIZE);
    arm_fir_init_f32(&S2, NUM_TAPS_2, (float32_t *)&firCoeffs32_2[0], &firStateF32_2[0], BLOCK_SIZE);
    arm_fir_init_f32(&S3, NUM_TAPS_3, (float32_t *)&firCoeffs32_3[0], &firStateF32_3[0], BLOCK_SIZE);
    arm_fir_init_f32(&S4, NUM_TAPS_4, (float32_t *)&firCoeffs32_4[0], &firStateF32_4[0], BLOCK_SIZE);

    arm_lms_init_f32	(&Sl_LMS,NUMTAPS_LMS,pCoeffs,lmsStateF32,MU,BLOCK_SIZE);

    /*- Initialize ADC and DAC */
    ADC1_CH6_DMA_Config(BLOCK_SIZE);
	DAC1_Config(BLOCK_SIZE);

	/*- Initialize push button menu */
    GPIO_Switch_Setup();

    /* Initialize Rotary encoder 1, pin A = PD0, pin B = PD1 */
	TM_RE_Init(&RE1_Data, GPIOB, GPIO_PIN_6, GPIOB, GPIO_PIN_7);
	/* Set rotation mode for rotary 1 */
	TM_RE_SetMode(&RE1_Data, TM_RE_Mode_One);

	/*- Start ADC master timer */
	TIM2_Config();

	/*- Start services timer */
    TIM3_Config();

    TIM5_Config();

    /*- wait forever ...*/
	while(1)
	{
	    // Infinite loop ...
	}
}


/* TM EXTI Handler for all EXTI lines */
void TM_EXTI_Handler(uint16_t GPIO_Pin) {
	/* Check RE pin 1 */
	if (GPIO_Pin == RE1_Data.GPIO_PIN_A) {
		/* Process data */
		TM_RE_Process(&RE1_Data);
	}
}

/*- IRQ for incoming samples */
void DMA2_Stream0_IRQHandler()
{

	iCurBuffIdx=!iCurBuffIdx;

    /*- converter ADC to float */
	arm_q15_to_float(&ADC_ConvertedValue[iCurBuffIdx][0],&BufferIn[0],BLOCK_SIZE);

     if (iCmdFlt==3){
      //CW -   NARROW
       arm_fir_f32(&S2,(float *)&BufferIn[0],(float *)&Buffer1[0],BLOCK_SIZE);
     }else if (iCmdFlt==2){
	   //SSB - MEDIUM
        arm_fir_f32(&S,(float *)&BufferIn[0],(float *)&Buffer1[0],BLOCK_SIZE);
     }else if (iCmdFlt==1){
	   //AM -  WIDE
        arm_fir_f32(&S4,(float *)&BufferIn[0],(float *)&Buffer1[0],BLOCK_SIZE);
     }
     else if (iCmdFlt==0){
       //AM -  PASSTROUGH - OFF
       arm_fir_f32(&S3,(float *)&BufferIn[0],(float *)&Buffer1[0],BLOCK_SIZE);
     }

    /*- noise reduction and panoramic spectrum */
    processing_noise_reduction(&Buffer1[0], &BufferOut[0]);

    /*- back converter from float to dac */
    arm_float_to_q15(&BufferOut[0],&OutputBuffer[iCurBuffIdx][0],BLOCK_SIZE);

  /*- cleat irq */
    DMA_ClearFlag(DMA2_Stream0, DMA_IT_TC);

}

/*- Scope spectrum plot routine 1 */
void spectrumScope(){
    float32_t actualValue;
    uint16_t  actualHeight;

  /* Display data on LCD */
		for (int k = 1; k < FFT_SIZE/2; k++) {

		    actualValue =(float32_t)FFTBufferMagPlot[(uint16_t)k];
            actualHeight = (uint16_t)((float32_t)actualValue / (float32_t)maxValue * (float32_t)FFT_BAR_MAX_HEIGHT);

		    /* Draw FFT results */
			DrawSpectrumPoints(20+2*k,
					220,FFT_BAR_MAX_HEIGHT,FFTBufferMagPlot_OLD[(uint16_t)k],
                    ILI9341_COLOR_BLACK);

			/* Draw FFT results */
			DrawSpectrumPoints(20+ 2*k,
                    220,FFT_BAR_MAX_HEIGHT, actualHeight,
					ILI9341_COLOR_YELLOW);

			  FFTBufferMagPlot_OLD[k] = actualHeight;
		   }

		maxValue_OLD = maxValue;
};

/*- Scope waterfall plot routine 1 */
void waterfallScope(){

  /* Display data on LCD half size */
  for (int k = 1; k < FFT_SIZE/4; k++) {

			/* Draw FFT results */
			DrawWaterfallPoints(20+ 4*k,
					FFT_BAR_MAX_HEIGHT,
					(uint16_t)maxValue,
					(float32_t)FFTBufferMagPlot[(uint16_t)k],
                    rowCt
			);
			DrawWaterfallPoints(22+ 4*k,
					FFT_BAR_MAX_HEIGHT,
					(uint16_t)maxValue,
					(float32_t)FFTBufferMagPlot[(uint16_t)k],
                    rowCt
			);

        }

  rowCt=(rowCt<MAX_WF_ROWS)?(rowCt+1):0;
};

/*- IRQ for services: display and controls */
 void TIM3_IRQHandler()
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

        /*- Lock the semaphore for plotting ... */
        semaphorePlot = 1;

        /* Display data on LCD */
        if (iCmdScp==0){
            spectrumScope();
        }else if (iCmdScp==1){
            waterfallScope();
        }

        /*- UnLock the semaphore after plotting ... */
		semaphorePlot = 0;

		if(slowCt>0){
		    slowCt--;
            return;
		}
        slowCt = 2;

       /* Get new rotation */
		TM_RE_Get(&RE1_Data);
		ict = RE1_Data.Absolute;

		/*- Read switch */
        switchPush = GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_0);
        if (switchPush==0) decodeMenuVoice();
        if (ict!=iLastict) decodeMenuValue();
    }
}

/**************************************END OF FILE****/

