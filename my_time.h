#ifndef MY_TIME_H_
#define MY_TIME_H_

#include <stdint.h>

typedef struct {
	int8_t hh;
	int8_t mm;
	int8_t ss;
} TimeData;

void add_hours(TimeData *td, int8_t dh);
void add_minutes(TimeData *td, int8_t dm);
void add_seconds(TimeData *td, int8_t ds);

#endif // MY_TIME_H_
