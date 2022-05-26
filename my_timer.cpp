#include "my_timer.h"

static TimerCbk timer_callback;

static uint16_t timer_cnt = 0;
static uint16_t curr_millis = 0;
static uint16_t passed_millis = 0;
static uint16_t passed_seconds = 0;
#define SECOND_MS 1000

void timer_reset(TimerCbk cbk) {
	timer_cnt = 0;
	curr_millis = 0;
	passed_millis = 0;
	passed_seconds = 0;
	timer_callback = cbk;
}

void timer_process() {
	uint16_t prev_millis = curr_millis;
	curr_millis = millis();
	uint16_t passed;

	if (curr_millis > prev_millis) {
		passed = curr_millis - prev_millis;
	} else {
		passed = prev_millis - curr_millis;
	}
	
	passed_millis += passed;
	
	
	if (passed_millis > SECOND_MS) {
		passed_millis -= SECOND_MS;
		passed_seconds++;
		
		timer_callback(passed_seconds);
	}
}
