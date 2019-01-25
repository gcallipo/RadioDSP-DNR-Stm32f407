/**
  ******************************************************************************
  * @file    io_display.c
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
#include "io_display.h"

#include "stm32f4xx_conf.h"
#include "defines.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_spi.h"
#include "tm_stm32f4_ili9341.h"
#include "tm_stm32f4_fonts.h"

/********************************************************************
 *      DISPLAY TIMER
 ********************************************************************/

 /**************************************************************************************************/

void initializeDisplay(){

    SystemInit();
    TM_ILI9341_Init();
	TM_ILI9341_Rotate(TM_ILI9341_Orientation_Landscape_2);

	TM_ILI9341_Fill(ILI9341_COLOR_BLACK);
    TM_ILI9341_DrawRectangle(0, 0, 125, 85,ILI9341_COLOR_YELLOW);
    TM_ILI9341_DrawRectangle(130, 0, 320, 85,ILI9341_COLOR_YELLOW);
	/*************************/
	   TM_ILI9341_Puts(10,5, ">", &TM_Font_11x18,ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
       TM_ILI9341_Puts(22,5, "DNR", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_YELLOW);

       TM_ILI9341_Puts(22,25, "FLT", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_YELLOW);

       TM_ILI9341_Puts(22,45, "SCP", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_YELLOW);

       TM_ILI9341_Puts(22,65, "FNC", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_YELLOW);

	/*************************/

	printDisplayString(60, 5," off ");
    printDisplayString(60, 25," off ");
    printDisplayString(60,45," SPEC");
    printDisplayString(60,65," off");

    fillDisplay(0);
    logoDispaly();

}

void fillDisplay(uint8_t signal){

    TM_ILI9341_DrawRectangle(0, 100, 320, 240,ILI9341_COLOR_YELLOW);
    TM_ILI9341_DrawFilledRectangle(2, 110, 315, 222, ILI9341_COLOR_BLACK);

    if (signal==0){
        TM_ILI9341_Puts(0, 90, "        SPECTRUM SCOPE       ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_YELLOW);
        TM_ILI9341_Puts(5, 225, "0      1k        3k        5k       7k", &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);

        TM_ILI9341_Puts(300,210, "S0", &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
        TM_ILI9341_Puts(300,190, "S1", &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
        TM_ILI9341_Puts(300,170, "S3", &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
        TM_ILI9341_Puts(300,150, "S5", &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
        TM_ILI9341_Puts(300,130, "S7", &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
        TM_ILI9341_Puts(300,110, "S9", &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
    }else  if (signal==1){
        TM_ILI9341_Puts(0, 90, "        WATERFALL SCOPE      ", &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_YELLOW);
        TM_ILI9341_Puts(5, 225, "0   0.5k   1k   1.5k   2k   2.5k   3k ", &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
    }
}


/********************************************************************
 *
 *                   IO BUTTON AND LED SETUP
 *
 ********************************************************************/
void GPIO_Switch_Setup()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    // Initialize GPIOE (PE0) for switch
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 ;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN ;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOE, &GPIO_InitStruct);
}

/* Draw bar for LCD */
/* Simple library to draw bars */
void DrawSpectrumPoints(uint16_t bottomX, uint16_t bottomY, uint16_t maxHeight, float32_t value, uint16_t color) {
	uint16_t height = (uint16_t)(float32_t)value;

	if (height< maxHeight) {
		TM_ILI9341_DrawPixel(bottomX, (bottomY-height), color);
	}
}

/* Simple library to draw waterfall */
void DrawWaterfallPoints(uint16_t bottomX, uint16_t maxHeight, uint16_t maxValue, float32_t value, uint16_t rowCt) {
	uint16_t height;
	uint16_t color;
	height = (uint16_t)((float32_t)value / (float32_t)maxValue * (float32_t)maxHeight);
	if (height >= maxHeight) {
	   color= ILI9341_COLOR_YELLOW;
    }
	else
    if ( COLOR_WF_SKIP_5 <height && height<maxHeight) {
        color= ILI9341_COLOR_YELLOW;
	}
	else if (COLOR_WF_SKIP_4<=height && height<COLOR_WF_SKIP_5) {
        color= ILI9341_COLOR_CYAN;
	}
    else if (COLOR_WF_SKIP_3<=height && height<COLOR_WF_SKIP_4) {
        color= ILI9341_COLOR_CYAN;
	}
    else if (COLOR_WF_SKIP_2<=height && height<COLOR_WF_SKIP_3) {
        color= ILI9341_COLOR_BLUE2;
	}
    else if (COLOR_WF_SKIP_1<=height && height<COLOR_WF_SKIP_2) {
        color= ILI9341_COLOR_BLUE2;
	}
    else if (COLOR_WF_SKIP_0<=height && height<COLOR_WF_SKIP_1) {
        color= ILI9341_COLOR_BLUE;
	}else{
	     color= ILI9341_COLOR_BLACK;
	}

	TM_ILI9341_DrawPixel(bottomX, (120+rowCt), color);
}

void decodeMenuValue(){

    if (menu_voice ==0){
       if (ict>iLastict){
           iCmdDnr++;
       }else if (ict<iLastict){
           iCmdDnr= (iCmdDnr>0)?(iCmdDnr-1):0;
       }

       if (ict!=iLastict){
        printDisplayValues (60,5, iCmdDnr);
        if (iCmdDnr==0) {printDisplayString(60, 5," off "); printDisplayString(110, 5," ");}
        if (iCmdDnr>0 && iCmdDnr<=4) printDisplayString(91, 5," N1");
        if (iCmdDnr>4) printDisplayString(91, 5," N2");
       }

    }
    else if (menu_voice ==1){
      if (ict>iLastict){
           iCmdFlt=(iCmdFlt<3)?(iCmdFlt+1):3;
           //printDisplayValues (60,25, iCmdFlt);
       }else if (ict<iLastict){
           iCmdFlt= (iCmdFlt>0)?(iCmdFlt-1):0;
           //printDisplayValues (60,25, iCmdFlt);
          }

      if (ict!=iLastict){
        //printDisplayValues (60,5, iCmdDnr);
        if (iCmdFlt==0) printDisplayString(60, 25," off ");
        if (iCmdFlt==1) printDisplayString(60, 25," WID ");
        if (iCmdFlt==2) printDisplayString(60, 25," MED ");
        if (iCmdFlt==3) printDisplayString(60, 25," NAR ");
       }
    }

    else if (menu_voice ==2){

         if (ict>iLastict){
               if (iCmdScp==0){
                 iCmdScp++;
                 printDisplayString(60,45," WFAL");
                 fillDisplay(1);
               }

         }else if (ict<iLastict){
               if (iCmdScp==1){
                 iCmdScp--;
                 printDisplayString(60,45," SPEC");
                 fillDisplay(0);
               }
           }
    }
    else if (menu_voice ==3){
      if (ict>iLastict){
           iCmdFnc=(iCmdFnc<1)?(iCmdFnc+1):1;
           initializeDecoder();
           clearMorseDispaly();
           printDisplayString(60,65," CW ");
       }else if (ict<iLastict){
           iCmdFnc= (iCmdFnc>0)?(iCmdFnc-1):0;
           clearMorseDispaly();
           logoDispaly();
           printDisplayString(60,65," off");
          }
    }

   iLastict = ict;
}


void printDisplayValues (uint8_t x, uint8_t y, uint8_t value){
      char str[10];
      sprintf (str, " %d ", (int)(value));
      TM_ILI9341_Puts(x,y, str, &TM_Font_11x18, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
}


void printDisplayString (uint8_t x, uint8_t y, char str[] ){

      TM_ILI9341_Puts(x,y, str, &TM_Font_11x18, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
}

void decodeMenuVoice(){

    menu_voice=(menu_voice<3)?(menu_voice+1):0;
    if (menu_voice ==0){
       TM_ILI9341_Puts(10, 5, ">", &TM_Font_11x18,ILI9341_COLOR_YELLOW,  ILI9341_COLOR_BLACK);
       TM_ILI9341_Puts(10,25, " ", &TM_Font_11x18,ILI9341_COLOR_BLACK,   ILI9341_COLOR_BLACK);
       TM_ILI9341_Puts(10,45, " ", &TM_Font_11x18, ILI9341_COLOR_BLACK,  ILI9341_COLOR_BLACK);
       TM_ILI9341_Puts(10,65, " ", &TM_Font_11x18, ILI9341_COLOR_BLACK,  ILI9341_COLOR_BLACK);
    }else if (menu_voice ==1){
       TM_ILI9341_Puts(10, 5, " ", &TM_Font_11x18,ILI9341_COLOR_BLACK,   ILI9341_COLOR_BLACK);
       TM_ILI9341_Puts(10,25, ">", &TM_Font_11x18, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
       TM_ILI9341_Puts(10,45, " ", &TM_Font_11x18, ILI9341_COLOR_BLACK,  ILI9341_COLOR_BLACK);
       TM_ILI9341_Puts(10,65, " ", &TM_Font_11x18, ILI9341_COLOR_BLACK,  ILI9341_COLOR_BLACK);
    }else if (menu_voice ==2){
       TM_ILI9341_Puts(10, 5, " ", &TM_Font_11x18,ILI9341_COLOR_BLACK,   ILI9341_COLOR_BLACK);
       TM_ILI9341_Puts(10,25, " ", &TM_Font_11x18, ILI9341_COLOR_BLACK,  ILI9341_COLOR_BLACK);
       TM_ILI9341_Puts(10,45, ">", &TM_Font_11x18, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
       TM_ILI9341_Puts(10,65, " ", &TM_Font_11x18, ILI9341_COLOR_BLACK,  ILI9341_COLOR_BLACK);
    }else if (menu_voice ==3){
       TM_ILI9341_Puts(10, 5, " ", &TM_Font_11x18,ILI9341_COLOR_BLACK,   ILI9341_COLOR_BLACK);
       TM_ILI9341_Puts(10,25, " ", &TM_Font_11x18, ILI9341_COLOR_BLACK,  ILI9341_COLOR_BLACK);
       TM_ILI9341_Puts(10,45, " ", &TM_Font_11x18, ILI9341_COLOR_BLACK,  ILI9341_COLOR_BLACK);
       TM_ILI9341_Puts(10,65, ">", &TM_Font_11x18, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
    }

}

/********************************************************************
 *                   Time base configuration
 * Calculate is as:
 * Timer freq = timer_clock / ((TIM_Prescaler+1) * (TIM_Period+1))

 * Timer freq = timer_clock / ((TIM_Prescaler+1) * (TIM_Period+1))
 * Timer freq = 84000000 / (1500+1)*(1500+1) = 37 Hz
 ********************************************************************/
void TIM3_Config(void){

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	TIM_TimeBaseInitStruct.TIM_Prescaler = 1500;
	TIM_TimeBaseInitStruct.TIM_Period = 1000;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	// Enable TIM3 interrupt
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);

	// Step 4: Initialize NVIC for timer interrupt
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

}

/********************************************************************
 *                   Time base configuration
 * This is the timemillis timer that count milliseconds.
 ********************************************************************/
void TIM5_Config(void){

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct.TIM_Period = 84000;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct);
	// Enable TIM3 interrupt
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);

	// Step 4: Initialize NVIC for timer interrupt
	NVIC_InitStruct.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

}

int row =0;
int col =1;
void showMorseChar(char c){

    char str[15];

    sprintf (str, "               ");
    TM_ILI9341_Putc(120 + col*15,5+row*20, c, &TM_Font_11x18, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
    if ((120+col*20) >300) {
        col=1; row++;
        if (row <4) TM_ILI9341_Puts(120 + col*15,5+row*20, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLACK);
    }
    if ((5+row*20) >65) {
        col=1; row= 0;
        TM_ILI9341_Puts(120 + col*15,5+row*20, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLACK);
    }
    col++;
}

void clearMorseDispaly(){
    char str[15];
    sprintf (str, "               ");
    TM_ILI9341_Puts(135, 5, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLACK);
    TM_ILI9341_Puts(135, 25, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLACK);
    TM_ILI9341_Puts(135, 45, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLACK);
    TM_ILI9341_Puts(135, 65, str, &TM_Font_11x18, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLACK);
    row =0;
    col =1;
}


void logoDispaly(){

    char str1[15];
    char str2[25];
    char str3[25];

    sprintf (str1, "    RadioDSP   ");
    sprintf (str2, "     audio processor");
    sprintf (str3, "         ik8yfw");

    TM_ILI9341_Puts(135, 25, str1, &TM_Font_11x18, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
    TM_ILI9341_Puts(135, 45, str2, &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
    TM_ILI9341_Puts(135, 65, str3, &TM_Font_7x10, ILI9341_COLOR_YELLOW, ILI9341_COLOR_BLACK);
}

/**************************************END OF FILE****/























