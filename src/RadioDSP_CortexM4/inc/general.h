/**
  ******************************************************************************
  * @file    general.h
  * @author  Giuseppe Callipo - IK8YFW - ik8yfw@libero.it
  * @version V2.0.0
  * @date    25-09-2018
  * @brief   IO Routine
  *
  *
  * NOTE: This file is part of RadioDSP project.
  *       See main.c file for additional project informations.
  ******************************************************************************/

#ifndef GENERAL_H_INCLUDED
#define GENERAL_H_INCLUDED

#ifdef __cplusplus
 extern "C" {
#endif

/*- Size of sampleing block and fft */
#define BLOCK_SIZE		         256
#define FFT_SIZE                 256
/* 120 px on the LCD */
#define FFT_BAR_MAX_HEIGHT		 100
#define MAX_WF_ROWS		         100

#define COLOR_WF_SKIP    FFT_BAR_MAX_HEIGHT/6
#define COLOR_WF_SKIP_5  COLOR_WF_SKIP*6
#define COLOR_WF_SKIP_4  COLOR_WF_SKIP*5
#define COLOR_WF_SKIP_3  COLOR_WF_SKIP*4
#define COLOR_WF_SKIP_2  COLOR_WF_SKIP*3
#define COLOR_WF_SKIP_1  COLOR_WF_SKIP*2
#define COLOR_WF_SKIP_0  COLOR_WF_SKIP

/********************************************************************
 *
 *                   VARIABLES
 *
 ********************************************************************/
/*- Global working buffers */
static float32_t BufferIn     [BLOCK_SIZE];
static float32_t BufferOut    [BLOCK_SIZE];
static float32_t Buffer1      [BLOCK_SIZE];
//static float32_t Buffer2      [BLOCK_SIZE];
//static float32_t Buffer3      [BLOCK_SIZE];
//static float32_t Buffer4      [BLOCK_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* GENERAL_H_INCLUDED */

/**************************************END OF FILE****/
