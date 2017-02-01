#include "test3.h"


static int isTwoBytes = 0;
static unsigned int contador = 0;


int kbd_test_scan(unsigned short ass) {
	
	if (KBC_subscribe_int() < 0)//it must be >= 0 because subscribe returns bit order in interrupt mask (irq_set)
	{
			printf("KBC_subscribe_int failed\n");
			return 1;	// KBC_subscribe_int failed
	}
	
	int escape = 0;
	int ipc_status, r = 0, irq_set = BIT(HOOK_ID_BIT_KBD);//BIT do hook_id inicial = 0
	message msg;
	
	
	while(!escape)
	{
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
					if(ass == 0)
					{
						unsigned long key_pressed;
						if(read_out_buf(SCN_CODE_REG,&key_pressed)==0)
						if(kbd_handler_c(key_pressed)==0)
						escape=1;
					}
					else
					{
						//if(kbd_handler_ass()=1)
						//if(kbd_handler_c(key)==0);
						escape=1;
					}
				}
				break;
			}
			default: {
				break; /* no other notifications expected: do nothing */
			}

			}
		} else {
		}/* received a standard message, not a notification */
		/* no standard messages expected: do nothing */
	}
	
	
	if(KBC_unsubscribe_int() < 0)
	{
		printf("KBC_unsubscribe_int failed\n");
		return 1;	// KBC_unsubscribe_int failed
	}
	return 0;
}

int kbd_test_leds(unsigned short n, unsigned short *leds)
{

	if (timer_subscribe_int() < 0)//it must be >= 0 because subscribe returns bit order in interrupt mask (irq_set)
				{
			printf("timer_subscribe_int failed\n");
			return 1;	// timer_subscribe_int failed
		}

	unsigned short iterador = 0;// irá estar a iterar sobre o array leds para accionar 1 a 1 (necessita de atualizacao)

	int escape = 0;
	int ipc_status, r = 0, irq_set = BIT(HOOK_ID_BIT) ;
	message msg;
	contador = 0;

	while (iterador < n)	// o iterador atualiza a cada segundo, (comecando em 0) quando chegar ao numero de elementos, o ciclo termina
	{
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
					{
						//entra aqui num intervalo de 1 segundo
						if(change_led(iterador, leds)!=0)
						{
							return 2;
						}
						iterador++;
					}
					contador++;
				}
				break;
			}
			default: {
				break; /* no other notifications expected: do nothing */
			}

			}
		} else {
		}/* received a standard message, not a notification */
		/* no standard messages expected: do nothing */
	}
	
		if (timer_unsubscribe_int() == 0)
		{
			printf("successfuly went through all led instructions\n");
			return 0;//successfuly went through all led instructions
			}
		else
		{
				printf("timer_unsubscribe_int failed.\n");
				return 3; //timer_unsubscribe failed
		}


}
int kbd_test_timed_scan(unsigned short n){

	if (KBC_subscribe_int() < 0)//it must be >= 0 because subscribe returns bit order in interrupt mask (irq_set)
	{
			printf("KBC_subscribe_int failed\n");
			return 1;	// KBC_subscribe_int failed
	}
	if (timer_subscribe_int() < 0)//it must be >= 0 because subscribe returns bit order in interrupt mask (irq_set)
			{
		printf("timer_subscribe_int failed\n");
		return 2;	// timer_subscribe_int failed
	}

	int escape = 0;
	int ipc_status, r = 0, irq_set1 = BIT(HOOK_ID_BIT_KBD), irq_set2 = BIT(HOOK_ID_BIT) ;
	message msg;
	contador = 0;


	while(!escape && contador < n*COMPUTER_FREQ)
	{
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
				if (msg.NOTIFY_ARG & irq_set1)
					{
						contador=0;
						unsigned long key_pressed;
						if(read_out_buf(SCN_CODE_REG,&key_pressed)==0)
						if(kbd_handler_c(key_pressed)==0)
						escape=1;

					}
				if (msg.NOTIFY_ARG & irq_set2)
				{
					contador++;			//colocar numa funcao???
				}
				break;
			}
			default: {
				break; /* no other notifications expected: do nothing */
			}

			}
		} else {
		}/* received a standard message, not a notification */
		/* no standard messages expected: do nothing */
	}

if (timer_unsubscribe_int() == 0) {
		return 0;
	} else {
		printf("timer_unsubscribe_int failed.\n");
		return 3; //timer_unsubscribe failed
	}


	if(KBC_unsubscribe_int() < 0)
	{
		printf("KBC_unsubscribe_int failed\n");
		return 1;	// KBC_unsubscribe_int failed
	}
	return 0;
}
