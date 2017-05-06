#include "Delay.h"

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
#include "driverlib/pwm.h"
#include "driverlib/systick.h"

void ms_delay(uint32_t delay)
{
	SysCtlDelay(((SysCtlClockGet())/3000)*delay);
}

void us_delay(uint32_t delay)
{
	SysCtlDelay(((SysCtlClockGet())/3000000)*delay);
}
