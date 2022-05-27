#include "my_timer.h"

TimerInfo ti_create(TimerCbk cbk, uint16_t period_ms) {
	TimerInfo ti = {
		.cbk = cbk,
		.curr_millis = 0,
		.passed_millis = 0,
		.period_ms = period_ms,
	};
	ti.time = mt_create_zero();
	return ti;
}

void ti_reset(TimerInfo *ti) {
	ti->curr_millis = millis();
	ti->passed_millis = 0;
	ti->time = mt_create_zero();
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
		mt_add_seconds(&ti->time, 1);
		
		ti->cbk();
	}
}
