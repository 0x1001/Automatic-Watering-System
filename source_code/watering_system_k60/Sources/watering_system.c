/*
 * watering_system.c
 *
 *  Created on: Aug 13, 2014
 *      Author: Dev
 */
#include <mqx.h>
#include "watering_system.h"

void watering_system_init(watering_system_ptr w, uint32_t time_between_watering, uint32_t watering_cycles, uint32_t pumping_time, uint32_t dry_time){
	pump p;
	pump_init(&p);
	
	w->pump = p;
	watering_system_update(w, time_between_watering, watering_cycles, pumping_time, dry_time);
}

void watering_system_update(watering_system_ptr w, uint32_t time_between_watering, uint32_t watering_cycles, uint32_t pumping_time, uint32_t dry_time){
	w->pumping_time = pumping_time;
	w->dry_time = dry_time;
	w->watering_cycles = watering_cycles;
	w->time_between_watering = time_between_watering;
}

void watering_system_start(watering_system_ptr w){
	uint32_t cycle = 0;
	
	while (TRUE){
		for(cycle = 0; w->watering_cycles > cycle; cycle++) {
			pump_run(&w->pump, w->pumping_time);
			_time_delay(w->time_between_watering);
		}
		_time_delay(w->dry_time);
	}
}
