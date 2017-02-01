#ifndef __TIMER_H
#define __TIMER_H

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h> //Used to emasure time
#include <minix/com.h> //Used for interrupt handling (while loop)

#define BIT(n) (0x01<<(n))
#define HOOK_ID_BIT_TIMER 	0
#define COMPUTER_FREQ		60
#define TIMER_IRQ 			0


/**
 * @brief Subscribes and enables timer interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int TIMER_subscribe_int();

/**
 * @brief The game's timer interrupt handler
 *
 * @param timer_counter is increased by one in each timer interrupt
 * @param time is increased by one every second
 */
void timer_handler(unsigned int *timer_counter, unsigned int *time);

/**
 * @brief Unsubscribes the timer interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int TIMER_unsubscribe_int();


#endif
