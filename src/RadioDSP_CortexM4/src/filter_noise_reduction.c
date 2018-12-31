/**
  ******************************************************************************
  * @file    filter_noise_reduction.h
  * @author  Giuseppe Callipo - IK8YFW - ik8yfw@libero.it
  * @version V2.0.0
  * @date    25-09-2018
  * @brief   IO Routine
  *
  *
  * NOTE: This file is part of RadioDSP project.
  *       See main.c file for additional project informations.
  * Platform: CortexM4
  ******************************************************************************/
#include "stm32f4xx_conf.h"
#include "io_audio.h"
#include "arm_math.h"
#include "arm_const_structs.h"
#include <math.h>

#include "tm_stm32f4_ili9341.h"
#include "tm_stm32f4_fonts.h"
#include <stdio.h>
#include <complex.h>
#include "filter_noise_reduction.h"
#include "filter_audio_segnal.h"

/*- working variables */
int energyChkPeriod_idx = 0;
float TH_VALUE          = 0.0;
float POW_VALUE         = 0.0;
float POW_VALUE_min     = 0.0;
float POW_VALUE_max     = 0.0;
float level             = 0;
int   lasCmdDnr         =0;

void processing_noise_reduction(float* bufferIn, float* bufferOut2){

    int j =0;
    for (int i =0; i< BLOCK_SIZE; i++)
	{
       FFTBufferOut[j] =  bufferIn[i];
       j++;
       FFTBufferOut[j]=0;
       j++;
	}

    /* Process the data through the CFFT/CIFFT module */
    arm_cfft_f32(&arm_cfft_sR_f32_len256, &FFTBufferOut[0], 0, 1);

    // Experimental: mult. by the fast convolution mask
    //arm_cmplx_mult_cmplx_f32(&FFTBufferTmp[0], &FLTBufferTmp[0], &FFTBufferOut[0], FFT_SIZE);

    /* Process the data through the Complex Magniture Module for calculating the magnitude at each bin */
    arm_cmplx_mag_f32(&FFTBufferOut[0], &FFTBufferMag[0], FFT_SIZE);

    /* Evaluate Noise */
    energyChkPeriod_idx++;
    if (energyChkPeriod_idx>=200){
        POW_VALUE_max = 0;
        energyChkPeriod_idx=0;
    }
    POW_VALUE=0;
        float TH_VALUE_new = 0.0;
        int low = 2; int high =(FFT_SIZE/4);
        for (j =low; j< high; j++)
        {
           TH_VALUE_new += FFTBufferMag[j];
        }

        // Only for internal voice bins ...
        for (j =2; j< 30; j++)
        {     // Evaluate power spectrum ...
              POW_VALUE += FFTBufferMag[j]*FFTBufferMag[j];
        }
        // ... and update the current max values ...
        if (POW_VALUE>POW_VALUE_max){
               POW_VALUE_max = POW_VALUE;
        }

        //*******ELEBORATE THE ENCODER ************
        if (iCmdDnr != lasCmdDnr){
            if (iCmdDnr > lasCmdDnr){
                 level = level + 0.5;
            }else
            {
                level = level - 0.5;
                if (level <= 0) level = 0;
            }

            lasCmdDnr = iCmdDnr;
        }

        //*******************************

    float mediumValue = (TH_VALUE_new/(high-low)); // MEDIUM
    float TH_VALUE_low = (0.8+(level/10)) * mediumValue;
    TH_VALUE = level*mediumValue*(1.0-(POW_VALUE_max*10));
    if (TH_VALUE<TH_VALUE_low){
            TH_VALUE = TH_VALUE_low;
    }

     /*- DNR disabled if level is 0 */
     if (level == 0){
        TH_VALUE = 0;
     }

    /* Preserve values for plotting and apply subtraction */
    for (j =0; j< FFT_SIZE*2; j++)
	{
	     /*- Copy values for plotting - before NR for spectrum */
	     if (iCmdScp==0 && semaphorePlot == 0){
            if (j<FFT_SIZE/2){
                FFTBufferMagPlot[j] = FFTBufferMag[j]*50;
            }
	     }

        /*- Subtract threshold noise */
        if (FFTBufferMag[j]<=TH_VALUE){
            FFTBufferMag[j] = 0;
	    }else{
            FFTBufferMag[j] =  FFTBufferMag[j]- TH_VALUE;
	    }

         /*- Copy values for plotting - after NR for spectrum */
	     if (iCmdScp==1 && semaphorePlot == 0){
            if (j<FFT_SIZE/2){
                FFTBufferMagPlot[j] = FFTBufferMag[j]*50;
            }
	     }

	}

	/* Rebuild & discard the fft complex signal from subctracted magnitude & original signal phase */
	int k =1;
	int s= 0;
	for (j =0; j< FFT_SIZE; j++)
	{

      float32_t r1 = FFTBufferOut[s]; float32_t i1 = FFTBufferOut[k];
      complex float z= r1 + i1*I;
      float32_t phi =  cargf(z);
      float32_t r2 = FFTBufferMag[j]*arm_cos_f32(phi);
      float32_t i2 = FFTBufferMag[j]*arm_sin_f32(phi);

      FFTBufferOut[s] = r2;
      FFTBufferOut[k] = i2;
      s=s+2;
      k=s+1;
     }

	//***************************************************************************************************
   // Plot if do not expire irq
   if (semaphorePlot == 0){
 	  FFTBufferMagPlot[0] = 0; // CUT DC Value ...
      /* Calculates maxValue and returns corresponding value */
	  arm_max_f32(&FFTBufferMagPlot[0], FFT_SIZE/2, &maxValue, &maxIndex);
    }

    // Compute the inverse FFT ...
    arm_cfft_f32(&arm_cfft_sR_f32_len256, &FFTBufferOut[0],1,1);

    j =0;
    for (int i =0; i< BLOCK_SIZE; i++)
	{
       bufferOut2[i] =  FFTBufferOut[j];
       j++; j++;
	}

	 /*- Experimental Smooting output */
    // smootingFilter(&smotStatusBuffer[0], &bufferPreOut[0], &bufferOut2[0], 3,BLOCK_SIZE);

}

/**************************************END OF FILE****/

