#include "game.h"


static int escape = 0; //is set to 1 when the escape key has been pressed

static int player2_score=0; //player2 total score
static int player1_array[10]; //array with player1 inputs
static int player2_array[10]; //array with player2 inputs
static int position=0; //current position in the input array
static int timeout=0;  //time of the timeout that will occur
static int timeout_started=0; //it is set to 1 when a timeout has just been initializaed, it is set to 0 when that timeout has ended
static int timeout_entered=0; //it is set to 1 when a timeout has just been initialized and is it set to 0 when the state relative to that timeout has been changed

static unsigned int timer_counter=0; //increases by one everytime there's a tiemr interrupt
static unsigned int time = 0; //increases by one every second
static unsigned int round = 1; //increases by one every time a round passes by

static int mouse_x_coord; //current x coordinate of the mouse position on the screen
static int mouse_y_coord; //current y coordinate of the mouse position on the screen

game_turn_t game_turn = PLAYER1; //the player1 always takes the first turn, the player2 always copies the player1
game_state_t game_state = MAIN_MENU;
player_state_t player_state = DEFAULT;

int simonsays()
{
	if (TIMER_subscribe_int()<0)
	{
		printf("TIMER_subscribe_int failed\n");
		return 1;
	}
	if (KBD_subscribe_int() < 0)
	{
		printf("KBD_subscribe_int failed\n");
		return 2;	// KBD_subscribe_int failed
	}
	if (MOUSE_subscribe_int()<0)
	{
		printf("MOUSE_subscribe_int failed\n");
		return 3;	// MOUSE_subscribe_int failed
	}
	if(write_mouse(STREAM_MODE)>0)
	{
		printf("ERROR SETTING STREAM MODE\n");
		return 4;
	}
	if(write_mouse(ENABLE)>0)
	{
		printf("ERROR ENABLING STREAM MODE\n");
		return 5;
	}
	char* virtual_address;
	virtual_address = vg_init(SIMONSAYS_RESOLUTION);
	if (virtual_address == NULL) {
		printf("ERROR INITIALIZING GRAPGHICS MODE \n");
		return 6;
	}

	get_bitmaps();


	X_IMG_COORD = 610;
	Y_IMG_COORD = 300;


	int input=0, input_buffer;
	int ipc_status, r = 0,
			irq_set_kbd = BIT(HOOK_ID_BIT_KBD),
			irq_set_mouse = BIT(HOOK_ID_BIT_MOUSE),
			irq_set_timer = BIT(HOOK_ID_BIT_TIMER);
	message msg;

	MOUSE_get_coord(&mouse_x_coord,&mouse_y_coord);

	while (escape == 0 || (game_state == END_GAME && timeout == 0))
	{
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
		{
			printf("driver_receive failed\n");
			break;
		}
		if (is_ipc_notify(ipc_status)) 		// notification
		{
			if(_ENDPOINT_P(msg.m_source) == HARDWARE)		//hardware
			{
				if (msg.NOTIFY_ARG & irq_set_timer)	//timer
				{
					timer_handler(&timer_counter, &time);
					if(time == timeout)
					{
						X_IMG_COORD=650;
						Y_IMG_COORD=400;
						timeout=0;
						timeout_started=0;
					}

					MOUSE_get_coord(&mouse_x_coord, &mouse_y_coord);
					if(timer_state_machine(input))
						time = 0;
					draw_cursor(mouse_x_coord, mouse_y_coord);
					swap_buffer();

				}
				if (msg.NOTIFY_ARG & irq_set_kbd)			//keyboard
				{

					input_buffer=KBD_handler();
					if (input_buffer== ESC_KEY) {
						escape = 1;
						break;
					}
					if(timeout == 0 && input_buffer > 1) //se o input for maior que 1 significa que é A,W,S ou D
					{
						input = input_buffer;
						state_machine(input);
					}
				}
				else if (msg.NOTIFY_ARG & irq_set_mouse) 		//mouse
				{
					input_buffer=MOUSE_handler(game_state);
					if (input_buffer>1 && input_buffer !=LEFT_CLICK && input_buffer != RIGHT_CLICK)
					{
						input=input_buffer;
						state_machine(input);
					}

				}
			}
		}
	}

	if (vg_exit() != 0) {
		printf("ERROR CHANGING BACK TO TEXT MODE\n");
		return 7;
	}
	if(write_mouse(DISABLE)>0)
	{
		printf("ERROR SETTING STREAM MODE\n");
		return 8;
	}
	if(MOUSE_unsubscribe_int() !=0)
	{
		printf("MOUSE_unsubscribe_int failed\n");
		return 9;	//MOUSE_unsubscribe_int failed
	}

	if (KBD_unsubscribe_int() != 0) {
		printf("KBD_unsubscribe_int failed\n");
		return 10;	// KBD_unsubscribe_int failed
	}
	if (TIMER_unsubscribe_int() != 0)
	{
		printf("TIMER_unsubscribe_int failed.\n");
		return 11;
	}
	return 0;
}

void state_machine(int input)
{

	if(timeout > 0)
		return;

	switch(game_state)
	{

	case MAIN_MENU:
	{
		//se carregar no start game -> muda estado de jogo para START;
		if(input == START_INPUT)
			game_state = START;
		else if (input == EXIT_INPUT)
			game_state = EXIT_GAME;
	}
	break;
	default:
	{
		if(game_turn == PLAYER1)
		{
			if(position < round)
				player1_array[position]=input;
			position++;
			if(position >= round)		// muda de jogada
			{
				game_state = TURN_TRANSITION;
				game_turn = PLAYER2;
				position = 0;
			}
			else
			{
				game_state = INPUT;
			}
		}
		else
		{
			if(position < round)
				player2_array[position]=input;
			position++;
			if(position >= round)			//muda de ronda
			{
				if(compare_arrays(player2_array, player1_array, round))		//aumenta os pontos do player a adivinhar
				{
					player2_score+=round;
					player_state=WIN;
				}
				else
				{
					player_state=LOSE;
				}
				round++;
				if(round > ROUND_LIMIT)
				{
					game_state = PRE_ENDING;
					return;
				}
				game_state = ROUND_TRANSITION;
				game_turn = PLAYER1;
				position = 0;
			}
			else
			{
				game_state = INPUT;
			}

		}
	}


	}

}


int timer_state_machine(int input)
{

	switch(game_state)
	{
	case MAIN_MENU:
	{
		timeout = 0;
		draw_main_menu();
		return 0;
	}
	break;
	case START:
	{
		if(checkTimeout(TURN_ANNOUNCEMENT_TIMEOUT))
			return 1;
		draw_player1_turn();
		if(timeout_started == 0 && timeout_entered == 1)
		{
			timeout_entered = 0;
			game_state = PLAYING;
		}
		return 0;
	}
	break;
	case TURN_TRANSITION:
	{

		if(checkTimeout(INPUT_TIMEOUT))
			return 1;

		draw_board(round);
		draw_input(input);
		if(timeout_started == 0 && timeout_entered == 1)
		{
			timeout_entered = 0;
			game_state = TURN_TRANSITION2;
		}
		return 0;
	}
	break;
	case TURN_TRANSITION2:
	{
		if(checkTimeout(TURN_ANNOUNCEMENT_TIMEOUT))
			return 1;

		draw_player2_turn();
		if(timeout_started == 0 && timeout_entered == 1)
		{
			timeout_entered = 0;
			game_state = PLAYING;
		}
		return 0;
	}
	break;
	case ROUND_TRANSITION:
	{
		if(checkTimeout(INPUT_TIMEOUT))
			return 1;
		draw_board(round-1);
		draw_input(input);
		if(timeout_started == 0 && timeout_entered == 1)
		{
			timeout_entered = 0;
			game_state = ROUND_TRANSITION2;
		}

		return 0;
	}
	break;
	case ROUND_TRANSITION2:
	{
		if(checkTimeout(ROUND_RESULT_TIMEOUT))
			return 1;
		if(player_state == WIN)
			draw_successful_copy();
		else if(player_state == LOSE)
			draw_failed_copy();

		if(timeout_started == 0 && timeout_entered == 1)
		{
			timeout_entered = 0;
			game_state = START;
		}
		return 0;
	}
	break;
	case INPUT:
	{
		if(checkTimeout(INPUT_TIMEOUT))
			return 1;
		draw_board(round);
		draw_input(input);
		if(timeout_started == 0 && timeout_entered == 1)
		{
			timeout_entered = 0;
			game_state = PLAYING;
		}
		return 0;
	}
	break;
	case PLAYING:
	{
		draw_board(round);
		return 0;
	}
	break;
	case PRE_ENDING:
	{
		if(checkTimeout(ROUND_RESULT_TIMEOUT))
			return 1;
		if(player_state == WIN)
			draw_successful_copy();
		else if(player_state == LOSE)
			draw_failed_copy();

		if(timeout_started == 0 && timeout_entered == 1)
		{
			timeout_entered = 0;
			game_state = ENDING;
		}
		return 0;
	}
	case ENDING:
	{
		if(checkTimeout(FINAL_SCORE_TIMEOUT))
			return 1;
		draw_score(player2_score);
		if(timeout_started == 0 && timeout_entered == 1)
		{
			timeout_entered = 0;
			game_state = END_GAME;
		}
		return 0;
	}
	case END_GAME:
	{
		if(checkTimeout(THANKS_TIMEOUT))
			return 1;
		draw_thanks_for_playing();
		if(timeout_started == 0 && timeout_entered == 1)
		{
			//reestabelecer as variaveis iniciais
			player2_score=0;

			position=0;
			timeout=0;
			timeout_started=0;
			timeout_entered=0;

			timer_counter=0;
			time = 0;
			round = 1;

			game_state = MAIN_MENU;
			player_state = DEFAULT;
			game_turn = PLAYER1;

		}
		return 0;
	}
	case EXIT_GAME:
	{
		if(checkTimeout(THANKS_TIMEOUT))
			return 1;
		draw_thanks_for_playing();
		if(timeout_started == 0 && timeout_entered == 1)
		{
			escape = 1;
		}
	}
	default:
	{
		return 0;
	}
	break;
	}
}


int checkTimeout ( int pause_time)
{
	if(timeout_started == 0 && timeout_entered == 0)
	{
		timeout = pause_time;
		timeout_entered = 1;
		timeout_started = 1;
		return 1;		// vai inicializar o timeout1;
	}
	else return 0;
}

int compare_arrays(int * array1, int * array2, int size)
{
	unsigned int i;
	for (i=0; i<size; i++)
	{
		if(!(array1[i]==array2[i]))
			return 0;
	}
	return 1;
}
