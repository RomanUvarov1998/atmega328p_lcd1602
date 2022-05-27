#include "menu.h"

static void move_digit(MenuState *st, BtnPress btn, TimeData *time_data);
static int8_t get_digit_cursor_pos(MenuState *st);
static bool has_unsaved_changes = false;

void set_initial_state(MenuState *st, int8_t lines_cnt) {
	st->tag = MST_CHOOSE_LINE;
	st->lines_cnt = lines_cnt;
	st->line_num = 0;
	st->line_cursor_pos = 0;
	st->digit_num = 0;
	st->digit_cursor_pos = 0;
	st->is_running = false;

}

void process_btn(MenuState *st, BtnPress btn, TimeData *time_datas, uint8_t *change_msk, bool *should_save)
{
	*should_save = false;
	*change_msk |= CM_CursorPos;
	*change_msk &= ~CM_IsRunning;
	
	if (st->is_running && btn == BP_OK) {
		*change_msk |= CM_IsRunning;
		*change_msk |= CM_Menu;
		*change_msk |= CM_Value;
		*change_msk |= CM_CursorPos;
		st->is_running = false;
		return;
	}
	
	switch (st->tag) {
		
		case MST_CHOOSE_LINE:			
			switch (btn) {
				
				case BP_LEFT:
				case BP_RIGHT:
					st->line_num += (int8_t)btn;
					if (st->line_num < 0) {
						st->line_num = st->lines_cnt;
					} else if (st->line_num > st->lines_cnt) {
						st->line_num = 0;
					}
					st->line_cursor_pos = st->line_num * 2;
					*change_msk &= ~CM_Menu;
					*change_msk &= ~CM_Value;
					break;

				case BP_OK:
					if (st->line_num == st->lines_cnt) {
						st->is_running = !st->is_running;
						*change_msk |= CM_IsRunning;
						*change_msk |= CM_Menu;
						*change_msk |= CM_Value;
					} else {
						st->tag = MST_CHOOSE_DIGIT;
						st->digit_num = 0;
						st->digit_cursor_pos = get_digit_cursor_pos(st);
						*change_msk |= CM_Menu;
						*change_msk |= CM_Value;
					}
					break;
				
			}
			break;

		case MST_CHOOSE_DIGIT:			
			switch (btn) {
				
				case BP_LEFT:
				case BP_RIGHT:
					st->digit_num += (int8_t)btn;
					if (st->digit_num < 0) {
						st->digit_num = DIGITS_COUNT;
					} else if (st->digit_num > DIGITS_COUNT) {
						st->digit_num = 0;
					}
					st->digit_cursor_pos = get_digit_cursor_pos(st);
					*change_msk &= ~CM_Menu;
					*change_msk &= ~CM_Value;
					break;

				case BP_OK:
					if (st->digit_num == DIGITS_COUNT) {
						st->tag = MST_CHOOSE_LINE;
						st->line_num = 0;
						*change_msk |= CM_Menu;
						*change_msk |= CM_Value;
						if (has_unsaved_changes) {
							*should_save = true;
							has_unsaved_changes = false;
						}
					} else {
						st->tag = MST_SET_VALUE;
						*change_msk |= CM_Menu;
						*change_msk &= ~CM_Value;
					}
					break;
				
			}
			break;

		case MST_SET_VALUE:
			switch (btn) {
				
				case BP_LEFT:
				case BP_RIGHT:
					move_digit(st, btn, &time_datas[st->line_num]);
					*change_msk &= ~CM_Menu;
					*change_msk |= CM_Value;
					has_unsaved_changes = true;
					break;

				case BP_OK:
					st->tag = MST_CHOOSE_DIGIT;
					*change_msk |= CM_Menu;
					*change_msk &= ~CM_Value;
					break;
				
			}
			break;
		
	}
	//dump(st, time_datas);
}

static void move_digit(MenuState *st, BtnPress btn, TimeData *time_data) {
	if (btn == BP_OK) return;
	
	int8_t delta;
	delta = (int8_t)btn;
	
	switch (st->digit_num) {
		case 0: mt_add_hours(time_data, delta * 10); break;
		case 1: mt_add_hours(time_data, delta); break;
		case 2: mt_add_minutes(time_data, delta * 10); break;
		case 3: mt_add_minutes(time_data, delta); break;
		case 4: mt_add_seconds(time_data, delta * 10); break;
		case 5: mt_add_seconds(time_data, delta); break;
	}
}

static int8_t get_digit_cursor_pos(MenuState *st) {
	switch (st->digit_num) {
		case 0: return 0;
		case 1: return 1;
		case 2: return 3;
		case 3: return 4;
		case 4: return 6;
		case 5: return 7;
		case 6: return 9;
	}
}

void dump(MenuState *st, TimeData *time_datas) {
	switch (st->tag) {
		
		case MST_CHOOSE_LINE:
			Serial.print("MST_CHOOSE_LINE, line: ");
			Serial.println(st->line_num);
			break;

		case MST_CHOOSE_DIGIT:
			Serial.print("MST_CHOOSE_DIGIT, digit: ");
			Serial.println(st->digit_num);
			break;

		case MST_SET_VALUE:
			Serial.print("MST_SET_VALUE, digit: ");
			Serial.println(st->digit_num);
			mt_print(&time_datas[st->line_num]);
			break;
			
	}
	Serial.println("---------------------");
}
