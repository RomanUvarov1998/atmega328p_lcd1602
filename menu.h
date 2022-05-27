#ifndef MENU_H_
#define MENU_H_

#include <stdint.h>
#include "my_time.h"
#include <Arduino.h>

//													hh 	:		mm	:		ss
//													v		v		v		v		v
#define DIGITS_COUNT 			(	2 + 		2 + 		2)
// indexes:									01	2		34	5		67

typedef enum {
	MST_CHOOSE_LINE,
	MST_CHOOSE_DIGIT,
	MST_SET_VALUE,
} MenuStateTag;

typedef struct {
	MenuStateTag tag;
	int8_t lines_cnt;
	int8_t line_num;
	int8_t line_cursor_pos;
	int8_t digit_num;
	int8_t digit_cursor_pos;
	bool is_running;
} MenuState;

typedef enum {
	BP_LEFT = -1,
	BP_RIGHT = 1,
	BP_OK = 0,
} BtnPress;

void set_initial_state(MenuState *st, int8_t lines_cnt);
void process_btn(MenuState *st, BtnPress btn, TimeData *lines_datas, uint8_t *change_msk, bool *should_save);
void dump(MenuState *st, TimeData *lines_datas);

typedef enum {
	CM_Menu = 0x01,
	CM_Value = 0x02,
	CM_CursorPos = 0x04,
	CM_IsRunning = 0x08,
} ChangeMsk;

#endif // MENU_H_
