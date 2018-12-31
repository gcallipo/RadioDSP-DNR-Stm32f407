/**
  ******************************************************************************
  * @file    io_display.h
  * @author  Giuseppe Callipo - IK8YFW - ik8yfw@libero.it
  * @version V2.0.0
  * @date    25-09-2018
  * @brief   IO Routine
  *
  *
  * NOTE: This file is part of RadioDSP project.
  *       See main.c file for additional project informations.
  ******************************************************************************/

#ifndef IO_DISPLAY_H_INCLUDED
#define IO_DISPLAY_H_INCLUDED


#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
#include "arm_math.h"
#include "general.h"

/********************************************************************
 *
 *                   VARIABLES
 *
 ********************************************************************/
extern uint8_t menu_voice;
extern int     ict;
extern uint8_t switchPush;
extern uint8_t menu_voice;
extern uint8_t iCmdDnr;
extern uint8_t iCmdFlt;
extern uint8_t iCmdScp;
extern int     iLastict;


/********************************************************************
 *
 *                   DISPLAY FNC
 *
 ********************************************************************/
void TIM3_Config(void);
void initializeDisplay();
void DrawWaterfallPoints(uint16_t bottomX, uint16_t maxHeight, uint16_t maxValue, float32_t value, uint16_t rowCt);
void DrawSpectrumPoints (uint16_t bottomX, uint16_t bottomY, uint16_t maxHeight, float32_t value, uint16_t color);
void decodeMenuValue();
void decodeMenuVoice();
void GPIO_Switch_Setup();
void fillDisplay(uint8_t signal);

#ifdef __cplusplus
}
#endif


#endif /* IO_AUDIO_H_INCLUDED */

/**************************************END OF FILE****/
