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
#include "touch_isr.h"
#include "images.h"

extern uint32_t byte_count;
extern uint8_t flag_intr;

uint32_t time_interval_step[] = {10000,5000,2000,1000,500,200};
uint32_t time_interval_ptr = 3;
uint32_t print_interval_step[] = {4000,2000,1000,400,200,80,40};
uint8_t print_int_ptr = 10;
uint8_t time_print_flag = 100;

RectangularButton(wave_print, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 79, 49,
                  140, 140, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0,
				  wave_print_press);
RectangularButton(increase, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 0, 0,
                  319, 40, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0,
				  increase_press);
RectangularButton(decrease, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 0, 199,
                  319, 40, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0,
				  decrease_press);
RectangularButton(left, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 0, 20,
                  80, 220, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0,
				  left_press);
RectangularButton(right, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 219, 20,
                  100, 220, 0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0, 0,
				  right_press);



RectangularButton(start, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 0, 179,
                  319, 60, PB_STYLE_FILL | PB_STYLE_TEXT, ClrDarkBlue, ClrDarkBlue, 0, ClrSilver,
				  &g_sFontCm20, "START", 0, 0, 0, 0,
				  start_press);
RectangularButton(plus, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 0, 20,
                  70, 70, PB_STYLE_IMG | PB_STYLE_TEXT, ClrBlack, ClrBlack, 0, ClrSilver,
                  &g_sFontCm20, "+", g_pui8Blue50x50, g_pui8Blue50x50Press, 0, 0,
				  plus_press);
RectangularButton(minus, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 0, 90,
                  70, 70, PB_STYLE_IMG | PB_STYLE_TEXT, ClrBlack, ClrBlack, 0,
                  ClrSilver, &g_sFontCm20, "-", g_pui8Blue50x50,
                  g_pui8Blue50x50Press, 0, 0, minus_press);


Slider(move_slider, 0, 0, 0, &g_sKentec320x240x16_SSD2119, 0, 69, 319, 100, 0, 100, 50,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, move_slider_press);

int32_t pre_value = 50;
void move_slider_press(tWidget *psWidget, int32_t value)
{
	int32_t diff_value = value - pre_value;

	if(abs(diff_value) > 3)
	{
		pre_value = value;

		if(diff_value > 0)
		{
			if(print_interval_step[print_int_ptr] == 40)
			{
				print_left = print_left - (print_interval_step[print_int_ptr]/40);
				print_right = print_right - (print_interval_step[print_int_ptr]/40);
			}
			else if(print_interval_step[print_int_ptr] == 80)
			{
				print_left = print_left - (print_interval_step[print_int_ptr]/80);
				print_right = print_right - (print_interval_step[print_int_ptr]/80);
			}
			else
			{
				print_left = print_left - (print_interval_step[print_int_ptr]/100);
				print_right = print_right - (print_interval_step[print_int_ptr]/100);
			}

			if(print_left <= 0)
			{
				print_left = 0;
				print_right = print_left + print_interval_step[print_int_ptr];
			}
		}

		if(diff_value < 0)
		{
			if(print_interval_step[print_int_ptr] == 40)
			{
				print_left = print_left + (print_interval_step[print_int_ptr]/40);
				print_right = print_right + (print_interval_step[print_int_ptr]/40);
			}
			else if(print_interval_step[print_int_ptr] == 80)
			{
				print_left = print_left + (print_interval_step[print_int_ptr]/80);
				print_right = print_right + (print_interval_step[print_int_ptr]/80);
			}
			else
			{
				print_left = print_left + (print_interval_step[print_int_ptr]/100);
				print_right = print_right + (print_interval_step[print_int_ptr]/100);
			}

			if(print_right >= 4000)
			{
				print_right = 4000;
				print_left = print_right - print_interval_step[print_int_ptr];

			}
		}

		UARTprintf("%d\t%d\n",print_left,print_right);

		data_convert(&SUT1_data + print_left, (uint8_t *)data_ch, print_interval_step[print_int_ptr]);
		data_convert(&SUT2_data + print_left, (uint8_t *)data_ch + 40, print_interval_step[print_int_ptr]);
		data_convert(&SUT3_data + print_left, (uint8_t *)data_ch + 80, print_interval_step[print_int_ptr]);
		data_convert(&SUT4_data + print_left, (uint8_t *)data_ch + 120, print_interval_step[print_int_ptr]);

		print_waveform(0);
		print_waveform(1);
		print_waveform(2);
		print_waveform(3);


		time_print_flag = 1;

	}


	/*
	if(i32Value > 50)
	{
		print_left = print_left - print_interval_step[print_int_ptr];
		print_right = print_right - print_interval_step[print_int_ptr];

		if(print_left <= 0)
		{
			print_left = 0;
			print_right = print_left + print_interval_step[print_int_ptr];
		}
	}*/


}

void start_press(tWidget *psWidget)
{
	UARTprintf("start_press");

	time_interval = time_interval_step[time_interval_ptr];
	SysTickPeriodSet((time_interval/100)*125);

	clear_screen();
	GrContextForegroundSet(&sContext, ClrWhite);
	GrLineDrawH(&sContext, 0, 319, 74);
	GrLineDrawH(&sContext, 0, 319, 129);
	GrLineDrawH(&sContext, 0, 319, 184);
	GrLineDrawH(&sContext, 0, 319, 239);

	WidgetRemove((tWidget *)&start);
	WidgetRemove((tWidget *)&minus);
	WidgetRemove((tWidget *)&plus);

	//WidgetAdd(WIDGET_ROOT, (tWidget *)&wave_print);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&increase);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&decrease);
	/*WidgetAdd(WIDGET_ROOT, (tWidget *)&left);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&right);*/
	WidgetAdd(WIDGET_ROOT, (tWidget *)&move_slider);

	byte_count = 0;
	flag_intr = 0;
	time_print_flag = 0;
	SysTickEnable();

	while(flag_intr == 0);

	data_convert(&SUT1_data, (uint8_t *)data_ch, print_interval_step[0]);
	data_convert(&SUT2_data, (uint8_t *)data_ch + 40, print_interval_step[0]);
	data_convert(&SUT3_data, (uint8_t *)data_ch + 80, print_interval_step[0]);
	data_convert(&SUT4_data, (uint8_t *)data_ch + 120, print_interval_step[0]);

	print_waveform(0);
	print_waveform(1);
	print_waveform(2);
	print_waveform(3);

	print_int_ptr = 0;
	print_left = 0;
	print_right = 4000;

	time_print_flag = 1;
}

void plus_press(tWidget *psWidget)
{
	UARTprintf("plus_press");

	if(time_interval_ptr == 0)
		time_interval_ptr = 0;
	else
		time_interval_ptr--;


	usprintf(str_time, "%3dms",time_interval_step[time_interval_ptr]);
	GrContextForegroundSet(&sContext, ClrBlack);
	GrRectFill(&sContext, &Rect_t2);
	GrContextForegroundSet(&sContext, ClrWhite);
	GrContextFontSet(&sContext, &g_sFontCm14);
	GrStringDrawCentered(&sContext, str_time, -1, 159, 99, 0);
}

void minus_press(tWidget *psWidget)
{
	UARTprintf("minus_press");

	time_interval_ptr++;
	if(time_interval_ptr > 5)
		time_interval_ptr = 5;

	usprintf(str_time, "%3dms",time_interval_step[time_interval_ptr]);
	GrContextForegroundSet(&sContext, ClrBlack);
	GrRectFill(&sContext, &Rect_t2);
	GrContextForegroundSet(&sContext, ClrWhite);
	GrContextFontSet(&sContext, &g_sFontCm14);
	GrStringDrawCentered(&sContext, str_time, -1, 159, 99, 0);
}

void wave_print_press(tWidget *psWidget)
{
	UARTprintf("wave_print_press byte = %d  \n",byte_count);
	byte_count = 0;
	flag_intr = 0;
	time_print_flag = 0;
	SysTickEnable();

	while(flag_intr == 0);

	data_convert(&SUT1_data, (uint8_t *)data_ch, print_interval_step[0]);
	data_convert(&SUT2_data, (uint8_t *)data_ch + 40, print_interval_step[0]);
	data_convert(&SUT3_data, (uint8_t *)data_ch + 80, print_interval_step[0]);
	data_convert(&SUT4_data, (uint8_t *)data_ch + 120, print_interval_step[0]);

	print_waveform(0);
	print_waveform(1);
	print_waveform(2);
	print_waveform(3);

	print_int_ptr = 0;
	print_left = 0;
	print_right = 4000;

	time_print_flag = 1;
}


void increase_press(tWidget *psWidget)
{


	print_int_ptr++;
	if(print_int_ptr > 6)
		print_int_ptr = 6;

	print_right = print_left + print_interval_step[print_int_ptr];

	UARTprintf("increase_press ptr = %d\tleft = %d\ttright = %d\n",print_interval_step[print_int_ptr],print_left,print_right);


	data_convert(&SUT1_data + print_left, (uint8_t *)data_ch, print_interval_step[print_int_ptr]);
	data_convert(&SUT2_data + print_left, (uint8_t *)data_ch + 40, print_interval_step[print_int_ptr]);
	data_convert(&SUT3_data + print_left, (uint8_t *)data_ch + 80, print_interval_step[print_int_ptr]);
	data_convert(&SUT4_data + print_left, (uint8_t *)data_ch + 120, print_interval_step[print_int_ptr]);

	print_waveform(0);
	print_waveform(1);
	print_waveform(2);
	print_waveform(3);

	time_print_flag = 1;
}
void decrease_press(tWidget *psWidget)
{
	if(print_int_ptr == 0)
		print_int_ptr = 0;
	else
		print_int_ptr--;

	print_right = print_left + print_interval_step[print_int_ptr];

	if(print_right >= 4000)
	{
		print_right = 4000;
		print_left = print_right - print_interval_step[print_int_ptr];
	}

	UARTprintf("decrease_press ptr = %d\tleft = %d\ttright = %d\n",print_interval_step[print_int_ptr],print_left,print_right);

	data_convert(&SUT1_data + print_left, (uint8_t *)data_ch, print_interval_step[print_int_ptr]);
	data_convert(&SUT2_data + print_left, (uint8_t *)data_ch + 40, print_interval_step[print_int_ptr]);
	data_convert(&SUT3_data + print_left, (uint8_t *)data_ch + 80, print_interval_step[print_int_ptr]);
	data_convert(&SUT4_data + print_left, (uint8_t *)data_ch + 120, print_interval_step[print_int_ptr]);

	print_waveform(0);
	print_waveform(1);
	print_waveform(2);
	print_waveform(3);

	time_print_flag = 1;

}
void left_press(tWidget *psWidget)
{
	print_left = print_left - print_interval_step[print_int_ptr];
	print_right = print_right - print_interval_step[print_int_ptr];

	if(print_left <= 0)
	{
		print_left = 0;
		print_right = print_left + print_interval_step[print_int_ptr];
	}

	UARTprintf("left_press\tleft = %d right = %d\n",print_left,print_right);

	data_convert(&SUT1_data + print_left, (uint8_t *)data_ch, print_interval_step[print_int_ptr]);
	data_convert(&SUT2_data + print_left, (uint8_t *)data_ch + 40, print_interval_step[print_int_ptr]);
	data_convert(&SUT3_data + print_left, (uint8_t *)data_ch + 80, print_interval_step[print_int_ptr]);
	data_convert(&SUT4_data + print_left, (uint8_t *)data_ch + 120, print_interval_step[print_int_ptr]);

	print_waveform(0);
	print_waveform(1);
	print_waveform(2);
	print_waveform(3);

	time_print_flag = 1;
}
void right_press(tWidget *psWidget)
{
	print_left = print_left + print_interval_step[print_int_ptr];
	print_right = print_right + print_interval_step[print_int_ptr];

	if(print_right >= 4000)
	{
		print_right = 4000;
		print_left = print_right - print_interval_step[print_int_ptr];

	}

	UARTprintf("right_press\tleft = %d right = %d\n",print_left,print_right);

	data_convert(&SUT1_data + print_left, (uint8_t *)data_ch, print_interval_step[print_int_ptr]);
	data_convert(&SUT2_data + print_left, (uint8_t *)data_ch + 40, print_interval_step[print_int_ptr]);
	data_convert(&SUT3_data + print_left, (uint8_t *)data_ch + 80, print_interval_step[print_int_ptr]);
	data_convert(&SUT4_data + print_left, (uint8_t *)data_ch + 120, print_interval_step[print_int_ptr]);

	print_waveform(0);
	print_waveform(1);
	print_waveform(2);
	print_waveform(3);

	time_print_flag = 1;
}



