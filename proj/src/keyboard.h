#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h> //Used to measure time in IH
#include <minix/com.h> //Used for interrupt handling (while loop)

#define BIT(n) (0x01<<(n))

#define ESC			 129	/**< @brief escape key break code */

#define Wkey		145	/**< @brief escape key W code */

#define Skey		159	/**< @brief escape key S code */

#define Akey		158	/**< @brief escape key A code */

#define Dkey		160	/**< @brief escape key D code */

#define FIRST_BYTE	 0xE0	/**< @brief first byte of two scan code key */

#define BREAKCODE	BIT(7) /**< @bit that defines a breakcode */

#define KBD_IRQ	        1    /**< @brief KBD IRQ line */

#define HOOK_ID_BIT_KBD			1		//bit de inicializacao do hook_id do teclado


/**
 * @brief Subscribes and enables KBD interrupts
 *
 * @return Returns bit order in interrupt mask; negative value on failure
 */
int KBD_subscribe_int();

/**
 * @brief Unsubscribes KBD interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int KBD_unsubscribe_int();

/**
 * @brief The game's keyboard interrupt handler
 *
 * @return Returns a number according to the key pressed(input), each key has a correspondent number
 */
int KBD_handler();


#endif /* __KEYBOARD_H */
