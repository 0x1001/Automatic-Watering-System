/*
 * network.c
 *
 *  Created on: Aug 14, 2014
 *      Author: Dev
 */

#include "network.h"

static _mqx_int cgi_water(HTTPSRV_CGI_REQ_STRUCT* param){
	HTTPSRV_CGI_RES_STRUCT response;

	char str[]="CGI Wroks!";
	
	if (param->request_method != HTTPSRV_REQ_GET)
	{
		return(0);
	}
	
	response.ses_handle = param->ses_handle;
	response.content_type = HTTPSRV_CONTENT_TYPE_PLAIN;
	response.status_code = 200;        
	response.data = str;
	response.data_length = sizeof(str);
	response.content_length = response.data_length;
	/* Send response */
	HTTPSRV_cgi_write(&response);
	
	return (response.content_length);
}

const HTTPSRV_CGI_LINK_STRUCT cgi_lnk_tbl[] = {
    { "water",        cgi_water, 1500},
    { 0, 0 }    // DO NOT REMOVE - last item - end of table
};

static _mqx_int info(HTTPSRV_SSI_PARAM_STRUCT* param){
    char str[]="kkk123";
    
    HTTPSRV_ssi_write(param->ses_handle, str, strlen(str));
    
    return 0;
}

const HTTPSRV_SSI_LINK_STRUCT fn_lnk_tbl[] = { 
	{ "info", info },
	{ 0, 0 }
};

void http_server_task(uint32_t initial_data) {
    uint32_t server;
    int32_t error;
    //extern const HTTPSRV_CGI_LINK_STRUCT cgi_lnk_tbl[];
    extern const HTTPSRV_SSI_LINK_STRUCT fn_lnk_tbl[];
    extern const TFS_DIR_ENTRY tfs_data[];
    HTTPSRV_PARAM_STRUCT params;
    
    error = _io_tfs_install("tfs:", tfs_data);
    if (error) printf("\nTFS install returned: %08x\n", error);

    /* Setup webserver parameters */
    _mem_zero(&params, sizeof(HTTPSRV_PARAM_STRUCT));
    params.af = AF_INET;
    params.root_dir = "tfs:";
    params.index_page = "\\index.shtml";
    params.cgi_lnk_tbl = (HTTPSRV_CGI_LINK_STRUCT*)cgi_lnk_tbl;
    params.ssi_lnk_tbl = (HTTPSRV_SSI_LINK_STRUCT*)fn_lnk_tbl;
    params.script_stack = 3000;

    server = HTTPSRV_init(&params);
    if(!server)
    {
        printf("Error: HTTP server init error.\n");
    }
    _task_block();
}
















/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
*************************************************************************** 
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
*END************************************************************************/
#include "main.h"
#include <ipcfg.h>

/*FUNCTION*-----------------------------------------------------
* 
* Function Name    : rtcs_init
* Comments     :
*    This function installs and initializes RTCS
*
*END*-----------------------------------------------------*/

void rtcs_init()
{
   IPCFG_IP_ADDRESS_DATA ip_data;
   _enet_address           enet_address;   

   uint32_t error = RTCS_create();
   if (error != RTCS_OK) 
   {
      printf("\nRTCS failed to initialize, error = %X", error);
      _task_block();
   }

   _IP_forward = TRUE;

#ifdef BSP_ENET_DEVICE_COUNT
#if  (BSP_ENET_DEVICE_COUNT > 0) 
   /*Following section is relevant only in case there as some enet driver available in BSP (FEC, MACNET, WIFI or other enet driver)*/
   
   ip_data.ip = ENET_IPADDR;
   ip_data.mask = ENET_IPMASK;
   ip_data.gateway = ENET_GATEWAY;  
   
   /* calcualte unique mac address from IP ADDRES */
   ENET_get_mac_address (ENET_DEVICE, ENET_IPADDR, enet_address);
   error = ipcfg_init_device (ENET_DEVICE, enet_address);
   if (error != RTCS_OK) 
   {
      printf("\nFailed to initialize ethernet device, error = %X", error);
      _task_block();
   }

#if RTCSCFG_ENABLE_LWDNS
   LWDNS_server_ipaddr = ENET_GATEWAY;   
   ipcfg_add_dns_ip(ENET_DEVICE,LWDNS_server_ipaddr);
#endif /* RTCSCFG_ENABLE_LWDNS */

   printf("\nWaiting for ethernet cable plug in ... ");
   while(!ipcfg_get_link_active(ENET_DEVICE)) {};
   printf("Cable connected\n");

#if RTCS_DHCP
   printf("Contacting DHCP server ... ");
   error = ipcfg_bind_dhcp_wait(ENET_DEVICE, FALSE, &ip_data);
#else
   printf("Setting static IP address ... ");
   error = ipcfg_bind_staticip (ENET_DEVICE, &ip_data);
#endif /* RTCS_DHCP */
   if (error != IPCFG_ERROR_OK) 
   {
      printf("\nRTCS failed to bind interface with IPv4, error = %X", error);
      _task_block();
   }
   else
   {
	  printf("OK\n");	   
   }

   ipcfg_get_ip(ENET_DEVICE, &ip_data);
   printf("\nIP Address      : %d.%d.%d.%d\n",IPBYTES(ip_data.ip));
   printf("\nSubnet Address  : %d.%d.%d.%d\n",IPBYTES(ip_data.mask));
   printf("\nGateway Address : %d.%d.%d.%d\n",IPBYTES(ip_data.gateway));
   printf("\nDNS Address     : %d.%d.%d.%d\n",IPBYTES(ipcfg_get_dns_ip(ENET_DEVICE,0)));

#endif /* BSP_ENET_DEVICE_COUNT > 0 */
#endif /* BSP_ENET_DEVICE_COUNT */   
   
#if RTCS_PPP
    //PPP_start();
#endif /* RTCS_PPP */

   /* TODO: start server(s) in separate tasks */
}

void rtcs_init_task(uint32_t initial_data){
	
	rtcs_init();
	_task_block();
}
