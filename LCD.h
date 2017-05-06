#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/flash.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/udma.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/checkbox.h"
#include "grlib/container.h"
#include "grlib/pushbutton.h"
#include "grlib/radiobutton.h"
#include "grlib/slider.h"
#include "utils/ustdlib.h"
#include "drivers/Kentec320x240x16_ssd2119_spi.h"
#include "drivers/touch.h"
#include "utils/ustdlib.h"



#ifndef __LCD_H__
#define __LCD_H__


extern tContext sContext;
extern tRectangle sRect,Rect_M,Rect_L,Rect_R,Rect_t1,Rect_t2;
extern uint32_t ui32SysClock;

extern uint8_t data_ch[4][40];
extern uint8_t data_ch_prev[4][40];

extern uint8_t Y_max[4];
extern uint8_t Y_min[4];

extern int32_t print_left;
extern int32_t print_right;
extern char str_left[10];
extern char str_right[10];
extern char str_time[10];

extern uint32_t time_interval;

void print_time(void);
void clear_screen(void);
void print_waveform(uint8_t channel);
void data_convert(uint8_t * data_in, uint8_t * data_out, uint32_t num_data);
void prev_init(void);



#endif // __LCD_H__
