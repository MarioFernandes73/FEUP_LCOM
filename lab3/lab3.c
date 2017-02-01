#include <limits.h>
#include <string.h>
#include <errno.h>
#include "test3.h"

static int proc_args(int argc, char **argv);
static unsigned short parse_ushrt(char *str, int base);
int parse_ushrt_array(char* str, unsigned short n, unsigned short * leds);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	sef_startup(); // command to initialize

	/* Enable IO-sensitive operations for ourselves */
    sys_enable_iop(SELF);
	
	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"scan <decimal no.- ass (0 for C, other for ass)>\"\n"
			"\t service run %s -args \"leds <decimal no. - n (number of elements on the second argument), array with decimal no. between 0 and 2 - leds (ex: 0 1 0 2 )>\"\n"
			"\t service run %s -args \"timedscan <decimal no. - n (number of seconds without scan)>\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned short ass, n;
	int size=0;
	unsigned int i;


	if (strncmp(argv[1], "scan", strlen("scan")) == 0) {
		if (argc != 3) {
			printf("keyboard: wrong no. of arguments for kbd_test_scan()\n");
			return 1;
		}
		ass = parse_ushrt(argv[2], 10);						/* Parses string to unsigned long */
		if (ass == USHRT_MAX)
			return 1;
		printf("keyboard::kbd_test_scan(%d)\n", ass);
		return kbd_test_scan(ass);
	}
	else if (strncmp(argv[1], "leds", strlen("leds")) == 0) {
		if (argc != 4) {
			printf("keyboard: wrong no. of arguments for kbd_test_leds()\n");
			return 1;
		}
		n = parse_ushrt(argv[2], 10);/* Parses string to unsigned long */

		if (n == USHRT_MAX)
			return 1;

		unsigned short leds[n];
		
		if(!parse_ushrt_array(argv[3],n,leds))
			{
				return 1;
			}

		printf("keyboard::kbd_test_leds(%d, [ ",n);

		for (i=0; i<n; i++)
			{
				printf("%d ",leds[i]);
			}
		printf("])\n");
		return kbd_test_leds(n,leds);
	}
	else if (strncmp(argv[1], "timedscan", strlen("timedscan")) == 0) {
		if (argc != 3) {
			printf("keyboard: wrong no of arguments for timedscan()\n");
			return 1;
		}
		n = parse_ushrt(argv[2], 10);						/* Parses string to unsigned long */
		if (n == USHRT_MAX)
			return 1;
		printf("keyboard::kbd_test_timed_scan(%d)\n", n);
		return kbd_test_timed_scan(n);
	}
	else {
		printf("keyboard: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned short parse_ushrt(char *str, int base)
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
		printf("keyboard: parse_ushrt: no digits were found in %s\n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return val;
}


int parse_ushrt_array(char* str, unsigned short n, unsigned short * leds)
{
		unsigned short temp;
		unsigned int i;

		if(strlen(str)!=n)
		{
			printf("keyboard: Wrong n size\n");
			return 0;
		}
			for (i=0; i<n; i++)
			{
				/* Convert string to unsigned short */
				temp = str[i]-'0';
				/* Check for conversion errors */
					if (temp != 0 && temp != 1 && temp != 2) {
						printf("keyboard: %d isn't 0 1 or 2\n",temp);
						return 0;
					}
					leds[i]=temp;
			}
			return 1;

}

