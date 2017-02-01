#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h> //Used to measure time in IH
#include <minix/com.h> //Used for interrupt handling (while loop)
#include "timer.h"
#include "i8254.h"


#define DELAY_US    20000 //Used to measure time

#define KBC_IRQ	        1    /**< @brief KBC IRQ line */

#define IBF		   BIT(1)    /**< @brief Input buffer full bit */

#define OBF		   BIT(0)    /**< @brief Output buffer full bit */

#define LEDs	     0xED    /**< @brief Command to switch on/off KBD LEDs */

#define ACK	         0xFA   /**< @brief KBD response when command/argument has been accepted */

#define RESEND       0xFE    /**< @brief KBD response when last byte has to be re-sent */

#define ERROR        0xEC    /**< @brief KBD response when the entire sequence has to be done again */

#define ESC			 0x81	/**< @brief escape key break code */

#define FIRST_BYTE	 0xE0	/**< @brief first byte of two scan code key */

#define BREAKCODE	BIT(7) /**< @bit that defines a breakcode */

#define PAR_ERR 	BIT(7)

#define TO_ERR		BIT(6)

/*---------------INPUT(USED TO WRITE)-------------------------*/
//assegurar que o input nao esta cheio antes de fazer sys_outb
#define KBC_CMD_REG 0x64//write commands to the KBC
#define KBC_CMD_ARG_REG 0x60//write commands' arguments to the KBC


#define KBD_CMD_REG 0x60 //write commands to the KBD
#define KBD_CMD_ARG_REG 0x60//write commands' arguments to the KBD

/*---------------OUTPUT(USED TO READ)-------------------------*/
//Return values, if any, are passed in the OUTPUT BUFFER
#define OUT_BUF		0x60

#define STAT_REG    0x64//o status register é de onde se lê o estado do KBC

#define SCN_CODE_REG 0x60//Read both make and break scancodes, sent by the keyboard
#define KBC_RET_REG 0x60//Read KBC's returns in response to the KBC commands
#define KBD_RET_REG 0x60//Read KBD's returns in response to the KBD commands


/*Keyboard-Related KBC Commands
1.  Write command to the IN_BUF
2.  Read KBD response from the OUT_BUF
3.  Write argument to the IN_BUF
4.  Read KBD response from the OUT_BUF*/

/** @defgroup keyboard keyboard
 * @{
 *
 * Functions for testing the kbd code
 */

/**
 * @brief Subscribes and enables KBC interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int KBC_subscribe_int(void );

/**
 * @brief Unsubscribes KBC interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int KBC_unsubscribe_int();

/**
 * @brief Reads from output buffer to *read
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int read_out_buf(unsigned long port, unsigned long *read);

/**
 * @brief writes write argument to input buffer
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int write_in_buf(unsigned long port, unsigned long write);

/**
 * @brief checks if input buffer is empty

 * @return Returns 0 if input buffer is EMPTY and non-zero otherwise
 */
int check_if_in_buf_empty();

/**
 * @brief checks if output buffer is full

 * @return Returns 0 if output buffer is FULL and non-zero otherwise
 */
int check_if_out_buf_full();

/**
 * @brief only returns a value when input buffer is empty

 * @return Returns 0 only when input buffer is empty
 */
int wait_until_in_buf_empty();

/**
 * @brief only returns a value when outpu buffer is full

 * @return Returns 0 only when output buffer is full
 */
int wait_until_out_buf_full();

/**
 * @brief receives an iterator and a led vector
	switches a led state acording the vector element in iterator position
 * @return Returns 0 on succsess and non-zero otherwise
 */
int change_led(unsigned short iterador,unsigned short *leds);

/**
 * @brief sends a led changing command
	the argument KBDresponse is filled with the KBD response to the command
 * @return Returns 0 on succsess and non-zero otherwise
 */
int send_led_command(unsigned long *KBDresponse);

/**
 * @brief sends the argument of a led changing command
 *  Bit 2  Caps Lock indicator
	Bit 1  Numeric Lock indicator
	Bit 0  Scroll lock indicator
	the argument *KBDresponse is filled with the KBD response to the argument sent
 * @return Returns 0 on succsess and non-zero otherwise
 */
int send_led_argument(unsigned short argument, unsigned long *KBDresponse);

/**
 * @brief it is called when KBD reponds with ACK to the LED command sent
 * sends the argument of a led changing command and reacts according to the KBD response
 * @return Returns 0 on succsess and non-zero otherwise
 */
int led_command_response_ACK(unsigned short *leds, unsigned long *KBDresponse);

/**
 * @brief it is called when KBD reponds with RESEND to the LED command sent
 * keeps sending the LED command until the KBD response changes, after that reacts to the new KBD response
 * @return Returns 0 on succsess and non-zero otherwise
 */
int led_command_response_RESEND(unsigned short *leds, unsigned long *KBDresponse);

/**
 * @brief it is called when KBD reponds with RESEND to the LED argument sent
 * keeps sending the LED argument until the KBD response changes, after that reacts to the new KBD response
 * @return Returns 0 on succsess and non-zero otherwise
 */
int led_argument_response_RESEND(unsigned short *leds, unsigned long *KBDresponse);


//reads from output buf
unsigned long readinteste();

//writes in input buf
unsigned long writeinteste(unsigned long data);

//sends command and analyzes response
int send_command(unsigned long command, unsigned long argument);
#endif /* __KEYBOARD_H */
