#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/com.h> //Used for interrupt handling (while loop on timer_test_int)
#include "timer.h"
#include "i8254.h"

static int hook_id = HOOK_ID_BIT; //hook_id is declared as static so that we can access it in other functions
static unsigned int contador = 0;

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

	while ((contador < time * COMPUTER_FREQ) || (cont_temp > 30 * COMPUTER_FREQ)) {
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
					if ((contador % COMPUTER_FREQ) == 0)
						printf("teste %d\n", (contador / COMPUTER_FREQ));
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
	contador++;
	return;
}

int timer_get_conf(unsigned long timer, unsigned char *st) {

	if (timer > 2) {
		printf("Invalid timer input (must be 0, 1 or 2)\n");
		return 1;
	}

	unsigned long var;
	if ((sys_outb(TIMER_CTRL,
	TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer))) != OK) //sys_outb to timer controls (TIMER_CTRL = CTRL PORT!!)
			{
		printf("ERROR ACCESSING CONTROL\n"); //Stops this function from progressing if sys_outb returns an error
		return 2;
	}
	if (sys_inb(TIMER_0 + timer, &var) != OK) {
		printf("sys_inb failed.\n");
		return 3;
	}
	*st = var;
	return 0;
}

int timer_display_conf(unsigned char conf) {

	printf("\nTIMER STATUS BYTE:");
	printf("%5s 0x%02x\n\n", "", conf);

	printf("OUTPUT VALUE:");
	switch (conf & TIMER_RB_CMD) {
	case TIMER_SEL0: {
		printf("%5s 0 \n\n", "");
		break;
	}
	case TIMER_SEL1: {
		printf("%5s 1 \n\n", "");
		break;
	}
	case TIMER_SEL2: {
		printf("%5s 2 \n\n", "");
		break;
	}
	default: {
		printf("%5s NO OUTPUT FOUND. \n\n", "");
		break;
	}
	}

	printf("NULL COUNT:");
	switch (conf & NULL_COUNT) {
	case NULL_COUNT: {
		printf("%5s 1\n\n", "");
		break;
	}
	case 0x0:
		printf("%5s 0\n\n", "");
		break;
	default: {
		printf("%5s NULL COUNT NOT FOUND\n\n", "");
		break;
	}
	}

	printf("ACCESS TYPE:");
	switch (conf & TIMER_LSB_MSB) {
	case TIMER_LSB: {
		printf("%5s LSB \n\n", "");
		break;
	}
	case TIMER_MSB: {
		printf("%5s MSB \n\n", "");
		break;
	}
	case TIMER_LSB_MSB: {
		printf("%5s LSB followed by MSB\n\n", "");
		break;
	}
	default: {
		printf("%5s ACCESS NOT FOUND\n\n", "");
		break;
	}
	}

	printf("PROGRAMMED MODE:");
	switch (conf & TIMER_PROGRAMMED_MODE) {
	case 0x0: {
		printf("%5s 0 -> Interrupt on Terminal Count\n\n", "");
		break;
	}
	case TIMER_1: {
		printf("%5s 1 -> Hardware Retriggerable One-Shot\n\n", "");
		break;
	}
	case TIMER_RATE_GEN: {
		printf("%5s 2 -> Rate Generator\n\n", "");
		break;
	}
	case TIMER_SQR_WAVE: {
		printf("%5s 3 -> Square Wave\n\n", "");
		break;
	}
	case TIMER_MODE_4: {
		printf("%5s 4 -> Software Triggered Strobe\n\n", "");
		break;
	}
	case TIMER_MODE_5: {
		printf("%5s 5 -> Hardware Triggered Strobe (Retriggerable)\n\n", "");
		break;
	}
	default: {
		printf("%5s PROGRAMMED MODE NOT FOUND\n\n", "");
		break;
	}
	}

	printf("BCD:");
	switch (conf & TIMER_BCD) {
	case TIMER_BIN: {
		printf("%5s 0 \n\n", "");
		break;
	}
	case TIMER_BCD: {
		printf("%5s 1 \n\n", "");
		break;
	}
	default: {
		printf("%5s BCD NOT FOUND\n\n", "");
		break;
	}
	}

	return 0;

}

//configures TIMER 0 to generate a square wave (mode 3) with a given frequency (freq), LSB followed by MSB -> control word |00|11|111|0|
int timer_test_square(unsigned long freq) {

	//o divisor nao pode ter + de 16 bits
	if (freq == 0 || (((TIMER_FREQ / freq) >> 16) != 0)) {
		printf("Invalid freq input (must be higher than 18).\n");
		return 1; // valores inválidos de frequencia.
	}

	if (timer_set_square(0, freq) == 0) {
		return 0; //OK value.
	} else {
		printf("timer_set_square failed\n");
		return 2; //something went wrong with set_square
	}
}

int timer_set_square(unsigned long timer, unsigned long freq) {

	if (timer > 2) {
		printf("Invalid timer input (must be 0, 1 or 2)\n");
		return 1;
	}
	//declared timer's bit and port.
	unsigned long bit, port;

	switch (timer) {
	case 0:
		port = TIMER_0;
		bit = TIMER_SEL0;
		break;
	case 1:
		port = TIMER_1;
		bit = TIMER_SEL1;
		break;
	case 2:
		port = TIMER_2;
		bit = TIMER_SEL2;
		break;
	default:
		return 3; // Timer input is wrong!
		break;
	}

	freq = TIMER_FREQ / freq;

	//Binary mode
	if (sys_outb(TIMER_CTRL, TIMER_BIN | bit | TIMER_SQR_WAVE | TIMER_LSB_MSB)
			!= OK) {
		printf("Failed sys_outb to CTRL\n");
		return 4; // Something wrong with sys_outb to ctrl
	}
	if (sys_outb(port, freq) != OK)	//freq in binary / SEND LSB
			{
		printf("Failed sys_outb to LSB\n");
		return 5; //Something wrong with sys_outb LSB to timer port
	}
	if (sys_outb(port, (freq >> 8)) != OK) //freq in binary / SEND MSB
			{
		printf("Failed sys_outb to MSB\n");
		return 6; //Something wrong with sys_outb MSB to timer port
	}
	return 0;
}

int timer_test_config(unsigned long timer) {

	//declared char st
	unsigned char st;

	if (timer_get_conf(timer, &st) != 0) // if timer_get_conf runs with no problem, return = 0
			{
		printf("timer_get_conf failed.\n");
		return 1;		// something went wrong with timer_get_conf
	}
	if (timer_display_conf(st) != 0)//if timer_display_conf runs with no problem, return = 0
			{
		printf("timer_display_conf failed.\n");
		return 2;		// something went wrong with tiemr_display_conf

	}

	return 0;		//everything is correct if return = 0.
}
