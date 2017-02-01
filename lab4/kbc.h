#ifndef __KBC_H
#define __KBC_H

#include <minix/syslib.h>
#include <minix/drivers.h>
#include <minix/sysutil.h> //Used to emasure time
#include <minix/com.h> //Used for interrupt handling (while loop)

#define BIT(n) (0x01<<(n))

#define TRIES 		50
#define DELAY_US    20000	/**< @brief used to measure time */

/*-------PORTS-----------------------------*/
#define STAT_REG    0x64
#define CTRL_REG 	0x64
#define IN_BUF		0x60
#define OUT_BUF		0x60

/*-------STATUS REGISTER BITS---------------*/
#define PARITY		BIT(7)
#define TIMEOUT		BIT(6)
#define AUX			BIT(5)
#define INH			BIT(4)
#define A2			BIT(3)
#define SYS			BIT(2)
#define IBF		   	BIT(1)    /**< @brief Input buffer full bit */
#define OBF		   	BIT(0)    /**< @brief Output buffer full bit */

/**
 * @brief Reads the KBC status byte and checks if the output buffer is full
 * so that we read from it
 *

 * @return Returns 0 on success and non-zero otherwise
 */
int read_status_to_read();

/**
 * @brief Reads the KBC status byte and checks if the input buffer is empty
 * so that we write to it
 *

 * @return Returns 0 on success and non-zero otherwise
 */
int read_status_to_write();


#endif
