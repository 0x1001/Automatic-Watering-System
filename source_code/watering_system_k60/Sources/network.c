/*
 * network.c
 *
 *  Created on: Aug 14, 2014
 *      Author: Dev
 */

#include "network.h"

static _mqx_int cgi_water(HTTPSRV_CGI_REQ_STRUCT* param);
static _mqx_int ssi_info(HTTPSRV_SSI_PARAM_STRUCT* param);

const HTTPSRV_SSI_LINK_STRUCT fn_lnk_tbl[] = { 
    { "info", ssi_info },
    { 0, 0 }
};

const HTTPSRV_CGI_LINK_STRUCT cgi_lnk_tbl[] = {
    { "water",        cgi_water, 1500},
    { 0, 0 }    // DO NOT REMOVE - last item - end of table
};

/*TASK*-----------------------------------------------------------------
*
* Function Name  : rtcs_init_task
* Comments       :
*    RTCS Network Initialization
*
*END------------------------------------------------------------------*/
void rtcs_init(void){
   IPCFG_IP_ADDRESS_DATA ip_data;
   _enet_address           enet_address;   

   uint32_t error = RTCS_create();
   
   if (error != RTCS_OK){
      printf("\nRTCS failed to initialize, error = %X", error);
      return;
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
   
   if (error != RTCS_OK) {
      printf("\nFailed to initialize ethernet device, error = %X", error);
      return;
   }

#if RTCSCFG_ENABLE_LWDNS
   LWDNS_server_ipaddr = ENET_GATEWAY;   
   ipcfg_add_dns_ip(ENET_DEVICE,LWDNS_server_ipaddr);
#endif /* RTCSCFG_ENABLE_LWDNS */

   printf("\nWaiting for ethernet cable plug in ... ");
   while(!ipcfg_get_link_active(ENET_DEVICE)) {};
   printf("Cable connected\n");

   printf("Contacting DHCP server ... ");
   error = ipcfg_bind_dhcp_wait(ENET_DEVICE, FALSE, &ip_data);

   if (error != IPCFG_ERROR_OK){
      printf("\nRTCS failed to bind interface with IPv4, error = %X", error);
      return;
   }
   else
       printf("OK\n");       

   ipcfg_get_ip(ENET_DEVICE, &ip_data);
   printf("\nIP Address      : %d.%d.%d.%d\n",IPBYTES(ip_data.ip));
   printf("\nSubnet Address  : %d.%d.%d.%d\n",IPBYTES(ip_data.mask));
   printf("\nGateway Address : %d.%d.%d.%d\n",IPBYTES(ip_data.gateway));
   printf("\nDNS Address     : %d.%d.%d.%d\n",IPBYTES(ipcfg_get_dns_ip(ENET_DEVICE,0)));

#endif /* BSP_ENET_DEVICE_COUNT > 0 */
#endif /* BSP_ENET_DEVICE_COUNT */
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : http_server_task
* Comments       :
*    HTTP Server
*
*END------------------------------------------------------------------*/
void http_server(void){
    uint32_t server;
    int32_t error;
    extern const TFS_DIR_ENTRY tfs_data[];
    HTTPSRV_PARAM_STRUCT params;
    
    error = _io_tfs_install("tfs:", tfs_data);
    if (error)
        printf("\nTFS install returned: %08x\n", error);

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
        printf("Error: HTTP server init error.\n");
}

/*CGI*-----------------------------------------------------------------
*
* Function Name  : cgi_water
* Comments       :
*    CGI Function
*
*END------------------------------------------------------------------*/
static _mqx_int cgi_water(HTTPSRV_CGI_REQ_STRUCT* param){
    HTTPSRV_CGI_RES_STRUCT response;

    char str[]="CGI Wroks!";
    
    if (param->request_method != HTTPSRV_REQ_GET)
        return(0);

    
    response.ses_handle = param->ses_handle;
    response.content_type = HTTPSRV_CONTENT_TYPE_PLAIN;
    response.status_code = 200;        
    response.data = str;
    response.data_length = strlen(str);
    response.content_length = response.data_length;
    /* Send response */
    HTTPSRV_cgi_write(&response);
    
    return (response.content_length);
}

/*SSI*-----------------------------------------------------------------
*
* Function Name  : ssi_info
* Comments       :
*    SSI Function
*
*END------------------------------------------------------------------*/
static _mqx_int ssi_info(HTTPSRV_SSI_PARAM_STRUCT* param){
    char str[]="kkk123";
    
    HTTPSRV_ssi_write(param->ses_handle, str, strlen(str));
    
    return 0;
}
