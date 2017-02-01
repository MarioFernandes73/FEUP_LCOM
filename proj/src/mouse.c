#include "mouse.h"

static int hook_id = HOOK_ID_BIT_MOUSE;
static unsigned long packet[3];//mouse's 3 byte packet with information about the pressed buttons and mouse travelled distance
static unsigned long sync_flag; //used to sycronize the mouse packet bytes
static int direction=0; //it is set to -1 if the x is moving in a negative direction, and it is set to 1 if the x moves on a positive direction
static short distance_travelled_y = 0; //vertical distance travelled by the mouse with the right button pressed
static int surpass_y_length = 0; //it is set to 1, if the minimum vertical length(LENGTH) as been surpassed AND the movement was a positive slope
static int Xcoord = 20;
static int Ycoord = 20;

state_t state=INIT;
ev_type_t event=NON;

void analyze_movement()
{
	short x_movement; //the packet x movement
	short y_movement; //the packet y movement
	unsigned long first_byte = packet[0];
	unsigned long second_byte = packet[1];
	unsigned long third_byte = packet[2];

	if(state!=DRAW)//if the state is not draw, then we dont care about the movement, because the right button is not even pressed yet
	{
		direction = 0;
		distance_travelled_y = 0;
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
			distance_travelled_y = y_movement;
		}
		else if ( (y_movement<0 && x_movement<=0) || (y_movement<=0 && x_movement<0) )
		{
			direction = -1;
			distance_travelled_y = y_movement;
		}
		else //comes here if both y and x movement is zero
			direction=0;
	}
	else if (direction == 1)
	{

		if (y_movement>=0 && x_movement>=0)
		{
			direction = 1;
			distance_travelled_y = distance_travelled_y + y_movement;
		}
		else if( (y_movement<0 && x_movement<=0) || (y_movement<=0 && x_movement<0) )
		{
			direction = -1;
			distance_travelled_y = y_movement;
		}
		else //comes here if one of the movements is negative and the other one is positive
		{
			direction = 0;
			distance_travelled_y = 0;
		}
	}
	else if (direction == -1)
	{

		if (y_movement<=0 && x_movement<=0)
		{
			direction = -1;
			distance_travelled_y = distance_travelled_y + y_movement;
		}
		else if( (y_movement>0 && x_movement>=0) || (y_movement>=0 && x_movement>0) )
		{
			direction = 1;
			distance_travelled_y = y_movement;
		}
		else //comes here if one of the movements is negative and the other one is positive
		{
			direction = 0;
			distance_travelled_y = 0;
		}
	}

	if( (distance_travelled_y >= LENGTH && LENGTH>=0) || (distance_travelled_y <=-LENGTH && -LENGTH<0) )
	{
		surpass_y_length = 1;

	}
	else
		surpass_y_length = 0;
}

int declare_event()
{
	unsigned long first_byte=packet[0];

	//declare event
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

	//change state according to the event
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
		return 1;

	return 0;

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



int MOUSE_handler(game_state_t game_state)
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
			//3 PACKETS PREPARADOS
			int x;
			x = MOUSE_analyze_input(game_state);
			return x;
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

int MOUSE_analyze_input(game_state_t game_state)
{
	unsigned long first_byte = packet[0];
	MOUSE_set_coord();
	//printf("X=%d",Xcoord);
	//printf("Y=%d",Ycoord);

	if(check_bit(first_byte, LB))
	{
		//click left button
		if(game_state == PLAYING)
		{
		if(Xcoord > SQUARE_LEFT_BORDER &&
				Xcoord < SQUARE_RIGHT_BORDER &&
				Ycoord > SQUARE_BOTTOM_BORDER &&
				Ycoord < SQUARE_TOP_BORDER)
			return SQUARE;		//clicked square
		else if(Xcoord > TRIANGLE_LEFT_BORDER &&
				Xcoord < TRIANGLE_RIGHT_BORDER &&
				Ycoord > TRIANGLE_BOTTOM_BORDER &&
				Ycoord < TRIANGLE_TOP_BORDER)
			return TRIANGLE;	//clicked triangle
		else if (Xcoord > CIRCLE_LEFT_BORDER &&
				Xcoord < CIRCLE_RIGHT_BORDER &&
				Ycoord > CIRCLE_BOTTOM_BORDER &&
				Ycoord < CIRCLE_TOP_BORDER)
			return CIRCLE;		//clicked circle
		else if (Xcoord > STAR_LEFT_BORDER &&
				Xcoord < STAR_RIGHT_BORDER &&
				Ycoord > STAR_BOTTOM_BORDER &&
				Ycoord < STAR_TOP_BORDER)
			return STAR;		//clicked STAR
		}
		else if (game_state == MAIN_MENU)
		{
			if (Xcoord > START_LEFT_BORDER &&
				Xcoord < START_RIGHT_BORDER &&
				Ycoord > START_BOTTOM_BORDER &&
				Ycoord < START_TOP_BORDER)
			return START_INPUT; // clicked START
		else if (Xcoord > EXIT_LEFT_BORDER &&
				Xcoord < EXIT_RIGHT_BORDER &&
				Ycoord > EXIT_BOTTOM_BORDER &&
				Ycoord < EXIT_TOP_BORDER)
			return EXIT_INPUT; // clicked EXIT
		}

		return LEFT_CLICK;
	}

	else if(check_bit(first_byte, RB) && game_state == PLAYING)
	{
		unsigned int final_state;
		//click right button
		analyze_movement();
		printf("%d\n", distance_travelled_y);
		final_state=declare_event();
		if(final_state && distance_travelled_y > 0)
		{
			distance_travelled_y = 0;
			state=INIT;
			event=NON;
			return ASCENDENT_SLOPE;
		}
		else if (final_state && distance_travelled_y < 0)
		{
			distance_travelled_y = 0;
			state=INIT;
			event=NON;
			return DESCENDENT_SLOPE;
		}
		return RIGHT_CLICK;
	}

	return INVALID_INPUT;	//input invalido (apenas movimento do rato)
}

void MOUSE_set_coord()
{
	unsigned long first_byte = packet[0];
	unsigned long second_byte = packet[1];
	unsigned long third_byte = packet[2];

	if((first_byte & X_OVF)==X_OVF)
		{
			if((first_byte & X_SIGN)==X_SIGN)
			{
				Xcoord -= (255 + convert_negative_2complement_to_decimal(second_byte));
				if(Xcoord < MIN_Xcoord)
					Xcoord = MIN_Xcoord;
			}
			else
			{
				Xcoord += 255 + second_byte;
				if(Xcoord > MAX_Xcoord)
					Xcoord = MAX_Xcoord;
			}
		}
	else
		{
			if((first_byte & X_SIGN)==X_SIGN)
			{
				Xcoord-=convert_negative_2complement_to_decimal(second_byte);
				if(Xcoord < MIN_Xcoord)
					Xcoord = MIN_Xcoord;
			}
			else
			{
				Xcoord += second_byte;
				if(Xcoord > MAX_Xcoord)
					Xcoord = MAX_Xcoord;
			}
		}

	if((first_byte & Y_OVF)==Y_OVF)
		{
			if((first_byte & Y_SIGN)==Y_SIGN)
			{
				Ycoord += 255 + convert_negative_2complement_to_decimal(third_byte);
				if(Ycoord > MAX_Ycoord)
					Ycoord = MAX_Ycoord;
			}
			else
			{
				Ycoord -= 255 + third_byte;
				if(Ycoord < MIN_Ycoord)
					Ycoord = MIN_Ycoord;
			}
		}
	else
		{
			if((first_byte & Y_SIGN)==Y_SIGN)
			{
				Ycoord+=convert_negative_2complement_to_decimal(third_byte);
				if(Ycoord > MAX_Ycoord)
					Ycoord = MAX_Ycoord;
			}
			else
			{
				Ycoord -= third_byte;
				if(Ycoord < MIN_Ycoord)
					Ycoord = MIN_Ycoord;
			}
		}
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

void MOUSE_get_coord(int * X, int * Y)
{
	*X = Xcoord;
	*Y = Ycoord;
	return;
}
