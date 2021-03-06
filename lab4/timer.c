#include "timer.h"

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

	//hook_id already initialized globally (has been through timer_subscribe_int)
	if (sys_irqdisable(&hook_id_timer) != OK)	//irqdisable desativa interrupcoes associadas na IRQ Line associada ao hook_id
			{
		printf("sys_irqdisable failed\n");
		return 1;
	}
	if (sys_irqrmpolicy(&hook_id_timer) != OK) //irqrmpolicy retira a subscricao �s notificacoes de interrupcoes associadas ao hook_id
			{
		printf("sys_rmpolicy failed\n");
		return 2;
	}
	return 0;
}
