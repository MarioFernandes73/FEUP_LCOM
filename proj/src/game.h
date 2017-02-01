#ifndef __GAME_H
#define __GAME_H

#include "keyboard.h"
#include "timer.h"
#include "video_gr.h"
#include "vbe.h"
#include "mouse.h"
#include "gamevariables.h"

/**
* @brief main function that calls the game interruption handlers and the game state machine funtions
*
*/
int simonsays();

/**
* @brief state machine that changes the game state according to the inputs given by the players
*
* This function is responsible for filling the arrays with the player inputs
*
* This function also changes the state acording to the button pressed on the main menu(START OR EXIT)
*
* @param input input given by a player
*/
void state_machine(int input);

/**
* @brief state machine that changes the game state according to the inputs given by the players ,makes the time pauses and draws the screen according to the current state
*
* although it has the same states as the state machine, the timer state machine is also responsible for the time pauses.
*
* @param input input given by a player
*
* @return Returns 1 if a timeout has started(only returns 1 in the beggining of the timeout, if we are in the middle of the timeout it returns 0), returns zero otherwise
*/
int timer_state_machine(int input);

/**
 * @brief is used to make a timeout(wait a number of seconds without doing anything)
 *
 * is useful between turns and other cases
 *
 * @param pause_time indicates the timeout number of seconds
 *
 * @return Returns 1 if a timeout has started(only returns 1 in the beggining of the timeout, if we are in the middle of the timeout it returns 0), returns zero otherwise
 */
int checkTimeout ( int pause_time);

/**
 * @brief compares the first size positions of two arrays
 *
 * if the first size positions are equal, then we consider both arrays equal
 *
 * @param array1 first array to compare
 *
 * @param array2 second array to compare
 *
 * @size number of position to be compared
 *
 * @return Returns 1 the arrays are equal, returns zero otherwise
 */
int compare_arrays(int * array1, int * array2, int size);



#endif
