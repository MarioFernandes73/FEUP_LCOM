#ifndef __TIMER_H
#define __TIMER_H

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h> //Used to emasure time
#include <minix/com.h> //Used for interrupt handling (while loop)

#define HOOK_ID_BIT_TIMER 	2
#define TIMER_IRQ 			0

int TIMER_subscribe_int();

int TIMER_unsubscribe_int();


#endif
