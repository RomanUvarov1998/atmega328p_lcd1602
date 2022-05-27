#ifndef MY_TIMER_H_
#define MY_TIMER_H_

#include <Arduino.h>
#include "my_time.h"

typedef void (*TimerCbk)();

typedef struct {
	TimerCbk cbk;
	uint16_t curr_millis;
	uint16_t passed_millis;
	uint16_t period_ms;
	TimeData time;
} TimerInfo;

TimerInfo ti_create(TimerCbk cbk, uint16_t period_ms);

void ti_reset(TimerInfo *ti);
void ti_process(TimerInfo *ti);

#endif // MY_TIMER_H_
