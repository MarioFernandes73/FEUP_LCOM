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
static char *double_buffer;

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
	unsigned int vram_base = VBE_mode_info.PhysBasePtr;  /*VRAM�s physical addresss*/
	unsigned int vram_size = h_res * v_res * bits_per_pixel/8;  /*VRAM�s size, but you can usethe frame-buffer size, instead*/
	/*Allow memory mapping*/
	mr.mr_base = (phys_bytes) vram_base;
	mr.mr_limit = mr.mr_base + vram_size;
	if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
		panic("sys_privctl (ADD_MEM) failed: %d\n", r);

	/*Map memory*/
	video_mem = vm_map_phys(SELF, (void*) mr.mr_base, vram_size);
	if(video_mem == MAP_FAILED)
		panic("couldn�t map video memory");

	(double_buffer = malloc(h_res * v_res * bits_per_pixel / 8));

	return video_mem;
}

void *vg_init(unsigned short mode)
{
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

unsigned getHorResolution()
{
	return h_res;
}

unsigned getVerResolution()
{
	return v_res;
}

char *getGraphicsBuffer()
{
	return double_buffer;
}

void swap_buffer()
{
	memcpy(video_mem, double_buffer, h_res * v_res * bits_per_pixel / 8);
}

