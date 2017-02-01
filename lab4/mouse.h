#ifndef __MOUSE_H
#define __MOUSE_H

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h> //Used to emasure time
#include <minix/com.h> //Used for interrupt handling (while loop)
#include "kbc.h"

#define COMPUTER_FREQ		60

#define BIT(n) (0x01<<(n))

#define TRIES 		50

#define PACKET_SIZE	3		/**< @brief number of bytes per packet */
#define FIRST_BYTE_BIT BIT(3) /**< @brief bit 3 of the first packet byte is always set to 1*/

#define HOOK_ID_BIT_MOUSE	3

#define DELAY_US    20000	/**< @brief used to measure time */

#define STAT_REG    0x64
#define CTRL_REG 	0x64
#define IN_BUF		0x60
#define OUT_BUF		0x60

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

 *
 * @return Return 0 when the position 0 and 1 of packet was filled. Returns 1 when the packet is full
 * Returns negative when non-success
 */
int test_packet_handler();

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
 * @brief Prints a packet information in a human friendly way
 *
 * @param *packet -> pointer to the packet that should be printed
 *
 * @return void
 */
void print_packet(unsigned long * packet);

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
 * @brief Prints the FIRST byte of the mouse's status in a human friendly way
 *
 * @param data -> byte to print
 *
 * @return void
 */
void print_data(unsigned long data);

/**
 * @brief Prints the SECOND byte of the mouse's status in a human friendly way
 *
 * @param res -> byte to print
 *
 * @return void
 */
void print_res(unsigned long res);

/**
 * @brief Prints the THIRD byte of the mouse's status in a human friendly way
 *
 * @param rate -> byte to print
 *
 * @return void
 */
void print_rate(unsigned long rate);

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
 *
 *  the movement is only analysed if the mouse right button is pressed
 * @param * distance_travelled_y -> counter that accumulates the y movement
 * @param length -> minimum length on y movement
 *
 * @return void
 */
void analyze_movement(short * distance_travelled_y, short length);

/**
 * @brief Declares an event based on the machine current state
 *
 *
 * @return void
 */
void declare_event();

/**
 * @brief Changes the machine state based on the occurred event
 *
 *
 *
 * @return Returns 0 if it has achieved the final state and 1 otherwise
 */
int test_gesture_handler();

/**
 * @brief Fills the data, res and rate bytes and prints them
 *
 *
 * @return Returns 0 on success and non-zero otherwise
 */
int test_config_handler();

#endif
