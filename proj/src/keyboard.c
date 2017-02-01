#include "keyboard.h"
#include "kbc.h"
#include "gamevariables.h"

static int hook_id;


int KBD_subscribe_int() {

	hook_id = HOOK_ID_BIT_KBD;

	//KBC_IRQ -> variable in keyboard.h -> irq line of input.
	//IRQ_REENABLE -> policy to inform the GIH that it can give EOI command.
	//hook_id -> input and output (that's why we need the &)(already initialized globally)
	if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id)
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

int KBD_unsubscribe_int() {

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


int KBD_handler()
{
	unsigned long key;
	if(read_status_to_read()==0)
	sys_inb(OUT_BUF, &key);
	else
	{
		printf("ERRO A LER DO OUT_BUF\n");
		return -1;
	}
	if ((key & BREAKCODE) == BREAKCODE)
	{
		if(key == ESC)
		return ESC_KEY;
		if(key == Akey)
		return A_KEY;
		if(key  == Wkey)
		return W_KEY;
		if(key  == Skey)
		return S_KEY;
		if(key  == Dkey)
		return D_KEY;
	}
	return INVALID_INPUT;//nao foi A,W,S OU D (input invalido)
}
