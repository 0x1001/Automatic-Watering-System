/*
 * pump.c
 *
 *  Created on: Aug 6, 2014
 *      Author: Dev
 */
#include <mqx.h>
#include "pump.h"

static void _led_indicator_init(pump_ptr p);
static void _pump_pwr_init(pump_ptr p);
static void _pump_pwr_off(pump_ptr p);
static void _pump_pwr_on(pump_ptr p);

void pump_init(pump_ptr p){
	_led_indicator_init(p);
	_pump_pwr_init(p);
}

void pump_start(pump_ptr p) {
	_pump_pwr_on(p);
}

void pump_stop(pump_ptr p) {
	_pump_pwr_off(p);
}

void pump_run(pump_ptr p,uint32_t duration){
	uint32_t counter;
	
	for(counter = 0; counter <= duration/2; counter++){
		pump_start(p);
		_time_delay(1);
		pump_stop(p);
		_time_delay(1);
	}
}

static void _led_indicator_init(pump_ptr p) {
    if (!lwgpio_init(&(p->led_indicator), BSP_LED1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
        _task_block();

    lwgpio_set_functionality(&(p->led_indicator), BSP_LED1_MUX_GPIO);
    lwgpio_set_value(&(p->led_indicator), LWGPIO_VALUE_HIGH);
}

static void _pump_pwr_init(pump_ptr p){
    if (!lwgpio_init(&(p->pump_pwr), GPIO_PORT_D | GPIO_PIN6, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE)) 
        _task_block();

	lwgpio_set_functionality(&(p->pump_pwr), LWGPIO_MUX_D6_GPIO);
	lwgpio_set_value(&(p->pump_pwr), LWGPIO_VALUE_LOW);
}

static void _pump_pwr_on(pump_ptr p){
    lwgpio_set_value(&(p->led_indicator), LWGPIO_VALUE_LOW);
    lwgpio_set_value(&(p->pump_pwr), LWGPIO_VALUE_HIGH);
}

static void _pump_pwr_off(pump_ptr p){
    lwgpio_set_value(&(p->led_indicator), LWGPIO_VALUE_HIGH);
    lwgpio_set_value(&(p->pump_pwr), LWGPIO_VALUE_LOW);
}
