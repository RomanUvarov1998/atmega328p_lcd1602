#include "my_time.h"

TimeData mt_create_zero() {
	return (TimeData) {
		.hh = 0,
		.mm = 0,
		.ss = 0,
	};
}

void mt_add_hours(TimeData *td, int8_t dh) {
	td->hh += dh;
	if (td->hh > 23) { td->hh = 0; }
	if (td->hh < 0) { td->hh = 23; }
}

void mt_add_minutes(TimeData *td, int8_t dm) {
	td->mm += dm;
	if (td->mm > 59) {
		td->mm = 0;
		mt_add_hours(td, 1);
	}
	if (td->mm < 0) {
		td->mm = 59;
		mt_add_hours(td, -1);
	}
}

void mt_add_seconds(TimeData *td, int8_t ds) {
	td->ss += ds;
	if (td->ss > 59) {
		td->ss = 0;
		mt_add_minutes(td, 1);
	}
	if (td->ss < 0) {
		td->ss = 59;
		mt_add_minutes(td, -1);
	}
}

void mt_print(TimeData *td) {
	Serial.print(td->hh / 10);
	Serial.print(td->hh % 10);
	Serial.print(":");
	Serial.print(td->mm / 10);
	Serial.print(td->mm % 10);
	Serial.print(":");
	Serial.print(td->ss / 10);
	Serial.println(td->ss % 10);
}

bool mt_eq(TimeData *td1, TimeData *td2) {
	return 	td1->hh == td2->hh &&
					td1->mm == td2->mm &&
					td1->ss == td2->ss;
}
