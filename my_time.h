#ifndef MY_TIME_H_
#define MY_TIME_H_

#include <stdint.h>
#include <Arduino.h>

typedef struct {
	int8_t hh;
	int8_t mm;
	int8_t ss;
} TimeData;

TimeData mt_create_zero();
void mt_add_hours(TimeData *td, int8_t dh);
void mt_add_minutes(TimeData *td, int8_t dm);
void mt_add_seconds(TimeData *td, int8_t ds);
void mt_print(TimeData *td);
bool mt_eq(TimeData *td1, TimeData *td2);

#endif // MY_TIME_H_
