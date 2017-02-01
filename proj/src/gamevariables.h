#ifndef __GAMEVARIABLES_H
#define __GAMEVARIABLES_H

/*------------------------PLAYER TURNS-------------------------*/
/**
 * Indicates whose player will play in the current turn
 */
typedef enum{
	PLAYER1,
	PLAYER2
}game_turn_t;

/*------------------------STATES AND EVENTS----------------------*/
/**
 * Game states
 */
typedef enum{
	MAIN_MENU,	/**< we are on the main menu screen*/
	START,      /**< a new round has started and a "player1's turn to play" screen is drawn*/
	TURN_TRANSITION,  /**< the last player1's move has been made and we are waiting for the timeout that follows every input*/
	TURN_TRANSITION2, /**< player1's turn has been finished and we are showing a "player2's turn to play" draw*/
	ROUND_TRANSITION, /**< the last player2's move has been made and we are waiting for the timeout that follows every input*/
	ROUND_TRANSITION2, /**< the round has finished and we are showing a draw telling if player2 copied or not  successfully in that round*/
	INPUT,  /**< a move has been made by a player and his input is being drawn on the game board*/
	PLAYING, /**< we are in the middle of a turn, waiting for a player input*/
	PRE_ENDING, /**< the game has finished and we are drawing a screen that tells if player2 has successfully copied player1 in the last round*/
	ENDING,     /**< the game has finished and player2's final score will be shown*/
	END_GAME,   /**< the game has ended ,a "thanks for playing" will be drawn, and the game will go to the main menu after a couple seconds*/
	EXIT_GAME  /**< the exit button has been pressed on main menu and the program will shut down*/
}game_state_t;

/**
 * Indicates if player2 has successfuly copied all the player1's moves in the last round
 */
typedef enum{
	DEFAULT,		/**< start, nothing happens*/
	WIN,			/**< player won last round, copied all player1's moves*/
	LOSE			/**< player lost lost round, failed to copy all player1's moves*/
}player_state_t;

//INPUTS
//note: a player move is considered an input, the inputs acepted by the game are:
// pressing key A,W,S or D
//pressing a figure on screen(triangle, circle, star or square) with mouse's left button
//doing a positive slope(ascendent or descendent) while the mouse has its right button pressed
//when the input is the escpae key, the game will shut down
#define INVALID_INPUT				0   /**< @brief Represents a player move(input) that is not accepted by the game */
#define ESC_KEY						1	/**< @brief Represents the escape key input*/
#define A_KEY						2	/**< @brief Represents the A key input*/
#define W_KEY						3	/**< @brief Represents the W key input*/
#define S_KEY						4	/**< @brief Represents the S key input*/
#define D_KEY						5	/**< @brief Represents the D key input*/
#define TRIANGLE					6	/**< @brief Represents the triangle input*/
#define CIRCLE						7	/**< @brief Represents the circle input*/
#define STAR						8	/**< @brief Represents the star input*/
#define SQUARE						9	/**< @brief Represents the square input*/
#define ASCENDENT_SLOPE				10	/**< @brief Represents the ascend slope input*/
#define DESCENDENT_SLOPE			11	/**< @brief Represents the descendent slope input*/

#define START_INPUT					20	/**< @brief Represents the start button input on hte main menu*/
#define EXIT_INPUT					21	/**< @brief Represents the exit button input on hte main menu*/

#define LEFT_CLICK					30	/**< @brief Represents the mouse left button click*/
#define RIGHT_CLICK					31	/**< @brief Represents the mouse right button click*/

//The following are coordinate limits for the figures that appear on screen
#define SQUARE_LEFT_BORDER			65
#define SQUARE_RIGHT_BORDER			193
#define SQUARE_TOP_BORDER			433
#define SQUARE_BOTTOM_BORDER		305

#define TRIANGLE_LEFT_BORDER		210
#define TRIANGLE_RIGHT_BORDER		338
#define TRIANGLE_TOP_BORDER			286
#define TRIANGLE_BOTTOM_BORDER		170

#define CIRCLE_LEFT_BORDER			340
#define CIRCLE_RIGHT_BORDER			470
#define CIRCLE_TOP_BORDER			428
#define CIRCLE_BOTTOM_BORDER		300

#define STAR_LEFT_BORDER			210
#define STAR_RIGHT_BORDER			344
#define STAR_TOP_BORDER				668
#define STAR_BOTTOM_BORDER			440

#define START_LEFT_BORDER			210
#define START_RIGHT_BORDER			352
#define START_TOP_BORDER			567
#define START_BOTTOM_BORDER			425

#define EXIT_LEFT_BORDER			450
#define EXIT_RIGHT_BORDER			592
#define EXIT_TOP_BORDER				567
#define EXIT_BOTTOM_BORDER			425





#define LENGTH 						300  /**< @brief minimum length considered for the mouse vertical slope distance*/

#define CURSOR_SPEED				1    /**< @brief speed at wich the mouse animation will move when a positive slope has been made*/

unsigned int X_IMG_COORD;		/**< @brief x coordinate that a mouse animated image will take, this animated image will appear when a positive slope has been made with the mouse*/
unsigned int Y_IMG_COORD;		/**< @brief y coordinate that a mouse animated image will take, this animated image will appear when a positive slope has been made with the mouse*/


#define ROUND_LIMIT					10  /**< @brief number of rounds the game will last for*/

/*-------------------TIME RELATED CONSTANTS(MEASURED IN SECONDS)-------------*/
#define INPUT_TIMEOUT					2   /**< @brief time to wait after a player has given an input, it is also the time for wich the input image will be on the screen*/
#define TURN_ANNOUNCEMENT_TIMEOUT		3	/**< @brief time that the player turn announcement image will last on the screen*/
#define ROUND_RESULT_TIMEOUT			3   /**< @brief time that the player2's round result(copied successfuly or not) image will last on the screen*/
#define FINAL_SCORE_TIMEOUT				5	/**< @brief time that the player2's game final score image will last on the screen*/
#define THANKS_TIMEOUT					5	/**< @brief time that the "thanks for playing" image will last on the screen*/

#endif
