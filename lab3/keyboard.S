#include "keyboard.h"

.data

makecode:	.string "makecode: 0x%X"
breakcode:	.string "breakcode: 0x%X"
paragrafo:  .string "\n"

.lcomm two_byte_code, 0 #é colocado a um quando estamos perante um scancode de dois bytes#

.text
_kbd_handler_ass:
						in $SCN_CODE_REG, %dh  #lê do output buffer o scan code e guarda em ah#
						cmp two_byte_code, $1  #verifica se este scancode é segundo de um two byte scan code
						je second_byte
						movb %dh, %dl           #cria copia do ah em dl#
						and %dl, $BREAKCODE
						cmp %dl, $BREAKCODE     #ve se o bit do break code esta ativo#
						jne makecode_routine
						#breakcode
						cmp %dh, $FIRST_BYTE     #ve se ah é igual ao first byte (scan code de dois bytes)#
						je two_bytes_breakcode
						#one byte breakcode
						.IF(%dh=$ESC)            #se for breakcode do esc, é retornado o valor 1#
esc_key:				mov $0, two_byte_code    #colaca two_byte_code a 0 para sabermos que o proximo byte nao pertence a um two byte scancode#
						mov $1, %eax
						print breakcode, %dh e paragrafo
						jump fim
						.ELSE
one_byte_breakcode:		mov $0, two_byte_code    #colaca two_byte_code a 0 para sabermos que o proximo byte nao pertence a um two byte scancode#
						mov $0, %eax			 #se nao for breakcode do esc, é retornado o valor 0#
						print breakcode, %dh e paragrafo
						jump fim
two_bytes_breakcode: 	mov $1, two_byte_code    #colaca two_byte_code a 1 para sabermos que o proximo byte pertence a um two byte scancode#
						mov $0, %eax
						print breakcode, $FIRST_BYTE
						jump fim
one_byte_makecode:		mov $0, two_byte_code    #colaca two_byte_code a 0 para sabermos que o proximo byte nao pertence a um two byte scancode#
						mov $0, %eax			 #se nao for breakcode do esc, é retornado o valor 0#
						print makecode, %dh e paragrafo
						jump fim
two_bytes_makecode: 	mov $1, two_byte_code    #colaca two_byte_code a 1 para sabermos que o proximo byte pertence a um two byte scancode#
						mov $0, %eax
						print makecode, $FIRST_BYTE
						jump fim
second_byte: 			mov $0, two_byte_code    #colaca two_byte_code a 0 para sabermos que o proximo byte nao pertence a um two byte scancode#
						mov $0, %eax
						print do %dh, paragrafo
fim:					ret
