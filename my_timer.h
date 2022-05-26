#ifndef MY_TIMER_H_
#define MY_TIMER_H_

#include <Arduino.h>

typedef void (*TimerCbk)(uint16_t);

void timer_reset(TimerCbk cbk);
void timer_process();

#endif // MY_TIMER_H_
