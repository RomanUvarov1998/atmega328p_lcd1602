#include "my_time.h"

void add_hours(TimeData *td, int8_t dh) {
	td->hh += dh;
	if (td->hh > 23) { td->hh = 0; }
	if (td->hh < 0) { td->hh = 23; }
}

void add_minutes(TimeData *td, int8_t dm) {
	td->mm += dm;
	if (td->mm > 59) {
		td->mm = 0;
		add_hours(td, 1);
	}
	if (td->mm < 0) {
		td->mm = 59;
		add_hours(td, -1);
	}
}

void add_seconds(TimeData *td, int8_t ds) {
	td->ss += ds;
	if (td->ss > 59) {
		td->ss = 0;
		add_minutes(td, 1);
	}
	if (td->ss < 0) {
		td->ss = 59;
		add_minutes(td, -1);
	}
}
