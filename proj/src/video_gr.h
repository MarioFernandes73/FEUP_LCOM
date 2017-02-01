#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H


/*--------------GRAPHICS MODES-----------------*/
#define SIMONSAYS_RESOLUTION			0x114 /**< @brief The game's video mode(800x600) */


/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief maps the VRAM address
 *
 * @param mode video mode to wich the graphics will be set
 */
void *map_VRAM(unsigned short mode);


/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen, 
 *  and the number of colors
 * 
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *vg_init(unsigned short mode);

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * 
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

/**
* @brief Gives the current video horizontal resolution
*
* @return current video horizontal resolution
*/
unsigned getHorResolution();

/**
* @brief Gives the current video vertical resolution
*
* @return current video vertical resolution
*/
unsigned getVerResolution();

/**
* @brief Gives the double buffer's address
*
* @return double buffer's address
*/
char *getGraphicsBuffer();

/**
* @brief Sends the double buffer content to the VRAM
*
*/
void swap_buffer();

 /** @} end of video_gr */
 
#endif /* __VIDEO_GR_H */
