#include <LiquidCrystal.h>
#include "menu.h"
#include "my_time.h"
#include "my_timer.h"
#include <EEPROM.h>
#include "lines.h"

#define RS  2
#define EN  3
#define D4  4
#define D5  5
#define D6  6
#define D7  7
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

uint8_t btns_state = 0x00;
#define	BTN_MSK_LEFT 		0x04
#define	BTN_MSK_OK 			0x02
#define	BTN_MSK_RIGHT 	0x01

uint8_t get_check_btns_msk();

#define LINES_COUNT 5
static_assert(1 <= LINES_COUNT && LINES_COUNT <= 7, "Число линий может быть только от 1 до 7 включительно");
MenuState menu_state;
TimeData lines_datas[LINES_COUNT] = {
	{ .hh = 12, .mm = 34, .ss = 56 },
	{ .hh = 12, .mm = 34, .ss = 56 },
	{ .hh = 12, .mm = 34, .ss = 56 },
};

TimerInfo timer_1000;
TimeData countdown_time;

void redraw_display(uint8_t change_msk);
void do_init();
void do_process();
void save_time_data();

void timer_1000_cbk();
bool time_changed = false;

void setup() {  
	do_init();
}

void loop() {	
  do_process();
}

void do_init() {
  Serial.begin(9600);
  while (!Serial);
	
	// init_btns
  DDRC &= ~(1 << DDC3);
  DDRC &= ~(1 << DDC4);
  DDRC &= ~(1 << DDC5);
  
	set_initial_state(&menu_state, LINES_COUNT);
	
	load_time_data();
	
  lcd.begin(16, 2);
  lcd.leftToRight();
	lcd.cursor();
	lcd.blink();
	redraw_display(CM_Menu | CM_Value | CM_CursorPos);
	
	timer_1000 = ti_create(&timer_1000_cbk, 1000);
	lines_init();
}

void do_process() {
	uint8_t b_ev = get_check_btns_msk();
	
	BtnPress bp;
	switch (b_ev) {
		case BTN_MSK_LEFT: 	bp = BP_LEFT; 	break;
		case BTN_MSK_OK: 		bp = BP_OK; 		break;
		case BTN_MSK_RIGHT:	bp = BP_RIGHT; 	break;
		default: bp = 2;
	}
	
	uint8_t change_msk = 0;
	bool should_save = false;
	if (bp != 2) {
		process_btn(&menu_state, bp, lines_datas, &change_msk, &should_save);
	}
	
	if (should_save) {
		save_time_data();
		
		lcd.setCursor(0, 1);
		lcd.print("Saved!          ");
		
		delay(800);
	}
	
	if (change_msk & CM_IsRunning) {
		if (menu_state.is_running) {
			ti_reset(&timer_1000);
			for (uint8_t i = 0; i < LINES_COUNT; i++) {
				lines_set(i, true);
			}
			
			countdown_time = lines_datas[0];
			for (uint8_t i = 1; i < LINES_COUNT; i++) {
				if (mt_less(&countdown_time, &lines_datas[i])) {
					countdown_time = lines_datas[i];
				}
			}
		} else {
			for (uint8_t i = 0; i < LINES_COUNT; i++) {
				lines_set(i, false);
			}
		}
	}
	
	if (menu_state.is_running) {
		ti_process(&timer_1000);
	}
	
	if (time_changed) {
		change_msk |= CM_Value;
		time_changed = false;
	}
	
	redraw_display(change_msk);
}

uint8_t get_check_btns_msk() {
  uint8_t prev_btns_state = btns_state;

  btns_state = (PINC >> 3) & 0x07;
	
	uint8_t diff = prev_btns_state ^ btns_state;
	
	if (btns_state > prev_btns_state) {
		return diff;
	} else {
		return 0;
	}
}

void redraw_display(uint8_t change_msk) {
	if (menu_state.is_running) {
		lcd.setCursor(0, 0);
		lcd.print("Press OK to stop");
		
		lcd.setCursor(0, 1);
		lcd.print(countdown_time.hh / 10);
		lcd.print(countdown_time.hh % 10);
		lcd.print(":");
		lcd.print(countdown_time.mm / 10);
		lcd.print(countdown_time.mm % 10);
		lcd.print(":");
		lcd.print(countdown_time.ss / 10);
		lcd.print(countdown_time.ss % 10);
		lcd.print("        ");
		
		return;
	}
	
	// Menu title
	if (change_msk & CM_Menu) {
		switch (menu_state.tag) {
		
			case MST_CHOOSE_LINE:
				lcd.setCursor(0, 0);
				lcd.print("Line:           ");
				break;
				
			case MST_CHOOSE_DIGIT: 
				lcd.setCursor(0, 0);
				lcd.print("Choose: (line ");
				lcd.print(menu_state.line_num + 1);
				lcd.print(")");
				break;
				
			case MST_SET_VALUE:
				lcd.setCursor(0, 0);
				lcd.print("Set:    (line ");
				lcd.print(menu_state.line_num + 1);
				lcd.print(")");
				break;
			
		}
	}
	
	// Value
	if (change_msk & CM_Value) {
		switch (menu_state.tag) {
		
			case MST_CHOOSE_LINE:
				lcd.setCursor(0, 1);
				uint8_t line_i, col_i;
				for (line_i = 1, col_i = 0; line_i <= LINES_COUNT && col_i < 16; line_i++, col_i += 2) {
					lcd.print(line_i);
					lcd.print(" ");
				}
				
				lcd.print("Start");
				col_i += 5;
				
				for (; col_i < 16; col_i++) {
					lcd.print(" ");
				}
				break;
				
			case MST_CHOOSE_DIGIT: 
			case MST_SET_VALUE:
				lcd.setCursor(0, 1);
				lcd.print(lines_datas[menu_state.line_num].hh / 10);
				lcd.print(lines_datas[menu_state.line_num].hh % 10);
				lcd.print(":");
				lcd.print(lines_datas[menu_state.line_num].mm / 10);
				lcd.print(lines_datas[menu_state.line_num].mm % 10);
				lcd.print(":");
				lcd.print(lines_datas[menu_state.line_num].ss / 10);
				lcd.print(lines_datas[menu_state.line_num].ss % 10);
				lcd.print(" >");
				lcd.print("      ");
				break;
			
		}
	}
	
	// Cursor
	if (change_msk & CM_CursorPos) {
		switch (menu_state.tag) {
			
			case MST_CHOOSE_LINE:
				lcd.setCursor(menu_state.line_cursor_pos, 1);
				break;
				
			case MST_CHOOSE_DIGIT: 
			case MST_SET_VALUE:
				lcd.setCursor(menu_state.digit_cursor_pos, 1);
				break;
				
		}
	}
}

void save_time_data() {
	uint8_t total_size = sizeof(TimeData) * LINES_COUNT;
	
	uint8_t *ptr = (uint8_t*)lines_datas;
	for (uint8_t i = 0; i < total_size; i++) {
		EEPROM.update(i, ptr[i]);
	}
}

void load_time_data() {
	uint8_t total_size = sizeof(TimeData) * LINES_COUNT;
	
	uint8_t *ptr = (uint8_t*)lines_datas;
	for (uint8_t i = 0; i < total_size; i++) {
		ptr[i] = EEPROM.read(i);
	}
}

void timer_1000_cbk() {
	if (!menu_state.is_running) return;
	
	mt_add_seconds(&countdown_time, -1);
	
	for (uint8_t i = 0; i < LINES_COUNT; i++) {
		if (mt_eq(&timer_1000.time, &lines_datas[i])) {
			lines_set(i, false);
		}
	}
	
	bool has_unfinished_lines = false;
		
	for (uint8_t i = 0; i < LINES_COUNT; i++) {
		if (mt_less(&timer_1000.time, &lines_datas[i])) {
			has_unfinished_lines = true;
		}
	}
	
	if (!has_unfinished_lines) {
		menu_state.is_running = false;
		
		redraw_display(CM_Menu | CM_Menu | CM_CursorPos);
		for (uint8_t i = 0; i < LINES_COUNT; i++) {
			lines_set(i, false);
		}
	}
	
	time_changed = true;
}
