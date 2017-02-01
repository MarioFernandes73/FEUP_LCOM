#include "test5.h"
#include "vbe.h"
#include "xpm.h"
#include "sprite.h"

void *test_init(unsigned short mode, unsigned short delay) {

	if (TIMER_subscribe_int() < 0) {
		printf("timer_subscribe_int failed\n");
		return NULL;
	}
	void* virtual_adress = vg_init(mode);
	if (virtual_adress == NULL) {
		printf("ERROR INITIALIZING GRAPGHICS MODE \n");
		return NULL;
	}
	int ipc_status, r = 0, irq_set_timer = BIT(HOOK_ID_BIT_TIMER);
	message msg;
	int contador = 0;
	while (contador < delay * COMPUTER_FREQ) //each interruption analizes one packet byte, since each packet has 3 bytes, there will be cnt*3 interruptions to analyze
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
				if (msg.NOTIFY_ARG & irq_set_timer) {
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

	if (vg_exit() != 0) {
		printf("ERROR CHANGING BACK TO TEXT MODE\n");
		return NULL;
	}
	if (TIMER_unsubscribe_int() != 0) {
		printf("timer_unsubscribe_int failed.\n");
		return NULL;
	}
	printf("virtual adress: %d", virtual_adress);//para avaliar o que acontece
	return virtual_adress;
}

int test_square(unsigned short x, unsigned short y, unsigned short size,
		unsigned long color) {

	if (KBD_subscribe_int() < 0)//it must be >= 0 because subscribe returns bit order in interrupt mask (irq_set)
			{
		printf("KBD_subscribe_int failed\n");
		return 1;	// KBD_subscribe_int failed
	}
	char* virtual_address;
	virtual_address = vg_init(RESOLUTION_768p);	//mode 105
	if (virtual_address == NULL) {
		printf("ERROR INITIALIZING GRAPGHICS MODE \n");
		return 2;
	}
	int escape = 0;
	int ipc_status, r = 0, irq_set_kbd = BIT(HOOK_ID_BIT_KBD);
	message msg;

	vg_draw_rectangle(virtual_address, x, y, size, size, color);

	while (escape == 0) {
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
				if (msg.NOTIFY_ARG & irq_set_kbd) {
					//verify if it's the escape key
					if (KBD_handler_square() == 1) {
						escape = 1;
						break;
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
	if (vg_exit() != 0) {
		printf("ERROR CHANGING BACK TO TEXT MODE\n");
		return 3;
	}
	if (KBD_unsubscribe_int() < 0) {
		printf("KBD_unsubscribe_int failed\n");
		return 4;	// KBD_unsubscribe_int failed
	}

	return 0;

}

int test_line(unsigned short xi, unsigned short yi, unsigned short xf,
		unsigned short yf, unsigned long color) {

	if (KBD_subscribe_int() < 0)//it must be >= 0 because subscribe returns bit order in interrupt mask (irq_set)
			{
		printf("KBD_subscribe_int failed\n");
		return 1;	// KBD_subscribe_int failed
	}
	char* virtual_address;
	virtual_address = vg_init(RESOLUTION_768p);	//mode 105
	if (virtual_address == NULL) {
		printf("ERROR INITIALIZING GRAPGHICS MODE \n");
		return 2;
	}
	int escape = 0;
	int ipc_status, r = 0, irq_set_kbd = BIT(HOOK_ID_BIT_KBD);
	message msg;

	vg_draw_line(virtual_address, xi, yi, xf, yf, color);

	while (escape == 0) {
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
				if (msg.NOTIFY_ARG & irq_set_kbd) {
					//verify if it's the escape key
					if (KBD_handler_square() == 1) {
						escape = 1;
						break;
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
	if (vg_exit() != 0) {
		printf("ERROR CHANGING BACK TO TEXT MODE\n");
		return 3;
	}
	if (KBD_unsubscribe_int() < 0) {
		printf("KBD_unsubscribe_int failed\n");
		return 4;	// KBD_unsubscribe_int failed
	}

	return 0;

}

int test_xpm(unsigned short xi, unsigned short yi, char *xpm[]) {

	if (KBD_subscribe_int() < 0)//it must be >= 0 because subscribe returns bit order in interrupt mask (irq_set)
			{
		printf("KBD_subscribe_int failed\n");
		return 1;	// KBD_subscribe_int failed
	}
	char* virtual_address;
	virtual_address = vg_init(RESOLUTION_768p);	//mode 105
	if (virtual_address == NULL) {
		printf("ERROR INITIALIZING GRAPGHICS MODE \n");
		return 2;
	}
	int escape = 0;
	int ipc_status, r = 0, irq_set_kbd = BIT(HOOK_ID_BIT_KBD);
	message msg;

	vbe_mode_info_t temp;

	vbe_get_mode_info(RESOLUTION_768p, &temp);

	int xpmWidth, xpmHeight;
	char * pixmap = read_xpm(xpm, &xpmWidth, &xpmHeight, temp.XResolution,
			temp.YResolution);
	vg_draw_xpm(virtual_address, xi, yi, xpmWidth, xpmHeight, pixmap);

	while (escape == 0) {
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
				if (msg.NOTIFY_ARG & irq_set_kbd) {
					//verify if it's the escape key
					if (KBD_handler_square() == 1) {
						escape = 1;
						break;
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

	if (vg_exit() != 0) {
		printf("ERROR CHANGING BACK TO TEXT MODE\n");
		return 3;
	}
	if (KBD_unsubscribe_int() < 0) {
		printf("KBD_unsubscribe_int failed\n");
		return 4;	// KBD_unsubscribe_int failed
	}

	return 0;

}

int test_move(unsigned short xi, unsigned short yi, char *xpm[],
		unsigned short hor, short delta, unsigned short time) {

	if (TIMER_subscribe_int() < 0) {
		printf("timer_subscribe_int failed\n");
		return 1;
	}
	if (KBD_subscribe_int() < 0)//it must be >= 0 because subscribe returns bit order in interrupt mask (irq_set)
			{
		printf("KBD_subscribe_int failed\n");
		return 1;	// KBD_subscribe_int failed
	}
	void* virtual_address;
	virtual_address = vg_init(RESOLUTION_768p);	//mode 105
	if (virtual_address == NULL) {
		printf("ERROR INITIALIZING GRAPGHICS MODE \n");
		return 2;
	}

	vbe_mode_info_t temp;

	vbe_get_mode_info(RESOLUTION_768p, &temp);

	Sprite *sprite;
	double speed = (double) (delta) / (time * 60);

	printf("speed:%d %d %d\n",delta, time,speed);

	 if(hor==0)
	sprite= create_sprite(xpm, xi, yi, speed, 0, temp.XResolution, temp.YResolution);
	 else
	 sprite= create_sprite(xpm, xi, yi, 0, speed, temp.XResolution, temp.YResolution);


	int ipc_status, r = 0, irq_set_timer = BIT(HOOK_ID_BIT_TIMER), irq_set_kbd =
			BIT(HOOK_ID_BIT_KBD);
	message msg;
	int contador = 0, escape = 0;
	while (escape == 0)	//each interruption analizes one packet byte, since each packet has 3 bytes, there will be cnt*3 interruptions to analyze
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
				if (msg.NOTIFY_ARG & irq_set_timer) {
					if (contador <= COMPUTER_FREQ * time) {

						animate_sprite(sprite);
						draw_sprite(sprite, virtual_address);
						contador++;
					}
				}
			if (msg.NOTIFY_ARG & irq_set_kbd) {
				//verify if it's the escape key
				if(KBD_handler_square()==1)
				{
					escape=1;
					break;
				}
			}
		}
		default: {
			break; /* no other notifications expected: do nothing */
		}

			}
		} else {
		}/* received a standard message, not a notification */
		/* no standard messages expected: do nothing */
	}

	//	destroy_sprite(sprite);

	if (KBD_unsubscribe_int() < 0) {
		printf("KBD_unsubscribe_int failed\n");
		return 5;	// KBD_unsubscribe_int failed
	}

	if (TIMER_unsubscribe_int() != 0) {
		printf("timer_unsubscribe_int failed.\n");
		return 6;
	}
	if (vg_exit() != 0) {
		printf("ERROR CHANGING BACK TO TEXT MODE\n");
		return 4;
	}

	return 0;

}

int test_controller() {

	vbe_controller_info_t VBE_controller_info;
		if(vbe_get_controller_info( &VBE_controller_info)!=1)//se retornar diferente de 1 significa que a informacao nao foi obtida com sucesso
			return 1;
		/*-------------PRINT CONTROLLER INFO----------------------------------*/
		printf("CAPABILITIES: 0x%02X \n", VBE_controller_info.Capabilities);

		printf("CAPABILITIES DAC BIT : ");
		if( (VBE_controller_info.Capabilities & DAC_SWITCHABLE) == DAC_SWITCHABLE)
			printf("%d  -->  DAC width is switchable to 8 bits per primary color \n", 1);
		else
			printf("%d  -->  DAC is fixed width, with 6 bits per primary color \n", 0);

		printf("CAPABILITIES VGA BIT : ");
		if( (VBE_controller_info.Capabilities & VGA_INCOMPATIBLE) == VGA_INCOMPATIBLE)
			printf("%d  -->  Controller is not VGA compatible \n", 1);
		else
			printf("%d  -->  Controller is VGA compatible \n", 0);

		printf("CAPABILITIES RAMDAC BIT : ");
		if( (VBE_controller_info.Capabilities & NOT_NORMAL_RAMDAC) == NOT_NORMAL_RAMDAC)
			printf("%d  -->  When programming large blocks of information to the RAMDAC, use the blank bit in Function 09h \n\n", 1);
		else
			printf("%d  -->  Normal RAMDAC operation \n\n", 0);

		/*int counter = 0;
		while( VBE_controller_info.VideoModePtr[counter] != VIDEO_MODE_END)
		{
			if(counter == 0)//if it has at least one video mode
			{
				printf("VBE VIDEO MODES: ");
				printf("0x%X, ", VBE_controller_info.VideoModePtr[counter]);
			}
			else if( VBE_controller_info.VideoModePtr[counter + 1] == VIDEO_MODE_END)//if it is the last one
				printf("0x%X \n\n", VBE_controller_info.VideoModePtr[counter] );//doesn't take a comma at the end
			else //if it is neither the first nor the last
				printf("0x%X, ", VBE_controller_info.VideoModePtr[counter]);
		}*/

		printf("VRAM memory size: %d KB\n\n", VBE_controller_info.TotalMemory * 64);//total memory comes in groups of 64 KB

		return 0;

}

