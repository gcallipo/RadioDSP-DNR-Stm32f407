/**
  ******************************************************************************
  * @file    filter_audio_segnal.h
  * @author  Giuseppe Callipo - IK8YFW - ik8yfw@libero.it
  * @version V1.0.0
  * @date    22-04-2018
  * @brief   Noise Reduction routines
  *
  *
  * NOTE: This file is part of RadioDSP project.
  *       See main.c file for additional project informations.
  ******************************************************************************/
#ifndef FILTER_AUDIO_SEGNAL_H_INCLUDED
#define FILTER_AUDIO_SEGNAL_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_conf.h"
#include "arm_math.h"
#include "general.h"

#define NUM_TAPS 25
#define NUM_TAPS_2 101
#define NUM_TAPS_3 17
#define NUM_TAPS_4 17

/********************************************************************
 *
 *                   VARIABLES
 *
 ********************************************************************/

//******************************************************
// LMS TEST
float32_t errOutput[BLOCK_SIZE];
//static float32_t LMSBufferOut[BLOCK_SIZE];

#define NUMTAPS_LMS            32
#define MU                     0.95f

float32_t lmsStateF32[NUMTAPS_LMS + BLOCK_SIZE];
float32_t pCoeffs[NUMTAPS_LMS];

arm_lms_instance_f32  	Sl_LMS;

//******************************************************


extern const float32_t firCoeffs32[NUM_TAPS];
extern const float32_t firCoeffs32_2[NUM_TAPS_2];
extern const float32_t firCoeffs32_3[NUM_TAPS_3];
extern const float32_t firCoeffs32_4[NUM_TAPS_4];

/** Hold the coefficents  calculated */
static float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1];
//
/** Hold the coefficents  calculated */
static float32_t firStateF32_2[BLOCK_SIZE + NUM_TAPS_2 - 1];

/** Hold the coefficents  calculated */
static float32_t firStateF32_3[BLOCK_SIZE + NUM_TAPS_3 - 1];

/** Hold the coefficents  calculated */
static float32_t firStateF32_4[BLOCK_SIZE + NUM_TAPS_4 - 1];


#ifdef EXP_SMOOTING
    /** Hold the smooting audio signal status */
    static float32_t smotStatusBuffer[BLOCK_SIZE];
    void smootingFilter(float* smotStatusBuffer, float* bufferIn, float* bufferOut, uint8_t n,int blockSize);
#endif


#ifdef __cplusplus
}
#endif

#endif /* FILTER_AUDIO_SEGNAL_H_INCLUDED */

/**************************************END OF FILE****/
