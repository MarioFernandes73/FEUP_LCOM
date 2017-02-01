  /** Creates a new sprite with pixmap "pic", with specified
* position (within the screen limits) and speed;
* Does not draw the sprite on the screen
* Returns NULL on invalid pixmap.
*/

#include "sprite.h"
#include <stdlib.h>



Sprite *create_sprite(char *pic[], int x, int y,
double xspeed, double yspeed, unsigned short HRES, unsigned short VRES) {
//allocate space for the "object"
Sprite *sp = (Sprite *) malloc ( sizeof(Sprite));
if( sp == NULL )
return NULL;
// read the sprite pixmap
sp->map = read_xpm(pic, &(sp->width), &(sp->height), HRES, VRES);
if( sp->map == NULL ) {
free(sp);
return NULL;
}
sp->x=x;
sp->y=y;
sp->xspeed = xspeed;
sp->yspeed = yspeed;

printf("ASDSDSDASD%d %d %d %d", x,y,xspeed,yspeed);
return sp;
}


void destroy_sprite(Sprite *sp) {
if( sp == NULL )
return;
free(sp->map);
free(sp);
sp = NULL; // hopeless: pointer is passed by value
// should do this @ the caller
}


int animate_sprite(Sprite *sp) {
sp->x=sp->x+ sp->xspeed;
sp->y=sp->y+ sp->yspeed;
return 0;
}
/* Some useful non-visible functions */
int draw_sprite(Sprite *sp, char *base) {
	return vg_draw_xpm(base, sp->x, sp->y, sp->width, sp->height, sp->map);
}
