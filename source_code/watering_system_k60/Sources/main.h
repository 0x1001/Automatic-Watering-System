#ifndef __main_h_
#define __main_h_
#include <mqx.h>
#include "watering_system.h"
#include "network.h"
#include "button.h"

#define MAIN_TASK 1
#define HTTP_SERVER 2
#define RTCS_INIT 3
#define WATERING 4
#define BUTTON_CTRL 5

void Main_task (uint32_t);
void watering_task(uint32_t);
void http_server_task(uint32_t initial_data);
void rtcs_init_task(uint32_t initial_data);
void button_task(uint32_t initial_data);

#endif /* __main_h_ */

