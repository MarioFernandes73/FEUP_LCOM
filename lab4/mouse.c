#include "mouse.h"

static int hook_id = HOOK_ID_BIT_MOUSE;
static unsigned long packet[3];
static unsigned long sync_flag;
static int direction=0; //it is set to -1 if the x is moving in a negative direction, and it is set to 1 if the x moves on a positive direction
static int surpass_y_length = 0; //it is set to 1, if the test_gesture minimum length as been surpassed AND the movement was a positive slope

enum state_t {
	INIT,  //initial state
	DRAW,  //state to where the machine transits if the right button is pressed
	COMP   //final state
};
enum state_t state=INIT;

enum ev_type_t {
	NON, 	//NON event keeps the machine on the same state.
	RDOWN, //right button pressed
	RUP,	// right button lifted
	SLOPE	//positive slope has been drawn with right button pressed
};
enum ev_type_t event=NON;

void analyze_movement(short * distance_travelled_y, short length)
{
	short x_movement; //the packet x movement
	short y_movement; //the packet y movement
	unsigned long first_byte = packet[0];
	unsigned long second_byte = packet[1];
	unsigned long third_byte = packet[2];

	if(state!=DRAW)//if the state is not draw, then we dont care about the movement, because the right button is not even pressed yet
	{
		direction = 0;
		*distance_travelled_y = 0;
		return;
	}

	//X VALUE(ACCORDING TO OVERFLOW AND SIGN)
	if((first_byte & X_OVF)==X_OVF)
	{
		if((first_byte & X_SIGN)==X_SIGN)
			x_movement = - 255 - convert_negative_2complement_to_decimal(second_byte);
		else
			x_movement = 255 + second_byte;
	}
	else
	{
		if((first_byte & X_SIGN)==X_SIGN)
			x_movement = - convert_negative_2complement_to_decimal(second_byte);
		else
			x_movement = second_byte;
	}
	//Y VALUE(ACCORDING TO OVERFLOW AND SIGN)
	if((first_byte & Y_OVF)==Y_OVF)
	{
		if((first_byte & Y_SIGN)==Y_SIGN)
			y_movement = - 255 - convert_negative_2complement_to_decimal(third_byte);
		else
			y_movement = 255 + third_byte;
	}
	else
	{
		if((first_byte & Y_SIGN)==Y_SIGN)
			y_movement = -convert_negative_2complement_to_decimal(third_byte);
		else
			y_movement = third_byte;
	}

	//change direction acording to the movement
	if(direction==0)
	{
		if ( (y_movement>0 && x_movement>=0) || (y_movement>=0 && x_movement>0) )
		{
			direction = 1;
			*distance_travelled_y = y_movement;
		}
		else if ( (y_movement<0 && x_movement<=0) || (y_movement<=0 && x_movement<0) )
		{
			direction = -1;
			*distance_travelled_y = y_movement;
		}
		else //comes here if both y and x movement is zero
			direction=0;
	}
	else if (direction == 1)
	{

		if (y_movement>=0 && x_movement>=0)
		{
			direction = 1;
			*distance_travelled_y = *distance_travelled_y + y_movement;
		}
		else if( (y_movement<0 && x_movement<=0) || (y_movement<=0 && x_movement<0) )
		{
			direction = -1;
			*distance_travelled_y = y_movement;
		}
		else //comes here if one of the movements is negative and the other one is positive
		{
			direction = 0;
			*distance_travelled_y = 0;
		}
	}
	else if (direction == -1)
	{

		if (y_movement<=0 && x_movement<=0)
		{
			direction = -1;
			*distance_travelled_y = *distance_travelled_y + y_movement;
		}
		else if( (y_movement>0 && x_movement>=0) || (y_movement>=0 && x_movement>0) )
		{
			direction = 1;
			*distance_travelled_y = y_movement;
		}
		else //comes here if one of the movements is negative and the other one is positive
		{
			direction = 0;
			*distance_travelled_y = 0;
		}
	}

	if( (*distance_travelled_y >= length && length>=0) || (*distance_travelled_y <=length && length<0) )
	{
		surpass_y_length = 1;

	}
	else
		surpass_y_length = 0;
	
	printf("distance_travelled_y=%d  \n", *distance_travelled_y);

}

void declare_event()
{
	unsigned long first_byte=packet[0];

	switch(state)
	{
	case INIT:
		if((first_byte & RB)==RB)
		{
			event=RDOWN;
		}
		else
		{
			event=NON;
		}
		break;

	case DRAW:
		if((first_byte & RB)==0)
		{
			event=RUP;
		}
		else if(surpass_y_length == 1)
		{
			event=SLOPE;
		}
		else//comes here if y_length has NOT yet been surpassed OR there wasn't a positive slope
		{
			event=NON;
		}
		break;
	}
}

int test_gesture_handler()
{
	switch(state)
	{
	case INIT:
		if (event==RDOWN)
		{
			state=DRAW;
		}
		break;

	case DRAW:
		if(event==RUP)
		{
			state=INIT;
		}
		if(event==SLOPE)
		{
			state=COMP;
		}
		break;
	}

	if (state==COMP)
		return 0;

	return 1;
}


int MOUSE_subscribe_int(void) {

	//MOUSE_IRQ -> variable in mouse.h -> irq line of input.
	//IRQ_REENABLE -> policy to inform the GIH that it can give EOI command.
	//IRQ_EXCLUSIVE -> don't let the default IH "steal" mouse interrupts
	//hook_id -> input and output (that's why we need the &)(already initialized globally)
	if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id) != OK) {
		printf("sys_irqsetpolicy failed\n");
		return -1;	// sys_irqsetpolicy failed
	}

	return HOOK_ID_BIT_MOUSE; //returns initial bit from hook_id (irq_set).

}

int MOUSE_unsubscribe_int() {

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



int test_packet_handler()
{
	unsigned long data;

	if(read_status_to_read()!=0)
		return -1;

	//TUDO OK NO STATUS, LER DO OUTPUT
	sys_inb(OUT_BUF,&data);
	packet[sync_flag]=data;	//

	if(sync_flag!=2)
		sync_flag++;
	else
		sync_flag=0;

	if(sync_flag==0)
	{
		//DUPLA VERIFICACAO
		if(packet[0] & BIT(3))	//ISTO VAI SER SEMPRE VERDADE, SE NAO, É ERRO DE CIMA!
		{
			//ESTA CHEIO, PRONTO PARA PRINT
			print_packet(packet);
			return 1;
		}
		else
			return -2;			//HA ERROS A LER
	}
	else
	{
		//ENTRA AQUI SE PACKET AINDA NAO ESTIVER CHEIO!
		return 0;	// RETURN 0 -> OK
	}
	return 1;

}

int check_bit(unsigned long n, unsigned long bit)
{
	if( (n & bit) == bit)
		return 1;
	else
		return 0;
}

int convert_negative_2complement_to_decimal(unsigned long n)//given a negative number n(8 bits) without the sign bit, expressed in 2's complement, converts it to its absolute value in decimal base
{//nesta funcao usamos o metodo de conversão em que mantemos todos os bits iguais até encontrarmos o primeiro bit a 1, a partir de aí trocamos todos os bits
	int found_first_one = 0;//this flag will become , when first bit set to 1 is found
	int decimal=0;
	int contador=0;
	while(contador<8)//8 is the number of bits present in n (it is always 8 because the arguments of this function will be either the second or third byte of the packet)
	{
		if(found_first_one) //comes here if the first BIT set to 1 has already been found
		{//if bit is 0, we switch it to one and vice-versa
			if((n & BIT(contador))==0)
				decimal=(decimal|BIT(contador));
			contador++;
		}
		else
		{
			if((n & BIT(contador))==BIT(contador))//comes here until the first bit set to 1 is found, inclusivé
			{//we keep the same bit
				decimal=(decimal|BIT(contador)); 
				found_first_one=1;
			}
			contador++;
		}
	}
	return decimal;
}


int test_config_handler()
{

	unsigned long data,res,rate;

	if (read_status_to_read()==0)
		sys_inb(OUT_BUF,&data);
	else
		return -1;
	if (read_status_to_read()==0)
		sys_inb(OUT_BUF,&res);
	else
		return -2;
	if (read_status_to_read()==0)
		sys_inb(OUT_BUF,&rate);
	else
		return -3;


	print_data(data);
	print_res(res);
	print_rate(rate);

	return 0;

}



void print_packet(unsigned long * packet)
{
	unsigned long first_byte = packet[0];
	unsigned long second_byte = packet[1];
	unsigned long third_byte = packet[2];

	printf("B1=0x%02x  ", first_byte);
	printf("B2=0x%02x  ", second_byte);
	printf("B3=0x%02x  ", third_byte);
	printf("LB=%d  ", check_bit(first_byte, LB));
	printf("MB=%d  ", check_bit(first_byte, MB));
	printf("RB=%d  ", check_bit(first_byte, RB));
	printf("XOV=%d  ", check_bit(first_byte, X_OVF));
	printf("YOV=%d  ", check_bit(first_byte, Y_OVF));
	//PRINT X VALUE(ACCORDING TO OVERFLOW AND SIGN
	if((first_byte & X_OVF)==X_OVF)
	{
		if((first_byte & X_SIGN)==X_SIGN)
			printf("X=-%d  ", 255 + convert_negative_2complement_to_decimal(second_byte));
		else
			printf("X=%d  ", 255 + second_byte);
	}
	else
	{
		if((first_byte & X_SIGN)==X_SIGN)
			printf("X=-%d  ", convert_negative_2complement_to_decimal(second_byte));
		else
			printf("X=%d  ", second_byte);
	}
	//PRINT Y VALUE(ACCORDING TO OVERFLOW AND SIGN
	if((first_byte & Y_OVF)==Y_OVF)
	{
		if((first_byte & Y_SIGN)==Y_SIGN)
			printf("Y=-%d\n", 255 + convert_negative_2complement_to_decimal(third_byte));
		else
			printf("Y=%d\n", 255 + third_byte);
	}
	else
	{
		if((first_byte & Y_SIGN)==Y_SIGN)
			printf("Y=-%d\n", convert_negative_2complement_to_decimal(third_byte));
		else
			printf("Y=%d\n", third_byte);
	}
}

int write_mouse(unsigned char cmd)
{
	unsigned long status;
	unsigned long response;

	if(read_status_to_write()==0)
		sys_outb(STAT_REG, KBC_MOUSE);//Send 0xD4 COMMAND to KBC to allow us to send bytes to the mouse
	else
		return -1;

	//printf("PASS1\n");
	if(read_status_to_write()==0)
		sys_outb(IN_BUF, cmd);//Send desired command/argument to the mouse
	else
		return -2;

	//printf("PASS2\n");

	if(read_status_to_read()==0)
		sys_inb(OUT_BUF, &response);//read response
	else
		return -3;

	//printf("PASS3\n");

	if(response!=ACK)
	{
		printf("RESPONSE NOT ACK\n");
		return -4;
	}

	return 0;
}



void print_data(unsigned long data)
{
	printf("======================BYTE 1======================\n");
	printf("\n");
	printf("Bit 7 -> Always 0\n");
	printf("Bit 6 -> ");
	if(check_bit(data,BIT(6)))
		printf("1: Remote mode enabled\n");
	else
		printf("0: Stream mode enabled\n");
	printf("Bit 5 -> ");
	if(check_bit(data,BIT(5)))
		printf("1: Data reporting enabled\n");
	else
		printf("0: Data reporting disabled\n");
	printf("Bit 4 -> ");
	if(check_bit(data,BIT(4)))
		printf("1: Scaling is 2:1\n");
	else
		printf("0: Scaling is 1:1\n");
	printf("Bit 3 -> Always 0\n");
	printf("Bit 2 -> ");
	//Buttons appear changed due to the information on the class pdf's
	if(check_bit(data,BIT(2)))
		printf("1: Left button is pressed\n");
	else
		printf("0: Left button is not pressed\n");
	printf("Bit 1 -> ");
	if(check_bit(data,BIT(1)))
		printf("1: Middle button is pressed\n");
	else
		printf("0: Middle button is not pressed\n");
	printf("Bit 0 -> ");
	if(check_bit(data,BIT(0)))
		printf("1: Right button is pressed\n");
	else
		printf("0: Right button is not pressed\n");
	printf("\n");
}

void print_res(unsigned long res)
{
	printf("======================BYTE 2======================\n");
	printf("\n");
	printf("Bit ");
	if(check_bit(res,BIT(3)))
		printf("3: Resolution = 8 count/mm\n");
	else if(check_bit(res,BIT(2)))
		printf("2: Resolution = 4 count/mm\n");
	else if(check_bit(res,BIT(1)))
		printf("1: Resolution = 2 count/mm\n");
	else if(check_bit(res,BIT(0)))
		printf("0: Resolution = 1 count/mm\n");
	printf("\n");
}

void print_rate(unsigned long rate)
{
	printf("======================BYTE 3======================\n");
	printf("\n");

	printf("Sample rate: %d hz\n\n",rate);
}
