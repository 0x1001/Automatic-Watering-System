/*
 * watering_system.h
 *
 *  Created on: Aug 13, 2014
 *      Author: Dev
 */

#ifndef WATERING_SYSTEM_H_
#define WATERING_SYSTEM_H_

#include <mutex.h>
#include "pump.h"

#define SECOND 1000
#define MINUTE (60*SECOND)
#define HOUR (60*MINUTE)
#define DAY (24*HOUR)

#define WATERING_PUMP 6
#define EVENT_MASK 1

typedef struct {
	uint64_t total_time;
	uint64_t passed_time;
}watering_system_progress, *watering_system_progress_ptr;

typedef struct {
	uint64_t start_delay;
	uint32_t pumping_time;
	uint32_t current_pump_time;
	uint64_t time_between_watering;
	uint32_t watering_cycles;
	uint64_t dry_time;
	pump_ptr pump;
	LWEVENT_STRUCT pump_event;
	watering_system_progress progress;
} watering_system_params, *watering_system_params_ptr;

void watering_system(uint64_t start_delay, uint64_t time_between_watering, uint32_t watering_cycles, uint32_t pumping_time, uint64_t dry_time);
void watering_pump_task(uint32_t);
void watering_system_update(uint64_t start_delay, uint64_t time_between_watering, uint32_t watering_cycles, uint32_t pumping_time, uint64_t dry_time);
void watering_system_pump_water(uint32_t duration);
watering_system_progress_ptr watering_system_get_progress(void);
watering_system_params_ptr watering_system_get_params(void);

#endif /* WATERING_SYSTEM_H_ */
