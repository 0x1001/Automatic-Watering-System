/**HEADER*******************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
**************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************
*
* Comments:
*
*   This file contains main initialization for your application
*   and infinite loop
*
*END************************************************************************/
#include "main.h"
#include "watering_system.h"

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   Main_task,   2000,  9,   "main", MQX_AUTO_START_TASK},
   {WATERING,   watering_task,   2000,  9,   "watering_task", 0},
   {0,           0,           0,     0,   0,      0,                 }
};

watering_system w;

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Main_task
* Comments       :
*    Pump control
*
*END------------------------------------------------------------------*/

void Main_task(uint32_t initial_data)
{
	uint32_t delay = 10;
    LWGPIO_STRUCT btn1;
	
	watering_system_init(&w, DAY, 2, 2*SECOND, 2*DAY);
	
	_task_create(0, WATERING, 0);
	
    if (!lwgpio_init(&btn1, BSP_BUTTON1, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
        _task_block();

    lwgpio_set_functionality(&btn1, BSP_BUTTON1_MUX_GPIO);
    lwgpio_set_attribute(&btn1, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);

    while (TRUE){
        if (LWGPIO_VALUE_LOW == lwgpio_get_value(&btn1))
        	watering_system_pump_water(&w, delay);
        else
        	_time_delay(delay);
    }

}

void watering_task(uint32_t initial_data){
	watering_system_start(&w);
}
