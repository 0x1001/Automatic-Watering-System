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


#if !defined(RAM_DISK_SIZE)
#error Please specify RAM_DISK_SIZE
#endif

#if defined(APPLICATION_HAS_SHELL) && (!SHELLCFG_USES_RTCS)
#error This application requires SHELLCFG_USES_RTCS defined non-zero in user_config.h. Please recompile libraries with this option if any Ethernet interface is available.
#endif


TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   Main_task,   2000,  9,   "main", MQX_AUTO_START_TASK},
   {0,           0,           0,     0,   0,      0,                 }
};

typedef enum {
    BUTTON_NOT_INITIALIZED = -1,
    BUTTON_RELEASED,
    BUTTON_PRESSED
} button_state_t;

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Main_task
* Comments       :
*    This task initializes MFS and starts SHELL.
*
*END------------------------------------------------------------------*/

void Main_task(uint32_t initial_data)
{
   /* Install ramdisk - MFS init */
    //Ram_disk_start();
  
   /* RTCS init */
    //rtcs_init();
   
    /* Structures holding information about specific pin */
    LWGPIO_STRUCT led1, motor_pwr, btn1;
    button_state_t  button_state, button_last_state;
    
/******************************************************************************
	Open GPIO pin as output and drive the output level low.
******************************************************************************/
	if (!lwgpio_init(&motor_pwr, GPIO_PORT_D | GPIO_PIN6, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
	{
		_task_block();
	}
	/* swich pin functionality (MUX) to GPIO mode */
	lwgpio_set_functionality(&motor_pwr, LWGPIO_MUX_D6_GPIO);

	/* write logical 1 to the pin */
	lwgpio_set_value(&motor_pwr, LWGPIO_VALUE_LOW); /* set pin to 1 */
    
/******************************************************************************
    Open the BSP_LED1) pin as output and drive the output level high.
******************************************************************************/
    /* initialize lwgpio handle (led1) for BSP_LED1 pin
     * (defined in mqx/source/bsp/<bsp_name>/<bsp_name>.h file)
     */
    if (!lwgpio_init(&led1, BSP_LED1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        _task_block();
    }
    /* swich pin functionality (MUX) to GPIO mode */
    lwgpio_set_functionality(&led1, BSP_LED1_MUX_GPIO);

    /* write logical 1 to the pin */
    lwgpio_set_value(&led1, LWGPIO_VALUE_HIGH); /* set pin to 1 */


/******************************************************************************
    Open the pin (BSP_BTN1) for input, initialize interrupt
    and set interrupt handler.
******************************************************************************/
    /* opening pins for input */
    if (!lwgpio_init(&btn1, BSP_BUTTON1, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
    {
        _task_block();
    }

    /* set pin functionality (MUX) to GPIO*/
    lwgpio_set_functionality(&btn1, BSP_BUTTON1_MUX_GPIO);
    lwgpio_set_attribute(&btn1, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);

    button_last_state = BUTTON_NOT_INITIALIZED;

    while (TRUE)
    {
         /* read pin/signal status */
        if (LWGPIO_VALUE_LOW == lwgpio_get_value(&btn1))
        {
            button_state = BUTTON_PRESSED;
        }
        else
        {
            button_state = BUTTON_RELEASED;
        }

        if (button_state != button_last_state)  {
            button_last_state = button_state;
            lwgpio_set_value(&led1, button_state == BUTTON_PRESSED ? LWGPIO_VALUE_LOW : LWGPIO_VALUE_HIGH);
            lwgpio_set_value(&motor_pwr, button_state == BUTTON_PRESSED ? LWGPIO_VALUE_HIGH : LWGPIO_VALUE_LOW);
        }
        /* Check button state every 100 ms */
        _time_delay(100);
   }
}
/* EOF */
