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
#include "pump.h"

#include "tfs.h"

#if !defined(RAM_DISK_SIZE)
#error Please specify RAM_DISK_SIZE
#endif

#if defined(APPLICATION_HAS_SHELL) && (!SHELLCFG_USES_RTCS)
#error This application requires SHELLCFG_USES_RTCS defined non-zero in user_config.h. Please recompile libraries with this option if any Ethernet interface is available.
#endif

#define HTTP_SERVER   2
void http_server_task(uint32_t initial_data); 

HTTPSRV_ALIAS http_aliases[] = {
    {"/usb/", "c:\\"},
    {NULL, NULL}
    };

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack, Pri, String, Auto? */
   {MAIN_TASK,   Main_task,   2000,  9,   "main", MQX_AUTO_START_TASK},
   {HTTP_SERVER,   http_server_task,   1400,  9,   "HTTP_SERVER", MQX_AUTO_START_TASK,   0,  0},
   {0,           0,           0,     0,   0,      0,                 }
};


/*TASK*-----------------------------------------------------------------
*
* Function Name  : Main_task
* Comments       :
*    Pump control
*
*END------------------------------------------------------------------*/

void Main_task(uint32_t initial_data)
{
   /* Install ramdisk - MFS init */
    //Ram_disk_start();
  
   /* RTCS init */
   rtcs_init();
}

void http_server_task(uint32_t initial_data) {
    uint32_t server;
    int32_t error;
    extern const HTTPSRV_CGI_LINK_STRUCT cgi_lnk_tbl[];
    extern const HTTPSRV_SSI_LINK_STRUCT fn_lnk_tbl[];
    extern const TFS_DIR_ENTRY tfs_data[];
    HTTPSRV_PARAM_STRUCT params;
    
    error = _io_tfs_install("tfs:", tfs_data);
    if (error) printf("\nTFS install returned: %08x\n", error);

    /* Setup webserver parameters */
    _mem_zero(&params, sizeof(HTTPSRV_PARAM_STRUCT));
    params.af |= AF_INET;
    params.root_dir = "tfs:";
    params.alias_tbl = (HTTPSRV_ALIAS*)http_aliases;
    params.index_page = "\\mqx.shtml";
    //params.cgi_lnk_tbl = (HTTPSRV_CGI_LINK_STRUCT*)cgi_lnk_tbl;
    //params.ssi_lnk_tbl = (HTTPSRV_SSI_LINK_STRUCT*)fn_lnk_tbl;
    params.script_stack = 2500;

    server = HTTPSRV_init(&params);
    if(!server)
    {
        printf("Error: HTTP server init error.\n");
    }
}

void button_pump_control(void){
    LWGPIO_STRUCT btn1;
    pump p;
    
    pump_init(&p);
    
    if (!lwgpio_init(&btn1, BSP_BUTTON1, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
        _task_block();

    lwgpio_set_functionality(&btn1, BSP_BUTTON1_MUX_GPIO);
    lwgpio_set_attribute(&btn1, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);

    while (TRUE)
    {
        if (LWGPIO_VALUE_LOW == lwgpio_get_value(&btn1))
            pump_run(&p,100);
        else
        	_time_delay(100);
   }
}
