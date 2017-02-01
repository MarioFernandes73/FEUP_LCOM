#include "bitmap.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include "stdio.h"
#include "video_gr.h"
#include "timer.h"
#include "gamevariables.h"

//special thanks to Henrique Ferrolho for providing us the code to work with bitmaps(loadBitmap, drawBitmap and deleteBitmap),
//all credit goes to him

Bitmap* loadBitmap(const char* filename) {
	// allocating necessary size
	Bitmap* bmp = (Bitmap*) malloc(sizeof(Bitmap));

	// open filename in read binary mode
	FILE *filePtr;
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// read the bitmap file header
	BitmapFileHeader bitmapFileHeader;
	fread(&bitmapFileHeader, 2, 1, filePtr);

	// verify that this is a bmp file by check bitmap id
	if (bitmapFileHeader.type != 0x4D42) {
		fclose(filePtr);
		return NULL;
	}

	int rd;
	do {
		if ((rd = fread(&bitmapFileHeader.size, 4, 1, filePtr)) != 1)
			break;
		if ((rd = fread(&bitmapFileHeader.reserved, 4, 1, filePtr)) != 1)
			break;
		if ((rd = fread(&bitmapFileHeader.offset, 4, 1, filePtr)) != 1)
			break;
	} while (0);

	if (rd = !1) {
		fprintf(stderr, "Error reading file\n");
		exit(-1);
	}

	// read the bitmap info header
	BitmapInfoHeader bitmapInfoHeader;
	fread(&bitmapInfoHeader, sizeof(BitmapInfoHeader), 1, filePtr);

	// move file pointer to the begining of bitmap data
	fseek(filePtr, bitmapFileHeader.offset, SEEK_SET);

	// allocate enough memory for the bitmap image data
	unsigned char* bitmapImage = (unsigned char*) malloc(
			bitmapInfoHeader.imageSize);

	// verify memory allocation
	if (!bitmapImage) {
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// read in the bitmap image data
	fread(bitmapImage, bitmapInfoHeader.imageSize, 1, filePtr);

	// make sure bitmap image data was read
	if (bitmapImage == NULL) {
		fclose(filePtr);
		return NULL;
	}

	// close file and return bitmap image data
	fclose(filePtr);

	bmp->bitmapData = bitmapImage;
	bmp->bitmapInfoHeader = bitmapInfoHeader;

	return bmp;
}

void drawBitmap(Bitmap* bmp, int x, int y, Alignment alignment) {
	if (bmp == NULL)
		return;

	int width = bmp->bitmapInfoHeader.width;
	int drawWidth = width;
	int height = bmp->bitmapInfoHeader.height;

	if (alignment == ALIGN_CENTER)
		x -= width / 2;
	else if (alignment == ALIGN_RIGHT)
		x -= width;

	if (x + width < 0 || x > getHorResolution() || y + height < 0
			|| y > getVerResolution())
		return;

	int xCorrection = 0;
	if (x < 0) {
		xCorrection = -x;
		drawWidth -= xCorrection;
		x = 0;

		if (drawWidth > getHorResolution())
			drawWidth = getHorResolution();
	} else if (x + drawWidth >= getHorResolution()) {
		drawWidth = getHorResolution() - x;
	}

	char* bufferStartPos;
	char* imgStartPos;

	int i;
	for (i = 0; i < height; i++) {
		int pos = y + height - 1 - i;

		if (pos < 0 || pos >= getVerResolution())
			continue;

		bufferStartPos = getGraphicsBuffer();
		bufferStartPos += x * 2 + pos * getHorResolution() * 2;

		imgStartPos = bmp->bitmapData + xCorrection * 2 + i * width * 2;

		memcpy(bufferStartPos, imgStartPos, drawWidth * 2);
	}
}

void deleteBitmap(Bitmap* bmp) {
	if (bmp == NULL)
		return;

	free(bmp->bitmapData);
	free(bmp);
}

void get_bitmaps()
{
	//BLACK BAR TO CREATE A DARK SCREEN
	bitmap_black_bar = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/barrapreta.bmp");

	bitmap_black_square = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/black_square.bmp");
	//MAIN MENU
	bitmap_welcome = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/welcome.bmp");
	bitmap_to = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/to.bmp");
	bitmap_simonsays = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/simonsays.bmp");
	bitmap_start = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/start.bmp"); //142x142
	bitmap_exit = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/exit.bmp"); //142x142
	//GAME BOARD
	bitmap_triangle = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/triangulo.bmp"); //128x116
	bitmap_circle = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/circulo.bmp"); //130x128
	bitmap_star = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/estrela.bmp"); //134x130
	bitmap_square = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/quadrado.bmp"); //128x128
	bitmap_vertical_white_bar = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/barrabrancavertical.bmp");
	bitmap_horizontal_white_bar = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/barrabrancahorizontal.bmp");
	bitmap_last_input = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/last_input.bmp");
	//KEYS
	bitmap_a_key = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/a_key.bmp");
	bitmap_w_key = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/w_key.bmp");
	bitmap_s_key = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/s_key.bmp");
	bitmap_d_key = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/d_key.bmp");
	//TURNS TRANSITION
	bitmap_player1 = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/player1.bmp");
	bitmap_player2 = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/player2.bmp");
	bitmap_cpu = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/cpu.bmp");
	bitmap_turn = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/turn.bmp");
	//ROUND
	bitmap_round = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/round.bmp");
	bitmap_number_0 = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/number_0.bmp");
	bitmap_number_1 = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/number_1.bmp");
	bitmap_number_2 = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/number_2.bmp");
	bitmap_number_3 = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/number_3.bmp");
	bitmap_number_4 = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/number_4.bmp");
	bitmap_number_5 = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/number_5.bmp");
	bitmap_number_6 = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/number_6.bmp");
	bitmap_number_7 = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/number_7.bmp");
	bitmap_number_8 = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/number_8.bmp");
	bitmap_number_9 = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/number_9.bmp");
	//SUCCESSFUL COPY
	bitmap_congratulations = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/congratulations.bmp");
	bitmap_nice = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/nice.bmp");
	bitmap_plagiarism = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/plagiarism.bmp");
	bitmap_happy_face = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/happy.bmp");
	//FAILED COPY
	bitmap_ups = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/ups.bmp");
	bitmap_you = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/you.bmp");
	bitmap_failed = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/failed.bmp");
	bitmap_sad_face = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/sad.bmp");
	//MOUSE CURSOR
	bitmap_cursor = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/cursor.bmp");
	//THANKS FOR PLAYING
	bitmap_thanks = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/thanks.bmp");
	bitmap_for = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/for.bmp");
	bitmap_playing = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/playing.bmp");

	bitmap_final_score = loadBitmap("/home/lcom/lcom1617-t4g03/proj/src/images/final_score.bmp");
}

void draw_black_screen()
{
	int y = 0;
	while(y<getVerResolution())
	{
		drawBitmap(bitmap_black_bar, 0, y, ALIGN_LEFT);
		y += 100; //100 is the black bar's height
	}
}

void draw_main_menu()
{
	draw_black_screen();
	drawBitmap(bitmap_welcome, 180, 25, ALIGN_LEFT);
	drawBitmap(bitmap_to, 350, 157, ALIGN_LEFT);
	drawBitmap(bitmap_simonsays, 135, 287, ALIGN_LEFT);
	drawBitmap(bitmap_start, 210, 425, ALIGN_LEFT);
	drawBitmap(bitmap_exit, 450, 425, ALIGN_LEFT);
}

void draw_board(int round)
{
	draw_black_screen();
	drawBitmap(bitmap_triangle, 210, 170, ALIGN_LEFT);
	drawBitmap(bitmap_circle, 340, 300, ALIGN_LEFT);
	drawBitmap(bitmap_star, 210, 440, ALIGN_LEFT);
	drawBitmap(bitmap_square, 65, 305, ALIGN_LEFT);
	drawBitmap(bitmap_vertical_white_bar, 530, 125, ALIGN_LEFT);
	drawBitmap(bitmap_horizontal_white_bar, 0, 125, ALIGN_LEFT);
	drawBitmap(bitmap_last_input, 545, 160, ALIGN_LEFT);

	//DRAW ROUND
	drawBitmap(bitmap_round, 100, 12, ALIGN_LEFT);

	if(round==10)
	{
		draw_number(1,570,12);
		draw_number(0,620,12);
	}
	else
	{
		draw_number(round,600,12);
	}

}

void draw_player1_turn()
{
	draw_black_screen();
	drawBitmap(bitmap_player1, 150, 150, ALIGN_LEFT);
	drawBitmap(bitmap_turn, 250, 350, ALIGN_LEFT);
}

void draw_player2_turn()
{
	draw_black_screen();
	drawBitmap(bitmap_player2, 150, 150, ALIGN_LEFT);
	drawBitmap(bitmap_turn, 250, 350, ALIGN_LEFT);
}

void draw_successful_copy()
{
	draw_black_screen();
	drawBitmap(bitmap_congratulations, 50, 50, ALIGN_LEFT);
	drawBitmap(bitmap_nice, 320, 200, ALIGN_LEFT);
	drawBitmap(bitmap_plagiarism, 170, 350, ALIGN_LEFT);
	drawBitmap(bitmap_happy_face, 400, 450, ALIGN_LEFT);
}

void draw_failed_copy()
{
	draw_black_screen();
	drawBitmap(bitmap_ups, 330, 150, ALIGN_LEFT);
	drawBitmap(bitmap_you, 200, 280, ALIGN_LEFT);
	drawBitmap(bitmap_failed, 400, 280, ALIGN_LEFT);
	drawBitmap(bitmap_sad_face, 400, 450, ALIGN_LEFT);
}

void draw_thanks_for_playing()
{
	draw_black_screen();
	drawBitmap(bitmap_thanks, 230, 50, ALIGN_LEFT);
	drawBitmap(bitmap_for, 320, 200, ALIGN_LEFT);
	drawBitmap(bitmap_playing, 210, 350, ALIGN_LEFT);
	drawBitmap(bitmap_happy_face, 400, 450, ALIGN_LEFT);
}
void draw_input(int input)//desenha uma jogada
{
	drawBitmap(bitmap_black_square, 600, 300, ALIGN_LEFT);

	switch(input)
	{
	//KEYS
	case A_KEY:
		drawBitmap(bitmap_a_key, 600, 300, ALIGN_LEFT);
		break;
	case W_KEY:
		drawBitmap(bitmap_w_key, 600, 300, ALIGN_LEFT);
		break;
	case S_KEY:
		drawBitmap(bitmap_s_key, 600, 300, ALIGN_LEFT);
		break;
	case D_KEY:
		drawBitmap(bitmap_d_key, 600, 300, ALIGN_LEFT);
		break;

		//FIGURES
	case TRIANGLE:
		drawBitmap(bitmap_triangle, 610, 300, ALIGN_LEFT);
		break;
	case CIRCLE:
		drawBitmap(bitmap_circle, 610, 300, ALIGN_LEFT);
		break;
	case STAR:
		drawBitmap(bitmap_star, 610, 300, ALIGN_LEFT);
		break;
	case SQUARE:
		drawBitmap(bitmap_square, 610, 300, ALIGN_LEFT);
		break;

	case ASCENDENT_SLOPE:
		drawBitmap(bitmap_cursor, X_IMG_COORD, Y_IMG_COORD, ALIGN_LEFT);
		X_IMG_COORD+=CURSOR_SPEED;
		Y_IMG_COORD-=CURSOR_SPEED;
		break;
	case DESCENDENT_SLOPE:
		drawBitmap(bitmap_cursor, X_IMG_COORD, Y_IMG_COORD, ALIGN_LEFT);
		X_IMG_COORD-=CURSOR_SPEED;
		Y_IMG_COORD+=CURSOR_SPEED;
		break;
	}
}

void draw_cursor(int x, int y)
{
	drawBitmap(bitmap_cursor, x, y, ALIGN_LEFT);
}

void draw_score(int score)
{
	draw_black_screen();
	drawBitmap(bitmap_final_score, 100, 200, ALIGN_LEFT);
	draw_number((score/10)%10, 320, 350);
	draw_number((score%10),400,350);
}

void draw_number(int number, int x, int y)
{
	switch(number)
	{
	case 0:
		drawBitmap(bitmap_number_0, x, y, ALIGN_LEFT);
		break;
	case 1:
		drawBitmap(bitmap_number_1, x, y, ALIGN_LEFT);
		break;
	case 2:
		drawBitmap(bitmap_number_2, x, y, ALIGN_LEFT);
		break;
	case 3:
		drawBitmap(bitmap_number_3, x, y, ALIGN_LEFT);
		break;
	case 4:
		drawBitmap(bitmap_number_4, x, y, ALIGN_LEFT);
		break;
	case 5:
		drawBitmap(bitmap_number_5, x, y, ALIGN_LEFT);
		break;
	case 6:
		drawBitmap(bitmap_number_6, x, y, ALIGN_LEFT);
		break;
	case 7:
		drawBitmap(bitmap_number_7, x, y, ALIGN_LEFT);
		break;
	case 8:
		drawBitmap(bitmap_number_8, x, y, ALIGN_LEFT);
		break;
	case 9:
		drawBitmap(bitmap_number_9, x, y, ALIGN_LEFT);
		break;
	}
}


