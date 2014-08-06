/*
 * pump.h
 *
 *  Created on: Aug 6, 2014
 *      Author: Dev
 */

#ifndef PUMP_H_
#define PUMP_H_

#include <bsp.h>

typedef struct {
    LWGPIO_STRUCT led_indicator;
    LWGPIO_STRUCT pump_pwr;
} pump, *pump_ptr;

void pump_init(pump_ptr p);
void pump_start(pump_ptr p);
void pump_stop(pump_ptr p);
void pump_run(pump_ptr p, uint32_t duration);

#endif /* PUMP_H_ */
