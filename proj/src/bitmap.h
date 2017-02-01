#pragma once

/** @defgroup Bitmap Bitmap
 * @{
 * Functions for manipulating bitmaps
 */

typedef enum {
    ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT
} Alignment;

/**
 * Represents a Bitmap File
 */
typedef struct {
    unsigned short type; /**< specifies the file type*/
    unsigned int size;  /**< specifies the size in bytes of the bitmap file*/
    unsigned int reserved; /**< reserved; must be 0*/
    unsigned int offset; /**<specifies the offset in bytes from the bitmapfileheader to the bitmap bits*/
} BitmapFileHeader;

/**
 * Represents the Bitmap information
 */
typedef struct {
    unsigned int size; /**< specifies the number of bytes required by the struct*/
    int width; /**< specifies width in pixels*/
    int height; /**< specifies height in pixels*/
    unsigned short planes; /**< specifies the number of color planes, must be 1*/
    unsigned short bits; /**< specifies the number of bit per pixel*/
    unsigned int compression; /**< specifies the type of compression*/
    unsigned int imageSize; /**< size of image in bytes*/
    int xResolution; /**< number of pixels per meter in x axis*/
    int yResolution; /**< number of pixels per meter in y axis*/
    unsigned int nColors; /**< number of colors used by the bitmap*/
    unsigned int importantColors; /**< number of colors that are important*/
} BitmapInfoHeader;

/**
 * Represents a Bitmap
 */
typedef struct {
    BitmapInfoHeader bitmapInfoHeader;
    unsigned char* bitmapData;
} Bitmap;

//Bitmaps
Bitmap* bitmap_welcome;
Bitmap* bitmap_to;
Bitmap* bitmap_simonsays;
Bitmap* bitmap_start;
Bitmap* bitmap_exit;
Bitmap* bitmap_triangle;
Bitmap* bitmap_circle;
Bitmap* bitmap_star;
Bitmap* bitmap_square;
Bitmap* bitmap_vertical_white_bar;
Bitmap* bitmap_horizontal_white_bar;
Bitmap* bitmap_last_input;
Bitmap* bitmap_round;
Bitmap* bitmap_number_0;
Bitmap* bitmap_number_1;
Bitmap* bitmap_number_2;
Bitmap* bitmap_number_3;
Bitmap* bitmap_number_4;
Bitmap* bitmap_number_5;
Bitmap* bitmap_number_6;
Bitmap* bitmap_number_7;
Bitmap* bitmap_number_8;
Bitmap* bitmap_number_9;
Bitmap* bitmap_black_square;
Bitmap* bitmap_a_key;
Bitmap* bitmap_w_key;
Bitmap* bitmap_s_key;
Bitmap* bitmap_d_key;
Bitmap* bitmap_black_bar;
Bitmap* bitmap_player1;
Bitmap* bitmap_player2;
Bitmap* bitmap_cpu;
Bitmap* bitmap_turn;
Bitmap* bitmap_cursor;
Bitmap* bitmap_congratulations;
Bitmap* bitmap_nice;
Bitmap* bitmap_plagiarism;
Bitmap* bitmap_happy_face;
Bitmap* bitmap_ups;
Bitmap* bitmap_you;
Bitmap* bitmap_failed;
Bitmap* bitmap_sad_face;
Bitmap* bitmap_thanks;
Bitmap* bitmap_for;
Bitmap* bitmap_playing;
Bitmap* bitmap_final_score;

/**
 * @brief Loads a bmp image
 *
 * @param filename Path of the image to load
 * @return Non NULL pointer to the image buffer
 */
Bitmap* loadBitmap(const char* filename);

/**
 * @brief Draws an unscaled, unrotated bitmap at the given position
 *
 * @param bitmap bitmap to be drawn
 * @param x destiny x coord
 * @param y destiny y coord
 * @param alignment image alignment
 */
void drawBitmap(Bitmap* bitmap, int x, int y, Alignment alignment);

/**
 * @brief Destroys the given bitmap, freeing all resources used by it.
 *
 * @param bitmap bitmap to be destroyed
 */
void deleteBitmap(Bitmap* bmp);

/**
 * @brief Loads all the bitmaps used by calling loadBitmap function.
 *
 */
void get_bitmaps();

/**
 * @brief Draws a black screen using the black bar image
 *
 */
void draw_black_screen();

/**
 * @brief Draws the main menu, wich gives you two options: START or EXIT.
 *
 */
void draw_main_menu();

/**
 * @brief Draws the game board, environment in wich the game will decurr (the gaming board shows the current round).
 *
 * @param round current game round
 */
void draw_board(int round);

/**
 * @brief A draw that announces player1's turn to play
 *
 */
void draw_player1_turn();

/**
 * @brief A draw that announces player2's turn to play
 *
 */
void draw_player2_turn();

/**
 * @brief A draw that announces a successful copy of player1's moves
 *
 */
void draw_success_copy();

/**
 * @brief A draw that announces a failed copy of player1's moves
 *
 */
void draw_failed_copy();

/**
 * @brief A draw that says "Thank you for playing"
 *
 */
void draw_thanks_for_playing();

/**
 * @brief A draws a given input given by a player.
 * an input is a play made by a player (it can be selecting a geometrical figure on the board, preesing a key, or draw a positive slope with the mouse)
 *
 * @param input play made by a player
 */
void draw_input(int input);

/**
 * @brief Draws the mouse cursor at a given location
 *
 * @param x x coordinate of the location where the mouse cursor will be drawn
 * @param y y coordinate of the location where the mouse cursor will be drawn
 */
void draw_cursor(int x, int y);

/**
 * @brief Draws a number(0-9) at a given location(x,y)
 *
 * @param number number to be drawn
 * @param x x coordinate where the number will be drawn
 * @param y y coordinate where the number will be drawn
 */
void draw_number(int number, int x, int y);

/**@}*/
