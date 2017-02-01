# Makefile for lab4

COMPILER_TYPE= gnu

CC= gcc

PROG= lab5
SRCS= lab5.c vbe.c test5.c video_gr.c keyboard.c timer.c kbc.c pixmap.c xpm.c sprite.c

CCFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -ldriver -lsys -llm

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>