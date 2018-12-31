/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H
#include "tm_stm32f4_ili9341.h"  //drivers header file
#include "gfx.h"

static __inline void init_board(GDisplay *g) {
	(void) g;
	TM_ILI9341_Init();
}

static __inline void post_init_board(GDisplay *g) {
	(void) g;
}

static __inline void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	if(state == 1){
		ILI9341_RST_RESET;}	
	else {
		ILI9341_RST_SET;
	}
}

static __inline void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void) percent;
}

static __inline void acquire_bus(GDisplay *g) {
	(void) g;
}

static __inline void release_bus(GDisplay *g) {
	(void) g;
}

static __inline void write_index(GDisplay *g, uint8_t index) {
	(void) g;
	TM_ILI9341_SendCommand(index);
}

static __inline void write_data(GDisplay *g, uint8_t data) {
	(void) g;
	TM_ILI9341_SendData(data);
}

static __inline void setreadmode(GDisplay *g) {
	(void) g;
}

static __inline void setwritemode(GDisplay *g) {
	(void) g;
}

static __inline uint16_t read_data(GDisplay *g) {
	(void) g;
	return 0;
}

#endif /* _GDISP_LLD_BOARD_H */
