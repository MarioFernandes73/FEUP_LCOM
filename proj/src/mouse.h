#ifndef __MOUSE_H
#define __MOUSE_H

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h> //Used to emasure time
#include <minix/com.h> //Used for interrupt handling (while loop)
#include "kbc.h"
#include "gamevariables.h"

#define COMPUTER_FREQ		60

#define BIT(n) (0x01<<(n)) /**< @brief returns a number's n bit */

#define TRIES 		50

#define PACKET_SIZE	3		/**< @brief number of bytes per packet */
#define FIRST_BYTE_BIT BIT(3) /**< @brief bit 3 of the first packet byte is always set to 1*/

#define HOOK_ID_BIT_MOUSE	3

#define DELAY_US    20000	/**< @brief used to measure time */

#define STAT_REG    0x64    /**< @brief STATUS REGISTER port */
#define CTRL_REG 	0x64	/**< @brief CONTROL REGISTER port */
#define IN_BUF		0x60	/**< @brief INPUT BUFFER port */
#define OUT_BUF		0x60	/**< @brief OUTPUT BUFFER port */

#define MOUSE_IRQ 	12		/**< @brief MOUSE IRQ line */

#define KBC_MOUSE 	0xD4	/**< @brief comand to send to the CTRL_REG to let us send bytes to the mouse */

/*---------------MOUSE COMMANDS---------*/
#define STREAM_MODE	0xEA	/**< @brief mouse comand that enables stream mode */
#define ENABLE 		0xF4	/**< @brief to allow us to receive packets in stream mode */
#define DISABLE 	0xF5	/**< @brief to don't receive packets in stream mode(that way there won't be interruptions being created */
#define CONFIG		0xE9

/*---------------MOUSE RESPONSES---------*/
#define ACK 		0xFA	/**< @brief mouse response if everything went OK */
#define NACK 		0xFE	/**< @brief mouse response if invalid byte */
#define ERROR 		0xFC	/**< @brief mouse response if second consecutive invalid byte */

/*-------STATUS REGISTER BITS---------------*/
#define PARITY		BIT(7)
#define TIMEOUT		BIT(6)
#define AUX			BIT(5)
#define INH			BIT(4)
#define A2			BIT(3)
#define SYS			BIT(2)
#define IBF		   	BIT(1)    /**< @brief Input buffer full bit */
#define OBF		   	BIT(0)    /**< @brief Output buffer full bit */

/*-------FIRST PACKET BYTE BITS------------*/
#define LB         BIT(0)
#define RB         BIT(1)
#define MB         BIT(2)
#define X_SIGN     BIT(4)
#define Y_SIGN     BIT(5)
#define X_OVF      BIT(6)
#define Y_OVF      BIT(7)

#define MAX_Ycoord		578 /**< @brief maximum valaue of the mouse y coordinate */
#define MIN_Ycoord		0   /**< @brief minimum valaue of the mouse y coordinate */
#define MAX_Xcoord		786 /**< @brief maximum valaue of the mouse x coordinate */
#define MIN_Xcoord		0   /**< @brief minimum valaue of the mouse x coordinate */

/**
 * Represents a mouse state
 */
typedef enum {
	INIT, /**< mouse is on the initial state(no key has been pressed) */
	DRAW, /**< right button has been pressed and we are ready todetect a positive slope*/
	COMP /**< a positive slope has been drawn successfuly(final state)*/
} state_t;

/**
 * Represents a mouse event
 */
typedef enum  {
	NON, /**< NON event keeps the machine on the same state*/
	RDOWN, /**< the right button has been pressed*/
	RUP, /**< the right button is released*/
	SLOPE /**< a positive slope has been drawn*/
} ev_type_t;




/**
 * @brief To subscribe mouse interrupts
 *
 *
 * @return Return HOOK_ID_BIT_MOUSE upon success and non-zero otherwise
 */
int MOUSE_subscribe_int(void);

/**
 * @brief To unsubscribe mouse interrupts
 *
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int MOUSE_unsubscribe_int();

/**
 * @brief Syncronizes and fills the packet. When the packet is full, calls the print function
 *
 * @param game_state current game state
 *
 * @return Returns the input given by the player. Returns 0 if the mouse packet is not yet full. Returns -2 or 1 if there are otehr types of errors
 */
int MOUSE_handler(game_state_t game_state);

/**
 * @brief checks if an input has been given by a player
 *
 * The possible inputs that this functions registers are selecting a figure on the screen or doing a positive slope
 *
 * It analyzes the current game state to know if a mouse click on the screen should be treated has a click on a figure in the game board or a click in the start button in the main menu, for example
 *
 * @param game_state current state of the game
 *
 * @return Returns the input given by the player
 */
int MOUSE_analyze_input(game_state_t game_state);

/**
 * @brief updates the mouse coordinates according to the distance travelled in the last movement
 *
 */
void MOUSE_set_coord();

/**
 * @brief Compares a number and a bit
 *
 * @param n -> number whose given BIT will be checked ; bit-> BIT to check
 *
 * @return Returns 1 if the given bit is active(set to 1) and retuurns 0 otherwise
 */
int check_bit(unsigned long n, unsigned long bit);

/**
 * @brief Converts a given negative number represented in 2's complement to a decimal base integer
 *
 * The given number in 2´s complement does not include the sign bit, and it will always be treated has a negative number
 * That being said, this function should only be called with a negative number
 *
 * The returned integer is always positive
 *
 * @param n -> negative number expressed in 2's complement, without the sign bit
 *
 * @return Returns n converted to a decimal base integer (absolute value)
 */
int convert_negative_2complement_to_decimal(unsigned long n);

/**
 * @brief Writes a command/argument to the mouse and analyzes the response
 *
 * First writes the 0xD4 command to the KBC, and then writes the given command/argument to the mouse.
 *
 * After this, reads the mouse response to the command/argument sent
 *
 * @param cmd -> command/argument to send to the mouse
 *
 * @return Return 0 if the response is ACK and non-zero otherwise
 */
int write_mouse(unsigned char cmd);

/**
 * @brief Analizes the x an y mouse movement on every packet
 *
 * sets the direction variable according to the movement direction
 * direction 1 means that both x and y movement is non negative
 *
 * direction -1 means that both x and y movement is non positive
 *
 *  direction 0 means that a movement different from all the other directions
 *
 *  also registers the y movement accumulation , if direction is different that zero
 *
 *  the objective of this function is to understand when a positive slope with the minimum length on y movement has happened
 *  and when it does happen, it sets the variable surpass_y_length to 1
 */
void analyze_movement();

/**
 * @brief Declares an event based on the machine current state and then changes the current state based on that same event
 *
 *
 * @return Returns 1 if the machine has accomplished its final state(a positive slope has been drawn pressing the right button, returns 0 otherwise
 */
int declare_event();

/**
 * @brief gives the current mouse coordinates
 *
 * @param X pointer to wich will be sent the current mouse x coordinate
 *
 * @param Y pointer to wich will be sent the current mouse y coordinate
 */
void MOUSE_get_coord(int * X, int * Y);

#endif
