#include <limits.h>
#include <string.h>
#include <errno.h>
#include "game.h"

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
			"\t service run %s -args \"simonsays\"\n",
			argv[0]);
}


static int proc_args(int argc, char **argv)
{

	if (strncmp(argv[1], "simonsays", strlen("simonsays")) == 0) {
		if (argc != 2) {
			printf("simonsays: wrong no. of arguments for the game\n");
			printf("please write service run `pwd`/simonsays -args \"simonsays" "\n");
			return 1;
		}
		printf("simonsays::simonsays()\n");
		return simonsays();
	}


	else {
		printf("simonsays: %s - no valid function!\n", argv[1]);
		return 1;
	}
}
