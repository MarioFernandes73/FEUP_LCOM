#ifndef __VIDEO_GR_H
#define __VIDEO_GR_H


/*--------------GRAPHICS MODES-----------------*/
#define RESOLUTION_480p 				0x101
#define RESOLUTION_600p					0x103
#define RESOLUTION_768p 				0x105
#define RESOLUTION_1024p				0x107
/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */
 
void *map_VRAM(unsigned short mode);

void vg_set_pixel(char* base_adress, unsigned short x, unsigned short y, unsigned long color);

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


void vg_draw_rectangle(char * address,unsigned short x, unsigned short y, unsigned short height, unsigned short width, unsigned long color);

void vg_draw_xpm(char * address, unsigned short x, unsigned short y, unsigned short width, unsigned short height, char* xpm);

void draw_line(char* virtual_address, unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color);

unsigned short round(double x);

 /** @} end of video_gr */
 
#endif /* __VIDEO_GR_H */
