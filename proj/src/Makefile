# Makefile for proj

COMPILER_TYPE= gnu

CC= gcc

PROG= simonsays
SRCS= simonsays.c game.c vbe.c video_gr.c keyboard.c timer.c kbc.c mouse.c bitmap.c

CCFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys 

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>