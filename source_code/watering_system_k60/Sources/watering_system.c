/*
 * watering_system.c
 *
 *  Created on: Aug 13, 2014
 *      Author: Dev
 */
#include <mqx.h>
#include "watering_system.h"

static watering_system_params ws_params;

static void watering_system_init(uint64_t start_delay,uint64_t time_between_watering, uint32_t watering_cycles, uint32_t pumping_time, uint64_t dry_time);
static void watering_system_start(void);
static void watering_system_deinit(void);
static void watering_system_delay(uint64_t delay);

void watering_system(uint64_t start_delay, uint64_t time_between_watering, uint32_t watering_cycles, uint32_t pumping_time, uint64_t dry_time){
	watering_system_init(start_delay, time_between_watering, watering_cycles, pumping_time, dry_time);
	watering_system_start();
	watering_system_deinit();
}

watering_system_progress_ptr watering_system_get_progress(void){
	return &ws_params.progress;
}

watering_system_params_ptr watering_system_get_params(void){
	return &ws_params;
}

void watering_pump_task(uint32_t initial_data){
	while (TRUE){
		_lwevent_wait_for(&ws_params.pump_event,EVENT_MASK,TRUE,NULL);
		pump_run(ws_params.pump, ws_params.current_pump_time);
	}
}

void watering_system_pump_water(uint32_t duration){
	ws_params.current_pump_time = duration;
	_lwevent_set(&ws_params.pump_event,EVENT_MASK);
}

void watering_system_init(uint64_t start_delay, uint64_t time_between_watering, uint32_t watering_cycles, uint32_t pumping_time, uint64_t dry_time){
	pump_ptr p;
	
	p = (pump_ptr)_mem_alloc_zero(sizeof(pump));
	pump_init(p);
	ws_params.pump = p;
	
	_lwevent_create(&ws_params.pump_event,0);
	_lwevent_set_auto_clear(&ws_params.pump_event,EVENT_MASK);
	
	_task_create(0,WATERING_PUMP,0);
	
	watering_system_update(start_delay, time_between_watering, watering_cycles, pumping_time, dry_time);
}

void watering_system_deinit(void){
	_mem_free(ws_params.pump);
	ws_params.pump = NULL;
	_lwevent_destroy(&ws_params.pump_event);
}

void watering_system_update(uint64_t start_delay, uint64_t time_between_watering, uint32_t watering_cycles, uint32_t pumping_time, uint64_t dry_time){
	ws_params.pumping_time = pumping_time;
	ws_params.dry_time = dry_time;
	ws_params.watering_cycles = watering_cycles;
	ws_params.time_between_watering = time_between_watering;
	ws_params.start_delay = start_delay;
}

void watering_system_start(void){
	uint32_t cycle = 0;
	
	watering_system_delay(ws_params.start_delay);
	while (TRUE){
		for(cycle = 0; ws_params.watering_cycles > cycle; cycle++) {
			watering_system_pump_water(ws_params.pumping_time);
			watering_system_delay(ws_params.time_between_watering);
		}
		watering_system_delay(ws_params.dry_time);
	}
}

void watering_system_delay(uint64_t delay){
	uint64_t refresh_time = 60*SECOND;
	
	if (delay == 0)
		return;
	else if (delay < refresh_time)
		refresh_time = delay;
	
	ws_params.progress.total_time = delay;
	ws_params.progress.passed_time = 0;
	
	for (ws_params.progress.passed_time = 0; ws_params.progress.passed_time <= ws_params.progress.total_time; ws_params.progress.passed_time += refresh_time)
		_time_delay(refresh_time);
	
	ws_params.progress.passed_time = delay;
}
