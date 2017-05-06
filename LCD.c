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

#include "files/UART.h"
#include "LCD.h"

tContext sContext;
tRectangle sRect,Rect_M,Rect_L,Rect_R,Rect_t1,Rect_t2;
uint32_t ui32SysClock;

uint8_t data_ch[4][40] = {{0xFF},{0xFF},{0xFF},{0xFF}};
uint8_t data_ch_prev[4][40] = {{0xFF},{0xFF},{0xFF},{0xFF}};

uint8_t Y_max[4] = {49,104,159,214};
uint8_t Y_min[4] = {74,129,184,239};

int32_t print_left = 4000;
int32_t print_right = 4000;
char str_left[10];
char str_right[10];
char str_time[10];

uint32_t time_interval = 1000;

void print_time(void)
{
	float time_left = ((float)print_left/4000.0)*((float)time_interval);
	float time_right = ((float)print_right/4000.0)*((float)time_interval);
	int32_t int_part, frac_part;


	int_part = (int32_t) time_left;
	frac_part = (int32_t) (time_left * 1000.0f);
	frac_part = frac_part - (int_part * 1000);
	if(frac_part < 0)
		frac_part *= -1;

	usprintf(str_left, "%3d.%03dms", int_part, frac_part);
	GrContextForegroundSet(&sContext, ClrDarkBlue);
	GrRectFill(&sContext, &Rect_L);
	GrContextForegroundSet(&sContext, ClrWhite);
	GrContextFontSet(&sContext, &g_sFontCm12);
	GrStringDrawCentered(&sContext, str_left, -1,39, 9, 0);
	UARTprintf("time\t%3d.%03d\t", int_part, frac_part);

	int_part = (int32_t) time_right;
	frac_part = (int32_t) (time_right * 1000.0f);
	frac_part = frac_part - (int_part * 1000);
	if(frac_part < 0)
		frac_part *= -1;

	usprintf(str_right, "%3d.%03dms", int_part, frac_part);
	GrContextForegroundSet(&sContext, ClrDarkBlue);
	GrRectFill(&sContext, &Rect_R);
	GrContextForegroundSet(&sContext, ClrWhite);
	GrContextFontSet(&sContext, &g_sFontCm12);
	GrStringDrawCentered(&sContext, str_right, -1,279, 9, 0);
	UARTprintf("%3d.%03d\n", int_part, frac_part);

}

void clear_screen(void)
{
	sRect.i16XMin = 0;
	sRect.i16YMin = 20;
	sRect.i16XMax = 319;
	sRect.i16YMax = 239;
	GrContextForegroundSet(&sContext, ClrBlack);
	GrRectFill(&sContext, &sRect);

	GrContextForegroundSet(&sContext, ClrDimGray);
	GrLineDrawV(&sContext, 159, 20, 239);
	GrLineDrawV(&sContext, 79, 20, 239);
	GrLineDrawV(&sContext, 239, 20, 239);

	GrLineDrawH(&sContext, 0, 319, 74);
	GrLineDrawH(&sContext, 0, 319, 129);
	GrLineDrawH(&sContext, 0, 319, 184);
	GrLineDrawH(&sContext, 0, 319, 239);
}



void print_waveform(uint8_t channel)
{
	uint8_t i,j,mask,val,val_prev;

	GrContextForegroundSet(&sContext, ClrWhite);

	mask = 0x01;
	val = data_ch[channel][0] & mask;
	val_prev = data_ch_prev[channel][0] & mask;

	for(i = 0; i < 40; i++)
	{
		mask = 0x01;
		for(j = 0; j < 8; j++)
		{
			if((i != 0) || (j != 0))
			{
				if(data_ch[channel][i] & mask)
				{
					if(val == 1)
						val = 0;
					else
						val = 1;
				}
				if(data_ch_prev[channel][i] & mask)
				{
					if(val_prev == 1)
						val_prev = 0;
					else
						val_prev = 1;
				}
			}

			if(((data_ch[channel][i] & mask) != (data_ch_prev[channel][i] & mask)) && ((i != 0) || (j != 0)))
			{
				if(data_ch[channel][i] & mask)
				{
					GrLineDrawV(&sContext, (i*8 + j), Y_max[channel], Y_min[channel]);
				}
				else
				{
					GrContextForegroundSet(&sContext, ClrDimGray);
					GrPixelDraw(&sContext, (i*8 + j), Y_min[channel]);
					if((i*8 + j) == 79 || (i*8 + j) == 159 || (i*8 + j) == 239)
					{
						GrLineDrawV(&sContext, (i*8 + j), Y_max[channel], Y_min[channel] - 1);
					}
					else
					{
						GrContextForegroundSet(&sContext, ClrBlack);
						GrLineDrawV(&sContext, (i*8 + j), Y_max[channel], Y_min[channel] - 1);
					}
					GrContextForegroundSet(&sContext, ClrWhite);
					if(val)
						GrPixelDraw(&sContext, (i*8 + j), Y_max[channel]);
					else
						GrPixelDraw(&sContext, (i*8 + j), Y_min[channel]);
				}
			}
			else if(val != val_prev)
			{
				if(val)
				{
					GrContextForegroundSet(&sContext, ClrDimGray);
					GrPixelDraw(&sContext, (i*8 + j), Y_min[channel]);
					GrContextForegroundSet(&sContext, ClrWhite);
					GrPixelDraw(&sContext, (i*8 + j), Y_max[channel]);
				}
				else
				{
					if((i*8 + j) == 79 || (i*8 + j) == 159 || (i*8 + j) == 239)
					{
						GrContextForegroundSet(&sContext, ClrDimGray);
						GrPixelDraw(&sContext, (i*8 + j), Y_max[channel]);
					}
					else
					{
						GrContextForegroundSet(&sContext, ClrBlack);
						GrPixelDraw(&sContext, (i*8 + j), Y_max[channel]);
					}
					GrContextForegroundSet(&sContext, ClrWhite);
					GrPixelDraw(&sContext, (i*8 + j), Y_min[channel]);
				}
			}

			mask = mask << 1;
		}

		data_ch_prev[channel][i] = data_ch[channel][i];
	}
}

void data_convert(uint8_t * data_in, uint8_t * data_out, uint32_t num_data)
{
	uint32_t num_bits;
	int16_t i,j;
	uint8_t val, val_prev, mask_in, mask_out, trans_count;

	num_bits = num_data/40;

	mask_in = 0x01;
	mask_out = 0x01;

	val = (*data_in) & mask_in;
	val_prev = (*data_in) & mask_in;

	trans_count = 0;

	for(i = 0; i < 320; i++)
	{
		if((i%8) == 0)
		{
			if(i != 0)
				data_out = data_out + 1;

			mask_out = 0x01;
		}

		trans_count = 0;
		for(j = 0; j < num_bits; j++)
		{
			if(((i*num_bits + j)%8) == 0)
			{
				if((i*num_bits + j) != 0)
					data_in = data_in + 1;
				mask_in = 0x01;
			}

			if(i != 0)
			{
				if((*data_in) & mask_in)
				{
					val = 1;
				}
				else
				{
					val = 0;
				}

				if(val != val_prev)
				{
					trans_count++;

				}
			}

			val_prev = val;
			mask_in = mask_in << 1;
		}

		if(i == 0)
		{
			(*data_out) = ((*data_out) & ~mask_out) | val;
		}
		else
		{
			if((trans_count)%2 == 0)
			{
				(*data_out) = ((*data_out) & ~mask_out);
			}
			else
			{
				(*data_out) = ((*data_out) | mask_out);
			}
		}
		mask_out = mask_out << 1;
	}
}

void prev_init(void)
{
	for(uint8_t i = 0; i < 4; i++)
		for(uint8_t j = 0; j < 40; j++)
			data_ch_prev[i][j] = 0;
}
