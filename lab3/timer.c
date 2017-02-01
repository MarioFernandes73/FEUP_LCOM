#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h> //Used for interrupt handling (while loop on timer_test_int)
#include "timer.h"
#include "i8254.h"

static int hook_id = HOOK_ID_BIT; //hook_id is declared as static so that we can access it in other functions
unsigned int counter = 0;

int timer_test_int(unsigned long time) {

	if (time == 0) {
		printf("Invalid time input.\n");
		return 1;
	}
	//Issue 1: How does the GIH know that a DD is interested in an interrupt? -> subscribe the timer with
	//an interrupt notification -> timer_subscribe_int()

	if (timer_subscribe_int() < 0)//it must be >= 0 because subscribe returns bit order in interrupt mask (irq_set)
			{
		printf("timer_subscribe_int failed\n");
		return 2;	// timer_subscribe_int failed
	}
	int ipc_status, r = 0, irq_set = BIT(HOOK_ID_BIT);//BIT do hook_id inicial = 0
	message msg;
	unsigned int cont_temp = 0;	//cont_temp

	while ((counter < time * COMPUTER_FREQ) || (cont_temp > 30 * COMPUTER_FREQ)) {
		//driver_receive -> receives any message sent to the DD
		//1st argument(ANY) -> message sender (in this case we be looking for any sender)
		//2nd argument(&msg) -> where the message will be written
		//3rd argument(&ipc_status) -> where the ipc status will be written (ipc_status will contain the message status, it can be either a standard message or a kernel notification
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed\n");
			break;
		}

		//is_ipc_notify -> returns true if the message(ipc_status) was a notification, returns false otherwise
		if (is_ipc_notify(ipc_status)) {
			//msg.m_source -> sender of the message's endpoint (an address with information regarding communication endpoints, i.e. the source and destination of a communication)
			//_ENDPOINT_P -> extracts the procces identifier from a process endpoint
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: // hardware interrupt notification
			{
				// msg.NOTIFY_ARG -> a 32-bit bitmask with the bits of the active interrupts subscribed set to 1
				//irq_set -> should be a bitmask with the bits corresponding to the hook_id values (HOOK_ID_BIT)
				if (msg.NOTIFY_ARG & irq_set) {
					if ((counter % COMPUTER_FREQ) == 0)
						//printf("teste %d\n", (counter / COMPUTER_FREQ)); commented because we don't want it to print a message every second
					timer_int_handler();
					/* call the corresponding interrupt handler */
				}
				break;
			}
			default: {
				cont_temp++; // para que nao haja loops infinitos
				break; /* no other notifications expected: do nothing */
			}

			}
		} else {
			cont_temp++; // para que nao haja loops infinitos
		}/* received a standard message, not a notification */
		/* no standard messages expected: do nothing */
	}

	if (timer_unsubscribe_int() == 0) {
		return 0;
	} else {
		printf("timer_unsubscribe_int failed.\n");
		return 3; //timer_unsubscribe failed
	}

}

int timer_subscribe_int(void) {

	//TIMER0_IRQ -> variable in i8254 -> irq line of input.
	//IRQ_REENABLE -> policy to inform the GIH that it can give EOI command.
	//hook_id -> input and output (that's why we need the &)(already initialized globally)
	if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK) {
		printf("sys_irqsetpolicy failed\n");
		return -1;	// sys_irqsetpolicy failed
	}
	//Issue 5: What if the GIH does not send the EOI? -> The DD will have to do it by calling sys_irqenable
	if (sys_irqenable(&hook_id) != OK) {
		printf("sys_irqenable failed\n");
		return -2;	//sys_irqenable failed
	}
	return HOOK_ID_BIT; //returns initial bit from hook_id (irq_set).

}

int timer_unsubscribe_int() {

	//hook_id already initialized globally (has been through timer_subscribe_int)
	if (sys_irqdisable(&hook_id) != OK)	//irqdisable desativa interrupcoes associadas na IRQ Line associada ao hook_id
			{
		printf("sys_irqdisable failed\n");
		return 1;
	}
	if (sys_irqrmpolicy(&hook_id) != OK) //irqrmpolicy retira a subscricao às notificacoes de interrupcoes associadas ao hook_id
			{
		printf("sys_rmpolicy failed\n");
		return 2;
	}
	return 0;
}

void timer_int_handler() {
	counter++;
	return;
}
