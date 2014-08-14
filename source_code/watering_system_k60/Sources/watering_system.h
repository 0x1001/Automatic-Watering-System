/*
 * watering_system.h
 *
 *  Created on: Aug 13, 2014
 *      Author: Dev
 */

#ifndef WATERING_SYSTEM_H_
#define WATERING_SYSTEM_H_

#include "pump.h"

#define SECOND 1000
#define MINUTE 60*SECOND
#define HOUR 60*MINUTE
#define DAY 24*HOUR

typedef struct {
	uint32_t pumping_time;
	uint32_t time_between_watering;
	uint32_t watering_cycles;
	uint32_t dry_time;
	pump pump;
} watering_system, *watering_system_ptr;

void watering_system_init(watering_system_ptr w, uint32_t time_between_watering, uint32_t watering_cycles, uint32_t pumping_time, uint32_t dry_time);
void watering_system_update(watering_system_ptr w, uint32_t time_between_watering, uint32_t watering_cycles, uint32_t pumping_time, uint32_t dry_time);
void watering_system_start(watering_system_ptr w);
void watering_system_pump_water(watering_system_ptr w, uint32_t duration);

#endif /* WATERING_SYSTEM_H_ */
