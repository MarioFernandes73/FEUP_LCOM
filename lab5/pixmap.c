#include "pixmap.h"
#include <stdlib.h>

char** getXpm(unsigned short id)
{
	switch(id)
	{
	case 1: return pic1;
	break;
	case 2: return pic2;
	break;
	case 3: return cross;
	break;
	case 4: return pic3;
	break;
	case 5: return penguin;
	break;
	default : return NULL;
	break;
	}
}
