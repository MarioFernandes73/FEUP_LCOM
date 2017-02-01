#include <limits.h>
#include <string.h>
#include <errno.h>
#include "test5.h"
#include "pixmap.h"

static int proc_args(int argc, char **argv);
static unsigned short parse_ushort(char *str, int base);
static short parse_short(char *str, int base);
static void print_usage(char **argv);



int main(int argc, char **argv)
{
	sef_startup(); // command to initialize

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}


static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n\n"
			"\t service run %s -args \"init <unsigned short no.- mode> <unsigned short no.- delay>\"\n\n"
			"\t service run %s -args \"square <unsigned short no. - x> <unsigned short no. - y> <unsigned short no. - size> <unsigned long no. - color>\"\n\n"
			"\t service run %s -args \"line <unsigned short no. - xi> <unsigned short no. - yi> <unsigned short no. - xf> <unsigned short no. - yf> <unsigned long no. - color>\"\n\n"
			"\t service run %s -args \"xpm <unsigned short no. - xi> <unsigned short no. - yi> <char *xpm[]>\"\n\n",
			"\t service run %s -args \"move <unsigned short no. - xi> <unsigned short no. - yi> <char *xpm[]> <unsigned short no. - hor> <short no. - delta> <unsigned short no. - time>\"\n\n",
			"\t service run %s -args \"controller\"\n",
			argv[0], argv[0], argv[0], argv[0],argv[0],argv[0]);
}


static int proc_args(int argc, char **argv)
{
	unsigned short mode,delay,x,y,size,xi,yi,xf,yf,hor,time,XPM_NUMBER;
	short delta;
	unsigned long color;
	char **xpm;

	if (strncmp(argv[1], "init", strlen("init")) == 0) {
		if (argc != 4) {
			printf("vgraph: wrong no. of arguments for test_init()\n");
			return 1;
		}
		mode = parse_ushort(argv[2], 16);						/* Parses string to unsigned short */
		if (mode == USHRT_MAX)
			return 1;
		delay = parse_ushort(argv[3], 10);						/* Parses string to unsigned short */
		if (delay == USHRT_MAX)
			return 1;
		printf("vgraph::test_init(%d,%d)\n", mode,delay);
		return test_init(mode,delay);
	}

	else if (strncmp(argv[1], "square", strlen("square")) == 0) {
		if (argc != 6) {
			printf("vgraph: wrong no. of arguments for test_square()\n");
			return 1;
		}
		x = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (x == USHRT_MAX)
			return 1;
		y = parse_ushort(argv[3], 10);						/* Parses string to unsigned short */
		if (y == USHRT_MAX)
			return 1;
		size = parse_ushort(argv[4], 10);						/* Parses string to unsigned short */
		if (size == USHRT_MAX)
			return 1;
		color = parse_ushort(argv[5], 16);						/* Parses string to unsigned short */
		if (color == USHRT_MAX)
			return 1;
		printf("vgraph::test_square(%d,%d,%d,%d)\n", x,y,size,color);
		return test_square(x,y,size,color);
	}


	else if (strncmp(argv[1], "line", strlen("line")) == 0) {
		if (argc != 7) {
			printf("vgraph: wrong no. of arguments for test_line()\n");
			return 1;
		}
		xi = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (xi == USHRT_MAX)
			return 1;
		yi = parse_ushort(argv[3], 10);						/* Parses string to unsigned short */
		if (yi == USHRT_MAX)
			return 1;
		xf = parse_ushort(argv[4], 10);						/* Parses string to unsigned short */
		if (xf == USHRT_MAX)
			return 1;
		yf = parse_ushort(argv[5], 10);						/* Parses string to unsigned short */
		if (yf == USHRT_MAX)
			return 1;
		color = parse_ushort(argv[5], 16);						/* Parses string to unsigned short */
		if (color == USHRT_MAX)
			return 1;
		printf("vgraph::test_line(%d,%d,%d,%d,%d)\n", xi,yi,xf,yf,color);
		return test_line(xi,yi,xf,yf,color);
	}


	else if (strncmp(argv[1], "xpm", strlen("xpm")) == 0) {
		if (argc != 5) {
			printf("vgraph: wrong no. of arguments for test_xpm()\n");
			return 1;
		}
		x = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (x == USHRT_MAX)
			return 1;
		y = parse_ushort(argv[3], 10);						/* Parses string to unsigned short */
		if (y == USHRT_MAX)
			return 1;
		XPM_NUMBER=parse_ushort(argv[4],10);
		if(XPM_NUMBER==USHRT_MAX)
			return 1;
		if((xpm=getXpm(XPM_NUMBER))==NULL)
			return 1;
		printf("vgraph::test_xpm(%d,%d,%d)\n", x,y,XPM_NUMBER);
		return test_xpm(x,y,xpm);
	}

	else if (strncmp(argv[1], "move", strlen("move")) == 0) {
		if (argc != 8) {
			printf("vgraph: wrong no. of arguments for test_move()\n");
			return 1;
		}
		xi = parse_ushort(argv[2], 16);						/* Parses string to unsigned short */
		if (mode == USHRT_MAX)
			return 1;
		yi = parse_ushort(argv[3], 10);						/* Parses string to unsigned short */
		if (delay == USHRT_MAX)
			return 1;
		XPM_NUMBER=parse_ushort(argv[4],10);
		if(XPM_NUMBER==USHRT_MAX)
			return 1;
		if((xpm=getXpm(XPM_NUMBER))==NULL)
			return 1;
		hor = parse_ushort(argv[3], 10);						/* Parses string to unsigned short */
		if (delay == USHRT_MAX)
			return 1;
		delta = parse_ushort(argv[3], 10);						/* Parses string to unsigned short */
		if (delay == USHRT_MAX)
			return 1;
		time = parse_ushort(argv[3], 10);						/* Parses string to unsigned short */
		if (delay == USHRT_MAX)
			return 1;
		printf("vgraph::test_init(%d,%d,%d,%d,%d,%d)\n", xi, yi, XPM_NUMBER, hor, delta, time);
		return test_move(xi, yi, xpm, hor, delta, time);
	}

	else if (strncmp(argv[1], "controller", strlen("controller")) == 0) {
		if (argc != 2) {
			printf("vgraph: wrong no. of arguments for test_controller()\n");
			return 1;
		}
		printf("vgraph::test_init()\n");
		return test_controller();
	}


	else {
		printf("vgraph: %s - no valid function!\n", argv[1]);
		return 1;
	}
}


static unsigned short parse_ushort(char *str, int base)
{
	char *endptr;
	unsigned short val;

	/* Convert string to unsigned short */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == USHRT_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return USHRT_MAX;
	}

	if (endptr == str) {
		printf("mouse: parse_ushrt: no digits were found in %s\n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}

static short parse_short(char *str, int base)
{
	char *endptr;
	short val;

	/* Convert string to unsigned short */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == SHRT_MAX) || (errno != 0 && val == 0) || (errno == ERANGE && val == SHRT_MIN) ) {
		perror("strtoul");
		return SHRT_MAX;
	}

	if (endptr == str) {
		printf("mouse: parse_shrt: no digits were found in %s\n", str);
		return SHRT_MAX;
	}

	/* Successful conversion */
	return val;
}

