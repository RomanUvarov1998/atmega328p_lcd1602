#include <LiquidCrystal.h>
#include "menu.h"
#include "my_time.h"
#include "my_timer.h"
#include <EEPROM.h>

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

#define LINES_COUNT 3
static_assert(1 <= LINES_COUNT && LINES_COUNT <= 8, "Число линий может быть только от 1 до 8 включительно");
MenuState menu_state;
TimeData lines_datas[LINES_COUNT] = {
	{ .hh = 12, .mm = 34, .ss = 56 },
	{ .hh = 12, .mm = 34, .ss = 56 },
	{ .hh = 12, .mm = 34, .ss = 56 },
};

TimeData current_time;

TimerInfo timer_1000;
TimerInfo timer_500;

void redraw_display(uint8_t change_msk);
void do_init();
void do_process();
void save_time_data();

void timer_1000_cbk(uint16_t seconds);
void timer_500_cbk(uint16_t seconds);

void setup() {  
	do_init();
}

void loop() {	
	ti_process(&timer_500);
	ti_process(&timer_1000);

  do_process();
}

void do_init() {
  Serial.begin(9600);
  while (!Serial);
	
	// init_btns
  DDRC &= ~(1 << DDC3);
  DDRC &= ~(1 << DDC4);
  DDRC &= ~(1 << DDC5);
  
	set_initial_state(&menu_state, 3);
	
	load_time_data();
	
  lcd.begin(16, 2);
  lcd.leftToRight();
	lcd.cursor();
	lcd.blink();
	redraw_display(CM_Menu | CM_Value | CM_CursorPos);
	
	current_time = mt_create_zero();
	
	timer_1000 = ti_create(&timer_1000_cbk, 1000);
	timer_500 = ti_create(&timer_500_cbk, 500);
}

void do_process() {
	uint8_t b_ev = get_check_btns_msk();
	
	BtnPress b_ev_value;
	switch (b_ev) {
		case BTN_MSK_LEFT: 	b_ev_value = BP_LEFT; 	break;
		case BTN_MSK_OK: 		b_ev_value = BP_OK; 		break;
		case BTN_MSK_RIGHT:	b_ev_value = BP_RIGHT; 	break;
		default: return;
	}
	
	uint8_t change_msk = 0;
	bool should_save = false;
	process_btn(&menu_state, b_ev_value, lines_datas, &change_msk, &should_save);
	
	if (should_save) {
		save_time_data();
		
		lcd.setCursor(0, 1);
		lcd.print("Saved!          ");
		
		delay(800);
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

void timer_1000_cbk(uint16_t seconds) {
	// mt_add_seconds(&current_time, 1);
	// mt_print(&current_time);
	
	// for (uint8_t i = 0; i < LINES_COUNT; i++) {
		// if (mt_eq(&current_time, &lines_datas[i])) {
			// Serial.print(i);
			// Serial.println(" off");
		// }
	// }
	
	Serial.println(seconds);
}

void timer_500_cbk(uint16_t seconds) {
	Serial.println("0.5");
}
