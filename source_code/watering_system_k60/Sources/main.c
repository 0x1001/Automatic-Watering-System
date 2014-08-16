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

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   Main_task,   2000,  10,   "main", MQX_AUTO_START_TASK},
   {WATERING,   watering_task,   2000,  10,   "watering_task", 0},
   {WATERING_PUMP,   watering_pump_task,   2000,  9,   "watering_pump_task", 0},
   {HTTP_SERVER,   http_server_task,   2000,  10,   "HTTP_SERVER", 0,   0,  0},
   {RTCS_INIT,   rtcs_init_task,   2000,  10,   "RTCS_INIT", 0,   0,  0},
   {BUTTON_CTRL,   button_task,   2000,  11,   "BUTTON_CTRL", 0,   0,  0},
   {0,           0,           0,     0,   0,      0,                 }
};

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Main_task
* Comments       :
*    Main task
*
*END------------------------------------------------------------------*/

void Main_task(uint32_t initial_data){
    _task_create(0,RTCS_INIT,0);
    _task_create(0,HTTP_SERVER,0);
    _task_create(0, WATERING, 0);
    //_task_create(0,BUTTON_CTRL,0);
   _task_block();
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : watering_task
* Comments       :
*    Watering System task
*
*END------------------------------------------------------------------*/
void watering_task(uint32_t initial_data){
    watering_system(44*HOUR ,2*DAY, 2, 10*SECOND, 1*DAY);
    _task_block();
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : http_server_task
* Comments       :
*    HTTP Server task
*
*END------------------------------------------------------------------*/
void http_server_task(uint32_t initial_data){
    http_server();
    _task_block();
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : rtcs_init_task
* Comments       :
*    RTCS Network Initialization
*
*END------------------------------------------------------------------*/
void rtcs_init_task(uint32_t initial_data){
    rtcs_init();
    _task_block();
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : button_task
* Comments       :
*    Button control
*
*END------------------------------------------------------------------*/
void button_task(uint32_t initial_data){
    button();
    _task_block();
}
