#include <limits.h>
#include <string.h>
#include <errno.h>
#include "test4.h"

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
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"packet <unsigned short no.- cnt>\"\n"
			"\t service run %s -args \"async <unsigned short no. - idle_time>\"\n"
			"\t service run %s -args \"config\"\n"
			"\t service run %s -args \"gesture <short no. -  length>\"\n",
			argv[0], argv[0], argv[0], argv[0]);
}


static int proc_args(int argc, char **argv)
{
	unsigned short cnt, idle_time;
	short length;

	if (strncmp(argv[1], "packet", strlen("packet")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments for mouse_test_packet()\n");
			return 1;
		}
		cnt = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (cnt == USHRT_MAX)
			return 1;
		printf("mouse::mouse_test_packet(%d)\n", cnt);
		return mouse_test_packet(cnt);
	}
	else if (strncmp(argv[1], "async", strlen("async")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments for mouse_test_async()\n");
			return 1;
		}
		idle_time = parse_ushort(argv[2], 10);						/* Parses string to unsigned short */
		if (idle_time == USHRT_MAX)
			return 1;
		printf("mouse::mouse_test_async(%d)\n", idle_time);
		return mouse_test_async(idle_time);
	}
	else if (strncmp(argv[1], "config", strlen("config")) == 0) {
		if (argc != 2) {
			printf("mouse: wrong no of arguments for mouse_test_config()\n");
			return 1;
		}
		printf("mouse::mouse_test_config()\n");
		return mouse_test_config();
	}
	else if (strncmp(argv[1], "gesture", strlen("gesture")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments for mouse_test_gesture()\n");
			return 1;
		}
		length = parse_short(argv[2], 10);						/* Parses string to short */
		if (length == SHRT_MAX || length == SHRT_MIN)
			return 1;
		printf("mouse::mouse_test_gesture(%d)\n", length);
		return mouse_test_gesture(length);
	}
	else {
		printf("mouse: %s - no valid function!\n", argv[1]);
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

