#include "kbc.h"

int read_status_to_read()
{
	unsigned long status;
	unsigned int contador = 0;
	//READ FROM MOUSE
	while(contador < TRIES)
	{
		sys_inb(STAT_REG,&status);
		if((status & TIMEOUT) || (status & PARITY))
			contador++;	//ERRO A LER DO STATUS
		else if(status & OBF)
			break;	//OK
		tickdelay(micros_to_ticks(DELAY_US));	//DELAY
	}
	if(contador>=TRIES)
		return -1;	//DEMASIADAS TRIES A LER DO STATUS
	else
		return 0;
}

int read_status_to_write()
{
	unsigned int contador = 0;
	unsigned long status;
	while( contador < TRIES)
	{
		sys_inb(STAT_REG, &status);
		if((status & TIMEOUT) || (status & PARITY))
			contador++;	//ERRO A LER DO STATUS
		//OK
		if( !(status & IBF))
		{
			return 0;	//OK
		}
		else
			contador++;

		tickdelay(micros_to_ticks(DELAY_US));
	}
	if(contador==TRIES)
	{
		printf("WAITED TOO MUCH TIME FOR INPUT BUFFER TO BECOME EMPTY\n");
		return 1;	//ERRO
	}
	return 1;
}
