#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

#include "vbe.h"
#include "video_gr.h"

/* Constants for VBE 0x105 mode */

/* The physical address may vary from VM to VM.
 * At one time it was 0xD0000000
 *  #define VRAM_PHYS_ADDR    0xD0000000 
 * Currently on lab B107 is 0xF0000000
 * Better run my version of lab5 as follows:
 *     service run `pwd`/lab5 -args "mode 0x105"
 */
#define VRAM_PHYS_ADDR		0xE0000000
#define H_RES             	1024
#define V_RES		  		768
#define BITS_PER_PIXEL	  	8

/* Private global variables */

static char *video_mem;		/* Process address to which VRAM is mapped */

static unsigned h_res;		/* Horizontal screen resolution in pixels */
static unsigned v_res;		/* Vertical screen resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

void *map_VRAM(unsigned short mode)
{
	vbe_mode_info_t VBE_mode_info;
		if(vbe_get_mode_info(mode, &VBE_mode_info)!=1)//se retornar diferente de 1 significa que a informacao nao foi obtida com sucesso
			return NULL;

		h_res = VBE_mode_info.XResolution;
		v_res = VBE_mode_info.YResolution;
		bits_per_pixel = VBE_mode_info.BitsPerPixel;

		int r;
		struct mem_range mr;
		unsigned int vram_base = VBE_mode_info.PhysBasePtr;  /*VRAM’s physical addresss*/
		unsigned int vram_size = h_res * v_res * bits_per_pixel/8;  /*VRAM’s size, but you can usethe frame-buffer size, instead*/
		/*Allow memory mapping*/
		mr.mr_base = (phys_bytes) vram_base;
		mr.mr_limit = mr.mr_base + vram_size;
		if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);

		/*Map memory*/
		video_mem = vm_map_phys(SELF, (void*) mr.mr_base, vram_size);
		if(video_mem == MAP_FAILED)
		panic("couldn’t map video memory");

		return video_mem;
}

void vg_set_pixel(char* base_adress, unsigned short x, unsigned short y, unsigned long color)
{
	char * begin = base_adress;
	if(x>=V_RES || y>=H_RES)
	{
		printf("PIXEL COORDINATES EXCEEDED LIMIT \n");
	}
	begin += y*H_RES + x;
	*begin = color;
}

void *vg_init(unsigned short mode)
{
	if(mode!=RESOLUTION_480p && mode!=RESOLUTION_600p && mode!=RESOLUTION_768p && mode!=RESOLUTION_1024p)
	{
		printf("MODE NOT COMPATIBLE WITH SYSTEM \n");
		return NULL;
	}


	struct reg86u r;
	r.u.w.ax = VBE_SET_MODE; // VBE call, function 02 -- set VBE mode
	r.u.w.bx = 1<<14|mode; // set bit 14: linear framebuffer
	r.u.b.intno = ACCESS_VBE;
	if( sys_int86(&r) != OK ) {
		printf("set_vbe_mode: sys_int86() failed \n");
		return NULL;
	}
	return map_VRAM(mode);
}

int vg_exit() {
  struct reg86u reg86;

  reg86.u.b.intno = 0x10; /* BIOS video services */

  reg86.u.b.ah = 0x00;    /* Set Video Mode function */
  reg86.u.b.al = 0x03;    /* 80x25 text mode*/

  if( sys_int86(&reg86) != OK ) {
      printf("\tvg_exit(): sys_int86() failed \n");
      return 1;
  } else
      return 0;
}

void vg_draw_rectangle(char * address,unsigned short x, unsigned short y, unsigned short height, unsigned short width, unsigned long color)
{
	unsigned short TempX, TempY;
	for (TempX=0; TempX < width; TempX++)
	{
		for (TempY=0; TempY < height; TempY++)
		{
			vg_set_pixel(address,TempX+x, TempY+y, color);
		}
	}
}

void vg_draw_xpm(char * address, unsigned short x, unsigned short y, unsigned short width, unsigned short height, char* xpm)
{
	unsigned short TempX, TempY;
	for (TempX=0; TempX < width; TempX++)
	{
		for (TempY=0; TempY < height; TempY++)
		{
			vg_set_pixel(address,TempX+x, TempY+y, *(xpm + (TempX + TempY * width)));
		}
	}
}

void vg_draw_line(char* virtual_address, unsigned short xi, unsigned short yi, unsigned short xf, unsigned short yf, unsigned long color)
{
	//this is an adaptation from a line drawing algorithm made by Jack Elton Bresenham
	//this algorithm avoids any divisions and problems with float/doubles
	short dx, dy,slope,incE, incNE,d,x,y;

	if(xi>xf)	//if initial x higher than final x, swap them so we always have a line going from left to right (or vertical)
	{
		unsigned short temp = xi;
		xi=xf;
		xf=temp;
	}

	dx=xf-xi;		//x's slope
	dy=yf-yi;		//y's slope

	if(dy<0)		//if y's slope is negative convert it to positive so we always picture a line going upwards (or horizontal)
	{
		slope = -1;
		dy=-dy;
	}
	else
		slope=1;

	  incE = 2 * dy;
	  incNE = 2 * dy - 2 * dx;
	  d = 2 * dy - dx;
	  y = yi;

	  for (x = xi; x <= xf; x++)	//cicle to color the pixels from top to bottom
	  {
		  vg_set_pixel(virtual_address, x, y,color);
		   if (d <= 0)
		   {
		  d += incE;
		  }
		  else
		  {
		  d += incNE;
		  y += slope;
		  }
	  }


	/*double y_coordinate;
	double x_coordinate;
	double xinicial=xi;
	double yinicial=yi;
	double xfinal=xf;
	double yfinal=yf;
	double slope = abs((yfinal-yinicial)/(xfinal-xinicial));
	if(xf==xi)
	{
		if(yf>yi)
		{
			y_coordinate=yi;
			yfinal=yf;
		}
		else
		{
			y_coordinate=yf;
			yfinal=yi;
		}
		while(y_coordinate<=yfinal)
		{
			set_pixel(virtual_adress, xi, y_coordinate, color);
			y_coordinate ++;
		}
	}
	else if( (xf>xi && yf>=yi) || (xf<xi && yf<=yi) ) // movimento com direcao da bissetriz quadrantes impares ou reta vertical                                                      .
	{
		if(xf>xi && yf>=yi)
		{
			xinicial=xi;
			yinicial=yi;
			xfinal=xf;
			yfinal=yf;
		}
		else
		{
			xinicial=xf;
			yinicial=yf;
			xfinal=xi;
			yfinal=yi;
		}
		y_coordinate=yinicial;
		x_coordinate=xinicial;

		if(slope>=1) //funciona sempre
		{
			while(y_coordinate<=yfinal)
			{
				unsigned short x_to_send = round( ((y_coordinate - yinicial)/slope) + xinicial );
				unsigned short y_to_send = y_coordinate;
				set_pixel(virtual_adress, x_to_send, y_to_send, color);
				y_coordinate++;
			}
		}
		if(slope<1) //nao funciona, pois arredonda para zero
		{
			while(x_coordinate<=xfinal)
			{
				unsigned short x_to_send = x_coordinate;
				unsigned short y_to_send = round( ((x_coordinate - xinicial)*slope) + yinicial );
				set_pixel(virtual_adress, x_to_send, y_to_send, color);
				x_coordinate++;
			}
		}
	}
	else if( (xi<xf && yi>yf) || (xi>xf && yi<yf) ) // movimento com direcao da bissetriz quadrantes pares
	{
		if(xi<xf && yi>yf)
		{
			xinicial=xi;
			yinicial=yi;
			xfinal=xf;
			yfinal=yf;
		}
		else
		{
			xinicial=xf;
			yinicial=yf;
			xfinal=xi;
			yfinal=yi;
		}
		y_coordinate=yinicial;
		x_coordinate=xinicial;
		if(slope>=1) //funciona apenas quando o declive é inteiro
		{
			while(y_coordinate>=yfinal)
			{
				unsigned short x_to_send = round((yinicial - y_coordinate)/slope) + xinicial;
				unsigned short y_to_send = y_coordinate;
				set_pixel(virtual_adress, x_to_send, y_to_send, color);
				y_coordinate--;
			}
		}
		if(slope<1)//nao funciona pois arredonda sempre para zero
		{
			while(x_coordinate<=xfinal)
			{
				unsigned short x_to_send = x_coordinate;
				unsigned short y_to_send = round( yinicial - ((x_coordinate - xinicial)*slope) );
				set_pixel(virtual_adress, x_to_send, y_to_send, color);
				x_coordinate++;
			}
		}
	}*/
}

unsigned short round(double x)
{
	unsigned short retorno;
	if( (x - floor(x)) >= 0.5)
	{
		retorno = floor(x)+1;
		return retorno;
	}
	retorno = floor(x);
	return retorno;
}



