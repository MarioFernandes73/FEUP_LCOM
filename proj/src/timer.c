#include "timer.h"
#include "gamevariables.h"

static int hook_id_timer = HOOK_ID_BIT_TIMER;

int TIMER_subscribe_int() {

	//TIMER_IRQ -> variable in mouse.h -> irq line of input.
	//IRQ_REENABLE -> policy to inform the GIH that it can give EOI command.
	//hook_id -> input and output (that's why we need the &)(already initialized globally)
	if (sys_irqsetpolicy(TIMER_IRQ, IRQ_REENABLE, &hook_id_timer) != OK) {
		printf("sys_irqsetpolicy failed\n");
		return -1;	// sys_irqsetpolicy failed
	}
	if (sys_irqenable(&hook_id_timer) != OK) {
		printf("sys_irqenable failed\n");
		return -2;	//sys_irqenable failed
	}
	return HOOK_ID_BIT_TIMER; //returns initial bit from hook_id (irq_set).
}

int TIMER_unsubscribe_int() {

	if (sys_irqrmpolicy(&hook_id_timer) != OK) //irqrmpolicy retira a subscricao às notificacoes de interrupcoes associadas ao hook_id
	{
		printf("sys_rmpolicy failed\n");
		return 2;
	}
	return 0;
}

void timer_handler(unsigned int *timer_counter, unsigned int *time)
{
	*timer_counter += 1;
	if((*timer_counter % COMPUTER_FREQ) == 0)
	{
		*time += 1;
	}
}
