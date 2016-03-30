#ifndef KEY_H
#define KEY_H

#include <stdint.h>
#include <stdbool.h>

typedef uint16_t key_t;

/**
* Keyboard group 1
*/
#define Key_Graph			(key_t)(1 << 8 | 1<<0)
#define Key_Trace			(key_t)(1 << 8 | 1<<1)
#define Key_Zoom			(key_t)(1 << 8 | 1<<2)
#define Key_Window			(key_t)(1 << 8 | 1<<3)
#define Key_Yequ 			(key_t)(1 << 8 | 1<<4)
#define Key_2nd				(key_t)(1 << 8 | 1<<5)
#define Key_Mode			(key_t)(1 << 8 | 1<<6)
#define Key_Del				(key_t)(1 << 8 | 1<<7)

/**
* Keyboard group 2
*/
#define Key_Store	(key_t)(2 << 8 | 1<<1)
#define Key_Ln		(key_t)(2 << 8 | 1<<2)
#define Key_Log		(key_t)(2 << 8 | 1<<3)
#define Key_Square	(key_t)(2 << 8 | 1<<4)
#define Key_Recip	(key_t)(2 << 8 | 1<<5)
#define Key_Math	(key_t)(2 << 8 | 1<<6)
#define Key_Alpha	(key_t)(2 << 8 | 1<<7)

/**
* Keyboard group 3
*/
#define Key_0			(key_t)(3 << 8 | 1<<0)
#define Key_1			(key_t)(3 << 8 | 1<<1)
#define Key_4			(key_t)(3 << 8 | 1<<2)
#define Key_7			(key_t)(3 << 8 | 1<<3)
#define Key_Comma		(key_t)(3 << 8 | 1<<4)
#define Key_Sin			(key_t)(3 << 8 | 1<<5)
#define Key_Apps		(key_t)(3 << 8 | 1<<6)
#define Key_GraphVar	(key_t)(3 << 8 | 1<<7)

/**
* Keyboard group 4
*/
#define Key_DecPnt		(key_t)(4 << 8 | 1<<0)
#define Key_2			(key_t)(4 << 8 | 1<<1)
#define Key_5			(key_t)(4 << 8 | 1<<2)
#define Key_8			(key_t)(4 << 8 | 1<<3)
#define Key_LParen		(key_t)(4 << 8 | 1<<4)
#define Key_Cos			(key_t)(4 << 8 | 1<<5)
#define Key_Pgrm		(key_t)(4 << 8 | 1<<6)
#define Key_Stat		(key_t)(4 << 8 | 1<<7)

/**
* Keyboard group 5
*/
#define Key_Chs			(key_t)(5 << 8 | 1<<0)
#define Key_3			(key_t)(5 << 8 | 1<<1)
#define Key_6			(key_t)(5 << 8 | 1<<2)
#define Key_9			(key_t)(5 << 8 | 1<<3)
#define Key_RParen		(key_t)(5 << 8 | 1<<4)
#define Key_Tan			(key_t)(5 << 8 | 1<<5)
#define Key_Vars		(key_t)(5 << 8 | 1<<6)

/**
* Keyboard group 6
*/
#define Key_Enter	(key_t)(6 << 8 | 1<<0)
#define Key_Add		(key_t)(6 << 8 | 1<<1)
#define Key_Sub		(key_t)(6 << 8 | 1<<2)
#define Key_Mul		(key_t)(6 << 8 | 1<<3)
#define Key_Div		(key_t)(6 << 8 | 1<<4)
#define Key_Power	(key_t)(6 << 8 | 1<<5)
#define Key_Clear	(key_t)(6 << 8 | 1<<6)

/**
* Keyboard group 7
*/
#define Key_Down	(key_t)(7 << 8 | 1<<0)
#define Key_Left	(key_t)(7 << 8 | 1<<1)
#define Key_Right	(key_t)(7 << 8 | 1<<2)
#define Key_Up		(key_t)(7 << 8 | 1<<3)

void Key_init();
bool Key_isDown(key_t key);
bool Key_wasDown(key_t key);
bool Key_justPressed(key_t key);
void Key_scanKeys(uint32_t keyDelay);
void Key_reset();

#endif // !KEY_H