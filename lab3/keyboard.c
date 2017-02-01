#include "keyboard.h"

static int hook_id = HOOK_ID_BIT_KBD;
static int has_first_byte = 0;
static int TRIES = 50;
static unsigned short led_a_acionar; //elemento atual do vetor de leds que esta a ser acionado
static unsigned short led_bits = 0;

int kbd_handler_c(unsigned long keypressed) {

	if ((keypressed & BREAKCODE) == BREAKCODE) {
		//Breakcode:
		if (has_first_byte == 1) {
			has_first_byte = 0;
			printf("Breakcode: 0xE0%X\n", keypressed);
			return 1;
		} else {
			if ((keypressed & FIRST_BYTE) == FIRST_BYTE) {
				has_first_byte = 1;
				return 2;
			} else {

				printf("Breakcode: 0x%X\n", keypressed);
				if ((keypressed == ESC) && has_first_byte == 0)
					return 0;
				else {
					has_first_byte = 0;
					return 1;
				}
			}
		}
	} else {
		//Makecode:
		if (has_first_byte) {
			has_first_byte = 0;
			printf("Scancode: 0xE0%X\n", keypressed);
			return 1;
		} else {
			if ((keypressed & FIRST_BYTE) == FIRST_BYTE) {
				has_first_byte = 1;
				return 2;
			} else {
				has_first_byte = 0;
				printf("Scancode: 0x%X\n", keypressed);
				return 1;
			}
		}
	}
}

int KBC_subscribe_int(void) {

	//KBC_IRQ -> variable in test3.h -> irq line of input.
	//IRQ_REENABLE -> policy to inform the GIH that it can give EOI command.
	//hook_id -> input and output (that's why we need the &)(already initialized globally)
	if (sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id)
			!= OK) {
		printf("sys_irqsetpolicy failed\n");
		return -1;	// sys_irqsetpolicy failed
	}
	//Issue 5: What if the GIH does not send the EOI? -> The DD will have to do it by calling sys_irqenable
	if (sys_irqenable(&hook_id) != OK) {
		printf("sys_irqenable failed\n");
		return -2;	//sys_irqenable failed
	}
	return HOOK_ID_BIT_KBD; //returns initial bit from hook_id (irq_set).

}

int KBC_unsubscribe_int() {

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

int read_out_buf(unsigned long port, unsigned long *read) { //port-> port where we should read from(in hexadecimal)    *read->argument where will be saved what has been read

	wait_until_out_buf_full();
	if (sys_inb(port, read) != OK) {
		printf("ERROR READING FROM OUTPUT BUFFER\n");
		return 1;
	}
	return 0; //Return 0 when it reads successfuly from output buffer
}

int write_in_buf(unsigned long port, unsigned long write) { //port-> port where we should write to(in hexadecimal)    write->what should be written
	wait_until_in_buf_empty();
	if (sys_outb(port, write) != OK) {
		printf("ERROR WRITING TO INPUT BUFFER\n");
		return 1;
	}
	return 0; //Return 0 when it writes successfuly in input buffer
}

int check_if_in_buf_empty() {
	unsigned long status;

	if (sys_inb(STAT_REG, &status) != OK) {
		printf("ERROR READING STATUS REGISTER\n");
		return 1;
	}
	if ((status & IBF) == 1) {
		//INPUT BUFFER FULL
		return 2;
	}
	return 0; //Return if input buffer is empty
}

int check_if_out_buf_full() {
	unsigned long status;

	if (sys_inb(STAT_REG, &status) != OK) {
		printf("ERROR READING STATUS REGISTER\n");
		return 1;
	}
	if ((status & OBF) == 0) {
		//OUTPUT BUFFER EMPTY
		return 2;
	}
	return 0; //Return if output buffer is full
}

int wait_until_in_buf_empty() {
	int cont = 0;
	while (check_if_in_buf_empty() != 0 && cont < TRIES)/*loop while input buffer is full*/
	{
		tickdelay(micros_to_ticks(DELAY_US));
		cont++;
	}

	if (cont == TRIES) {
		printf("Waited 50 loops for input buffer to become empty\n");
		return 1;
	}

	return 0; //Returns zero only when input buffer is empty
}

int wait_until_out_buf_full() {
	int cont = 0;
	while (check_if_out_buf_full() != 0 && cont < TRIES)/*loop while input buffer is full*/
	{
		tickdelay(micros_to_ticks(DELAY_US));
		cont++;
	}

	if (cont == TRIES) {
		printf("Waited 50 loops for output buffer to become full\n");
		return 1;
	}

	return 0; //Returns zero only when output buffer has info in it
}

int change_led(unsigned short iterador, unsigned short *leds) {
	unsigned long KBDresponse;
	led_a_acionar = iterador;
	if (send_led_command(&KBDresponse) != 0) {
		return 1;
	}
	//the following code is commented because it only works if the keybord sends a response to the output buffer after receiving a LED command or argument
	if (KBDresponse == ACK) {
		if (led_command_response_ACK(leds, &KBDresponse) != 0) {
			printf("led_command_response_ACK failed \n");
			return 3;
		}
	} else if (KBDresponse == RESEND) {
		if (led_command_response_RESEND(leds, &KBDresponse) != 0) {
			return 4;
		}
	} else if (KBDresponse == ERROR) {
		printf("keyboard responded with ERROR\n");
		return 5;
	} else {
		printf("KBDresponse failed\n");
		return 6;
	}
	if (led_bits & BIT(*(leds+led_a_acionar)) == BIT(*(leds + led_a_acionar))) {
		led_bits = 0;
	} else {
		led_bits = BIT(*(leds + led_a_acionar));
	}

	if (send_led_argument(led_bits, &KBDresponse) != 0) {
		return 2;
	}
	return 0;
}

int send_led_command(unsigned long *KBDresponse) {
	writeinteste(LEDs);
	//the following code is commented because it only works if the keybord sends a response to the output buffer after receiving a LED command or argument
	*KBDresponse = readinteste();

	return 0;
}

int send_led_argument(unsigned short argument, unsigned long *KBDresponse) {
	if (writeinteste(argument) != 0) {
		printf("ERROR SENDING LED COMMAND ARGUMENTS\n");
		return 1;
	}
	//the following code is commented because it only works if the keybord sends a response to the output buffer after receiving a LED command or argument
	*KBDresponse=readinteste();
	return 0;
}

//the above functions aren't used, they would be used only if the keybord sent a response to the output buffer after receiving a LED command or argument
int led_command_response_ACK(unsigned short *leds, unsigned long *KBDresponse) {
	send_led_argument(BIT(*(leds + led_a_acionar)), KBDresponse);
	if (*KBDresponse == ACK) {
		return 0; //LED successfully changed state
	} else if (*KBDresponse == RESEND) {
		if (led_argument_response_RESEND(leds, KBDresponse) != 0) {
			return 1;
		}
	} else if (*KBDresponse == ERROR) {
		printf("keyboard responded with ERROR\n");
		return 2;
	} else
		return 4;
}

int led_command_response_RESEND(unsigned short *leds,
		unsigned long *KBDresponse) {
	unsigned int cont = 0;
	while (*KBDresponse == RESEND && cont < TRIES) {
		send_led_command(KBDresponse);
		cont++;
	}
	if (cont == TRIES) {
		printf("ERROR CHANGING LED STATE, IT ENTERED IN A RESEND LOOP\n");
		return 1;
	}
	if (*KBDresponse == ACK) {
		led_command_response_ACK(leds, KBDresponse);
	}
	if (*KBDresponse == ERROR) {
		printf("keyboard responded with ERROR\n");
		return 2;
	}
}

int led_argument_response_RESEND(unsigned short *leds,
		unsigned long *KBDresponse) {
	unsigned int cont = 0;
	while (*KBDresponse == RESEND && cont < TRIES) {
		send_led_argument(BIT(*(leds + led_a_acionar)), KBDresponse);
		cont++;
	}
	if (cont == TRIES) {
		printf("ERROR CHANGING LED STATE, IT ENTERED IN A RESEND LOOP");
		return 1;
	}
	if (*KBDresponse == ACK) {
		return 0; //LED successfuly changed state
	} else if (*KBDresponse == ERROR) {
		printf("keyboard responded with ERROR\n");
		return 3;
	} else {
		printf("KBDresponse failed\n");
		return 4;
	}
}

unsigned long readinteste(){

 unsigned int contador = 0;


while( contador<TRIES )
{
unsigned long stat, data;

sys_inb(STAT_REG, &stat); /* assuming it returns OK */
/* loop while 8042 output buffer is empty */
if( stat & OBF ) {
sys_inb(OUT_BUF, &data); /* assuming it returns OK */
if ( (stat &(PAR_ERR | TO_ERR)) == 0 )
return data;
else
return 1;
}
tickdelay(micros_to_ticks(DELAY_US));
contador++;
}
printf("erro tries \n");
return 2;
}

unsigned long writeinteste(unsigned long cmd)
{

	 unsigned int contador = 0;
	 unsigned long stat;
	while( contador < TRIES) {
	sys_inb(STAT_REG, &stat); /* assuming it returns OK */
	/* loop while 8042 input buffer is not empty */
	if( (stat & IBF) == 0 ) {
	sys_outb(KBC_CMD_REG, cmd); /* no args command */
	return 0;
	}
	tickdelay(micros_to_ticks(DELAY_US));
	contador++;
	}
	printf("erro a escrever");
	return 1;
}

int send_command(unsigned long command, unsigned long argument){
	unsigned long response;
	writeinteste(command);
	response=readinteste();
	if(response==ACK)
	{
		writeinteste(argument);
		response=readinteste();

		if(response==ACK)
		return 0;
		else
		{
			printf("response NOT ACK\n1");
			return -1;
		}

	}
	printf("response NOT ACK\n1");
	return -1;
}
