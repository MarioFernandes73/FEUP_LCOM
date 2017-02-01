#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>

#include "vbe.h"
#include "lmlib.h"

#define LINEAR_MODEL_BIT 14

#define PB2BASE(x) (((x) >> 4) & 0x0F000)
#define PB2OFF(x) ((x) & 0x0FFFF)

int vbe_get_mode_info(unsigned short mode, vbe_mode_info_t *vmi_p) {
  
	lm_init();

	mmap_t buf;
	if (lm_alloc( sizeof (vbe_mode_info_t),&buf) ==NULL)
		return -1;

	struct reg86u r;

	r.u.w.ax = 0x4F01; /* VBE get mode info */
	/* translate the buffer linear address to a far pointer */
	r.u.w.es = PB2BASE(buf.phys); /* set a segment base */
	r.u.w.di = PB2OFF(buf.phys); /* set the offset accordingly */
	r.u.w.cx = mode;
	r.u.b.intno = 0x10;
	if( sys_int86(&r) == OK ) {
		/* call BIOS */

			*vmi_p= *((vbe_mode_info_t *) buf.virtual );

			printf("%d\n",vmi_p->XResolution);
			lm_free(&buf);
			return 1;
	}
  
  return -2;
}


int vbe_get_controller_info(vbe_controller_info_t *vci_p)
{
	lm_init();

	mmap_t buf;
	if (lm_alloc( sizeof (vbe_controller_info_t),&buf) ==NULL)
		return -1;

	struct reg86u r;

	r.u.w.ax = VBE_RETURN_CONTROLLER; /* VBE get controller info */
	/* translate the buffer linear address to a far pointer */
	r.u.w.es = PB2BASE(buf.phys); /* set a segment base */
	r.u.w.di = PB2OFF(buf.phys); /* set the offset accordingly */
	r.u.b.intno = ACCESS_VBE;
	if( sys_int86(&r) == OK ) {
		/* call BIOS */

			*vci_p= *((vbe_controller_info_t *) buf.virtual );
			lm_free(&buf);
			return 1;
	}

  return -2;
}



