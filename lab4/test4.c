#include "test4.h"

static unsigned long packet[3];
static int final_state = 1;

int mouse_test_packet(unsigned short cnt)
{
	if(MOUSE_subscribe_int()<0)
	{
		printf("MOUSE_subscribe_int failed\n");
		return 1;
	}
	//printf("PASSOU 1\n");
	if(write_mouse(STREAM_MODE)>0)
	{
		printf("ERROR SETTING STREAM MODE\n");
		return 2;
	}
	//printf("PASSOU 2\n");
	if(write_mouse(ENABLE)>0)
	{
		printf("ERROR ENABLING STREAM MODE\n");
		return 3;
	}
	//printf("PASSOU 3\n");

	int ipc_status, r = 0, irq_set = BIT(HOOK_ID_BIT_MOUSE);
	message msg;
	int contador = 0;
	int temp;

	while(contador<cnt)//each interruption analizes one packet byte, since each packet has 3 bytes, there will be cnt*3 interruptions to analyze
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
				if (msg.NOTIFY_ARG & irq_set)
				{
					//printf("PASSOU 4\n");
					temp=test_packet_handler();
					if(temp <0)
					{
						printf("ERRO LEITURA\n");
						break;	// ERRO CRITICO
					}
					else if (temp==1)
					{
						//FEZ PRINT DE UM PACKET, LOGO, AUMENTA-SE CONTADOR PARA +1
						contador++;
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

	if(write_mouse(DISABLE)>0)
	{
		printf("ERROR SETTING STREAM MODE\n");
		return 4;
	}
	if(MOUSE_unsubscribe_int()>0)
	{
		printf("MOUSE_unsubscribe_int failed\n");
		return 5;
	}

	return 0;
}

int mouse_test_async(unsigned short idle_time) {
	if(MOUSE_subscribe_int()<0)
	{
		printf("MOUSE_subscribe_int failed\n");
		return 1;
	}
	if (TIMER_subscribe_int()<0)
	{
		printf("timer_subscribe_int failed\n");
		return 2;
	}
	if(write_mouse(STREAM_MODE)>0)
	{
		printf("ERROR SETTING STREAM MODE\n");
		return 3;
	}
	if(write_mouse(ENABLE)>0)
	{
		printf("ERROR SETTING STREAM MODE\n");
		return 4;
	}

	int ipc_status, r = 0, irq_set = BIT(HOOK_ID_BIT_MOUSE),irq_set_timer = BIT(HOOK_ID_BIT_TIMER);
	message msg;
	int contador = 0,temp;

	while(contador< idle_time * COMPUTER_FREQ)//each interruption analizes one packet byte, since each packet has 3 bytes, there will be cnt*3 interruptions to analyze
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
				if (msg.NOTIFY_ARG & irq_set)
				{
					contador = 0;
					temp=test_packet_handler();
					if(temp <0)
					{
						printf("ERRO LEITURA\n");
						break;	// ERRO CRITICO
					}
					else if (temp==1)
					{
						contador = 0;
					}
				}
				if (msg.NOTIFY_ARG & irq_set_timer)
				{
					contador ++;
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
	printf("ACABOU O CICLO, PASSOU TODO O IDLE_TIME\n");

	if (TIMER_unsubscribe_int() != 0)
	{
		printf("timer_unsubscribe_int failed.\n");
		return 5;
	}
	if(write_mouse(DISABLE)>0)
	{
		printf("ERROR SETTING STREAM MODE\n");
		return 6;
	}
	if(MOUSE_unsubscribe_int()>0)
	{
		printf("MOUSE_unsubscribe_int failed\n");
		return 7;
	}

	return 0;
}

int mouse_test_config(void) {

	if(MOUSE_subscribe_int()<0)
	{
		printf("MOUSE_subscribe_int failed\n");
		return 1;
	}
	if(write_mouse(DISABLE)>0)
	{
		printf("ERROR SETTING STREAM MODE\n");
		return 2;
	}
	if(write_mouse(CONFIG) > 0)
	{
		printf("ERROR REQUESTING STATUS\n");
		return 3;
	}


	if(test_config_handler() <0)
	{
		printf("ERROR READING STATUS\n");
	}


	if(MOUSE_unsubscribe_int()>0)
	{
		printf("MOUSE_unsubscribe_int failed\n");
		return 3;
	}

	return 0;
}	

int mouse_test_gesture(short length) {
	if(MOUSE_subscribe_int()<0)
	{
		printf("MOUSE_subscribe_int failed\n");
		return 1;
	}
	if(write_mouse(STREAM_MODE)>0)
	{
		printf("ERROR SETTING STREAM MODE\n");
		return 2;
	}
	if(write_mouse(ENABLE)>0)
	{
		printf("ERROR SETTING STREAM MODE\n");
		return 3;
	}

	int ipc_status, r = 0, irq_set = BIT(HOOK_ID_BIT_MOUSE);
	message msg;
	int contador = 0;
	int temp;
	short distance_travelled_y = 0;

	while(final_state!=0)//each interruption analizes one packet byte, since each packet has 3 bytes, there will be cnt*3 interruptions to analyze
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
					temp=test_packet_handler();
					if(temp==1)	// OK
					{
						analyze_movement(&distance_travelled_y, length);
						declare_event();
						final_state=test_gesture_handler();
					}
					else if (temp<0)
					{
						printf("ERRO LEITURA\n");
						break;	// ERRO CRITICO
					}
				}
				else
				{
					//this ELSE is used to handle an FA that always appears has the first byte of the first packet
					//and that should not happen, so we create this else for that FA to be ignored and not be read has a packet byte.
					//NOTE: this FA, if not ignored, will appear only and always in the first byte of the first packet
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

	if(write_mouse(DISABLE)>0)
	{
		printf("ERROR SETTING STREAM MODE\n");
		return 4;
	}
	if(MOUSE_unsubscribe_int()>0)
	{
		printf("MOUSE_unsubscribe_int failed\n");
		return 5;
	}

	return 0;
}
