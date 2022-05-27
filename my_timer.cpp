#include "my_timer.h"

TimerInfo ti_create(TimerCbk cbk, uint16_t period_ms) {
	return (TimerInfo) {
		.cbk = cbk,
		.curr_millis = 0,
		.passed_millis = 0,
		.passed_seconds = 0,
		.period_ms = period_ms,
	};
}

void ti_reset(TimerInfo *ti) {
	ti->curr_millis = 0;
	ti->passed_millis = 0;
	ti->passed_seconds = 0;
}

void ti_process(TimerInfo *ti) {
	uint16_t prev_millis = ti->curr_millis;
	ti->curr_millis = millis();
	uint16_t passed;

	if (ti->curr_millis > prev_millis) {
		passed = ti->curr_millis - prev_millis;
	} else {
		passed = prev_millis - ti->curr_millis;
	}
	
	ti->passed_millis += passed;
	
	if (ti->passed_millis > ti->period_ms) {
		ti->passed_millis -= ti->period_ms;
		ti->passed_seconds++;
		
		ti->cbk(ti->passed_seconds);
	}
}
