#include <LiquidCrystal.h>
#include "menu.h"
#include "my_time.h"

#define RS  2
#define EN  3
#define D4  4
#define D5  5
#define D6  6
#define D7  7
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

uint8_t btns_state = 0x00;
typedef enum {
	BE_LEFT = 0x04,
	BE_OK = 0x02,
	BE_RIGHT = 0x01,
} BtnEvent;

uint8_t check_btns();

MenuState menu_state;
TimeData lines_datas[3] = {
	{ .hh = 12, .mm = 34, .ss = 56 },
	{ .hh = 12, .mm = 34, .ss = 56 },
	{ .hh = 12, .mm = 34, .ss = 56 },
};

void redraw_display(bool menu_changed, bool value_changed);
void do_init();
void do_process();

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
  DDRC &= ~(1 << DDC2);
  DDRC &= ~(1 << DDC3);
  DDRC &= ~(1 << DDC4);
  
	set_initial_state(&menu_state, 3);
	
  lcd.begin(16, 2);
  lcd.leftToRight();
	lcd.cursor();
	lcd.blink();
	redraw_display(true, true);
}

void do_process() {
	uint8_t b_ev = check_btns();
	
	bool menu_changed = false, value_changed = false;
	
  BtnPress p;
	switch (b_ev) {
		case BE_LEFT: 	
			Serial.println("BE_LEFT");
			process_btn(&menu_state, BP_LEFT, lines_datas, &menu_changed, &value_changed);
			break;

		case BE_OK: 		
			Serial.println("BE_OK");	
			process_btn(&menu_state, BP_OK, lines_datas, &menu_changed, &value_changed);
			break;

		case BE_RIGHT: 	
			Serial.println("BE_RIGHT");	
			process_btn(&menu_state, BP_RIGHT, lines_datas, &menu_changed, &value_changed);
			break;
			
		default: return;
	}
	
	redraw_display(menu_changed, value_changed);
}

uint8_t check_btns() {
  uint8_t prev_btns_state = btns_state;

  btns_state = (PINC >> 2) & 0x07;
	
	uint8_t diff = prev_btns_state ^ btns_state;
	
	if (btns_state > prev_btns_state) {
		return diff;
	} else {
		return 0;
	}
}

void redraw_display(bool menu_changed, bool value_changed) {
	// Menu title
	if (menu_changed) {
		switch (menu_state.tag) {
		
			case MST_CHOOSE_LINE:
				lcd.setCursor(0, 0);
				lcd.print("Line:           ");
				break;
				
			case MST_CHOOSE_DIGIT: 
				lcd.setCursor(0, 0);
				lcd.print("Choose digit:   ");
				break;
				
			case MST_SET_VALUE:
				lcd.setCursor(0, 0);
				lcd.print("Set digit:      ");
				break;
			
		}
	}
	
	// Value
	if (value_changed) {
		switch (menu_state.tag) {
		
		case MST_CHOOSE_LINE:			
			lcd.setCursor(0, 1);
			lcd.print("1 2 3           ");
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
