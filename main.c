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

#include "files/Delay.h"
#include "files/SWITCH.h"
#include "files/UART.h"
#include "images.h"
#include "LCD.h"
#include "touch_isr.h"


uint8_t * data_in_ch1 = &SUT1_data;
uint8_t * data_in_ch2 = &SUT2_data;
uint8_t * data_in_ch3 = &SUT3_data;
uint8_t * data_in_ch4 = &SUT4_data;

uint8_t bit_count = 0;
uint32_t byte_count = 10 , mask_intr = 0x01;
uint8_t flag_intr = 0xFF;


void SysTickIntHandler(void)
{

	int32_t GPIO = HWREG(GPIO_PORTE_BASE + (GPIO_O_DATA + ((GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4) << 2)));
	if((GPIO & GPIO_PIN_1))
	{
		(*(uint32_t *)data_in_ch1) = (*(uint32_t *)data_in_ch1) | mask_intr;
	}
	else
	{
		(*(uint32_t *)data_in_ch1) = (*(uint32_t *)data_in_ch1) & ~mask_intr;
	}

	if((GPIO & GPIO_PIN_2))
	{
		(*(uint32_t *)data_in_ch2) = (*(uint32_t *)data_in_ch2) | mask_intr;
	}
	else
	{
		(*(uint32_t *)data_in_ch2) = (*(uint32_t *)data_in_ch2) & ~mask_intr;
	}

	if((GPIO & GPIO_PIN_3))
	{
		(*(uint32_t *)data_in_ch3) = (*(uint32_t *)data_in_ch3) | mask_intr;
	}
	else
	{
		(*(uint32_t *)data_in_ch3) = (*(uint32_t *)data_in_ch3) & ~mask_intr;
	}

	if((GPIO & GPIO_PIN_4))
	{
		(*(uint32_t *)data_in_ch4) = (*(uint32_t *)data_in_ch4) | mask_intr;
	}
	else
	{
		(*(uint32_t *)data_in_ch4) = (*(uint32_t *)data_in_ch4) & ~mask_intr;
	}

	mask_intr = mask_intr << 1;
	if(mask_intr == 0)//(bit_count == 8)
	{
		data_in_ch1 += 4;
		data_in_ch2 += 4;
		data_in_ch3 += 4;
		data_in_ch4 += 4;
		mask_intr = 0x01;
		byte_count++;
		if(byte_count == 1000)
		{
			data_in_ch1 = &SUT1_data;
			data_in_ch2 = &SUT2_data;
			data_in_ch3 = &SUT3_data;
			data_in_ch4 = &SUT4_data;
			byte_count = 0;
			SysTickDisable();
			flag_intr = 1;
		}
	}
}

void system_init(void)
{
	FPUEnable();
	FPUStackingDisable();

	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	UART_init();

	SysTickPeriodSet((time_interval/100)*125);
	SysTickIntRegister(SysTickIntHandler);
	SysTickIntEnable();

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	prev_init();
	IntMasterEnable();


}


int main(void)
{
	system_init();

	UARTprintf("Logic Analyzer\n");

	ui32SysClock = SysCtlClockGet();

	Kentec320x240x16_SSD2119Init(ui32SysClock);

	GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);

	TouchScreenInit(ui32SysClock);
	TouchScreenCallbackSet(WidgetPointerMessage);

	Rect_M.i16XMin = 99;
	Rect_M.i16YMin = 0;
	Rect_M.i16XMax = 219;
	Rect_M.i16YMax = 19;
	GrContextForegroundSet(&sContext, ClrDarkBlue);
	GrRectFill(&sContext, &Rect_M);

	Rect_L.i16XMin = 0;
	Rect_L.i16YMin = 0;
	Rect_L.i16XMax = 99;
	Rect_L.i16YMax = 19;
	GrContextForegroundSet(&sContext, ClrDarkBlue);
	GrRectFill(&sContext, &Rect_L);

	Rect_R.i16XMin = 219;
	Rect_R.i16YMin = 0;
	Rect_R.i16XMax = 319;
	Rect_R.i16YMax = 19;
	GrContextForegroundSet(&sContext, ClrDarkBlue);
	GrRectFill(&sContext, &Rect_R);

	GrContextForegroundSet(&sContext, ClrWhite);
	GrContextFontSet(&sContext, &g_sFontCm16);
	GrStringDrawCentered(&sContext, "Logic Analyzer", -1,159, 7, 0);


	Rect_t1.i16XMin = 99;
	Rect_t1.i16YMin = 29;
	Rect_t1.i16XMax = 219;
	Rect_t1.i16YMax = 89;

	Rect_t2.i16XMin = 99;
	Rect_t2.i16YMin = 89;
	Rect_t2.i16XMax = 219;
	Rect_t2.i16YMax = 109;

	GrContextFontSet(&sContext, &g_sFontCm14);
	GrStringDrawCentered(&sContext, "Time Period", -1,159, 55 , 0);

	usprintf(str_time, "%3dms",time_interval_step[time_interval_ptr]);
	GrContextForegroundSet(&sContext, ClrBlack);
	GrRectFill(&sContext, &Rect_t2);
	GrContextForegroundSet(&sContext, ClrWhite);
	GrContextFontSet(&sContext, &g_sFontCm14);
	GrStringDrawCentered(&sContext, str_time, -1, 159, 99, 0);

	WidgetAdd(WIDGET_ROOT, (tWidget *)&plus);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&minus);
	WidgetAdd(WIDGET_ROOT, (tWidget *)&start);

	WidgetPaint((tWidget *)&plus);
	WidgetPaint((tWidget *)&minus);
	WidgetPaint((tWidget *)&start);


	while(1)
	{
		 WidgetMessageQueueProcess();
		 if(time_print_flag == 1)
		 {
			 time_print_flag = 0;
			 print_time();
		 }
	}

	return 0;
}


