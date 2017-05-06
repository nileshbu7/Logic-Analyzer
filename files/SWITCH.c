

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/pwm.h"
#include "driverlib/systick.h"
#include "inc/hw_nvic.h"
#include "inc/hw_gpio.h"
#include "driverlib/adc.h"

#include "SWITCH.h"

void Switch_Init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	/*GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4, GPIO_RISING_EDGE);
	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);
	IntEnable(INT_GPIOF);
	GPIOIntRegister(GPIO_PORTF_BASE, PortFIntHandler);*/

}

bool Switch_1_pressed(void)
{
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) != 0b00010000)
		return(true);
	else
		return(false);
}

bool Switch_2_pressed(void)
{
	if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0) != 0b00000001)
		return(true);
	else
		return(false);
}

void PortFIntHandler(void)
{
	uint32_t status = GPIOIntStatus(GPIO_PORTF_BASE, true);
	if(status == GPIO_INT_PIN_4)
	{
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);
		UARTprintf("Switch_1_pressed\n");
	}
	if(status == GPIO_INT_PIN_0)
	{
		GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0);
		UARTprintf("Switch_2_pressed\n");
	}
}


