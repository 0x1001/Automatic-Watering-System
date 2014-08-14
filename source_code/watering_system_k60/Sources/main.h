#ifndef __main_h_
#define __main_h_
#include <mqx.h>
#include <bsp.h>
#include <mfs.h>

#define MAIN_TASK   1
#define WATERING 2

extern void Main_task (uint32_t);
extern void watering_task(uint32_t);

#endif /* __main_h_ */

