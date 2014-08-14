/*
 * button.c
 *
 *  Created on: Aug 14, 2014
 *      Author: Dev
 */

#include "button.h"

void button(void){
    uint32_t delay = 10;
    LWGPIO_STRUCT btn1;
    
    if (!lwgpio_init(&btn1, BSP_BUTTON1, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
        return;

    while (TRUE){
        if (LWGPIO_VALUE_LOW == lwgpio_get_value(&btn1))
            ;//watering_system_pump_water(&w, delay);
        else
            _time_delay(delay);
    }
}
