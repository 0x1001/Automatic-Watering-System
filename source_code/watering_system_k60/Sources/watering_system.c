/*
 * watering_system.c
 *
 *  Created on: Aug 13, 2014
 *      Author: Dev
 */
#include <mqx.h>
#include "watering_system.h"

static watering_system_params ws_params;

static void watering_system_init(uint32_t time_between_watering, uint32_t watering_cycles, uint32_t pumping_time, uint32_t dry_time);
static void watering_system_start(void);
static void watering_system_deinit(void);

void watering_system(uint32_t time_between_watering, uint32_t watering_cycles, uint32_t pumping_time, uint32_t dry_time){
	watering_system_init(time_between_watering, watering_cycles, pumping_time, dry_time);
	watering_system_start();
	watering_system_deinit();
}

void watering_system_init(uint32_t time_between_watering, uint32_t watering_cycles, uint32_t pumping_time, uint32_t dry_time){
	pump_ptr p;
	
	p = (pump_ptr)_mem_alloc_zero(sizeof(pump));
	pump_init(p);
	ws_params.pump = p;
	
	_mutex_init(&ws_params.pump_mutex,NULL);
	
	watering_system_update(time_between_watering, watering_cycles, pumping_time, dry_time);
}

void watering_system_deinit(void){
	_mem_free(ws_params.pump);
	ws_params.pump = NULL;
	_mutex_destroy(&ws_params.pump_mutex);
}

void watering_system_update(uint32_t time_between_watering, uint32_t watering_cycles, uint32_t pumping_time, uint32_t dry_time){
	ws_params.pumping_time = pumping_time;
	ws_params.dry_time = dry_time;
	ws_params.watering_cycles = watering_cycles;
	ws_params.time_between_watering = time_between_watering;
}

void watering_system_start(void){
	uint32_t cycle = 0;
	
	while (TRUE){
		for(cycle = 0; ws_params.watering_cycles > cycle; cycle++) {
			watering_system_pump_water(ws_params.pumping_time);
			_time_delay(ws_params.time_between_watering);
		}
		_time_delay(ws_params.dry_time);
	}
}

void watering_system_pump_water(uint32_t duration){
	if (MQX_EOK == _mutex_try_lock(&ws_params.pump_mutex)){
		pump_run(ws_params.pump, duration);
		_mutex_unlock(&ws_params.pump_mutex);
	}
}
