


typedef struct {
double x, y; // current position
int width, height; // dimensions
double xspeed, yspeed; // current speed
char *map; // the pixmap
} Sprite;



void destroy_sprite(Sprite *sp);

Sprite *create_sprite(char *pic[], int x, int y, double xspeed, double yspeed, unsigned short HRES, unsigned short VRES);

int animate_sprite(Sprite *sp);
