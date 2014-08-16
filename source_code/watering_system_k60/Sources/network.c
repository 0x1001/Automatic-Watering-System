/*
 * network.c
 *
 *  Created on: Aug 14, 2014
 *      Author: Dev
 */

#include "network.h"

static _mqx_int cgi_water(HTTPSRV_CGI_REQ_STRUCT* param);

static _mqx_int ssi_progress_left(HTTPSRV_SSI_PARAM_STRUCT* param);
static _mqx_int ssi_progress_percent(HTTPSRV_SSI_PARAM_STRUCT* param);
static _mqx_int ssi_pumping_time(HTTPSRV_SSI_PARAM_STRUCT* param);
static _mqx_int ssi_start_delay(HTTPSRV_SSI_PARAM_STRUCT* param);
static _mqx_int ssi_time_between_watering(HTTPSRV_SSI_PARAM_STRUCT* param);
static _mqx_int ssi_watering_cycles(HTTPSRV_SSI_PARAM_STRUCT* param);
static _mqx_int ssi_dry_time(HTTPSRV_SSI_PARAM_STRUCT* param);

static void server_mount_tfs(void);
static void server_configure(HTTPSRV_PARAM_STRUCT *params);
static uint32_t cgi_write(char *data, HTTPSRV_CGI_RES_STRUCT *response, HTTPSRV_CONTENT_TYPE type);

const HTTPSRV_SSI_LINK_STRUCT fn_lnk_tbl[] = {
	{ "p_percent", ssi_progress_percent },
    { "p_left", ssi_progress_left },
    { "ws_pt", ssi_pumping_time },
    { "ws_dt", ssi_dry_time },
    { "ws_wc", ssi_watering_cycles },
    { "ws_tbw", ssi_time_between_watering },
    { "ws_sd", ssi_start_delay },
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
	_enet_address enet_address;   

	uint32_t error = RTCS_create();
   
	if (error != RTCS_OK){
		printf("\nRTCS failed to initialize, error = %X", error);
		return;
	}

	_IP_forward = TRUE;

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

	LWDNS_server_ipaddr = ENET_GATEWAY;   
	ipcfg_add_dns_ip(ENET_DEVICE,LWDNS_server_ipaddr);

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
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : http_server_task
* Comments       :
*    HTTP Server
*
*END------------------------------------------------------------------*/
void http_server(void){
	HTTPSRV_PARAM_STRUCT params;
	
	server_mount_tfs();
	server_configure(&params);

    if(!HTTPSRV_init(&params))
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

    watering_system_pump_water(5*SECOND);
	
    if (param->request_method != HTTPSRV_REQ_GET)
        return(0);
    
    response.ses_handle = param->ses_handle;
    return cgi_write("<html><head><meta http-equiv='refresh' content='0; url=/' /></head></html>",&response,HTTPSRV_CONTENT_TYPE_HTML);
}

/*SSI*-----------------------------------------------------------------
*
* Function Name  : ssi_progress_left
* Comments       :
*    SSI Function
*
*END------------------------------------------------------------------*/
static _mqx_int ssi_progress_left(HTTPSRV_SSI_PARAM_STRUCT* param){
	watering_system_progress_ptr progress;
	uint32_t hours_left;
	uint32_t minutes_left;
	char data[CGI_STRING_BUFFER_SIZE];
	
	progress = watering_system_get_progress();
	hours_left = (progress->total_time - progress->passed_time)/HOUR;
	minutes_left = ((progress->total_time - progress->passed_time)%HOUR)/MINUTE;

	if (hours_left < 10 && minutes_left < 10)
		sprintf(data, "0%d:0%d", hours_left, minutes_left);
	else if (hours_left >= 10 && minutes_left < 10)
		sprintf(data, "%d:0%d", hours_left, minutes_left);
	else if (hours_left < 10 && minutes_left >= 10)
		sprintf(data, "0%d:%d", hours_left, minutes_left);
	else
		sprintf(data, "%d:%d", hours_left, minutes_left);
	
    HTTPSRV_ssi_write(param->ses_handle, data, strlen(data));
    
    return 0;
}

/*SSI*-----------------------------------------------------------------
*
* Function Name  : ssi_progress_percent
* Comments       :
*    SSI Function
*
*END------------------------------------------------------------------*/
static _mqx_int ssi_progress_percent(HTTPSRV_SSI_PARAM_STRUCT* param){
	watering_system_progress_ptr progress;
	uint32_t percent_progress;
	char template_percent[] = "%d";
	char data[CGI_STRING_BUFFER_SIZE];
	
	progress = watering_system_get_progress();
	percent_progress = (progress->passed_time*100)/progress->total_time;

	sprintf(data, template_percent, percent_progress);
		
    HTTPSRV_ssi_write(param->ses_handle, data, strlen(data));
    
    return 0;
}

/*SSI*-----------------------------------------------------------------
*
* Function Name  : ssi_pumping_time
* Comments       :
*    SSI Function
*
*END------------------------------------------------------------------*/
static _mqx_int ssi_pumping_time(HTTPSRV_SSI_PARAM_STRUCT* param){
	watering_system_params_ptr ws_params;
	char template[] = "%d";
	char data[CGI_STRING_BUFFER_SIZE];
	
	ws_params = watering_system_get_params();
	sprintf(data, template, ws_params->pumping_time/SECOND);
		
    HTTPSRV_ssi_write(param->ses_handle, data, strlen(data));
    
    return 0;
}

/*SSI*-----------------------------------------------------------------
*
* Function Name  : ssi_start_delay
* Comments       :
*    SSI Function
*
*END------------------------------------------------------------------*/
static _mqx_int ssi_start_delay(HTTPSRV_SSI_PARAM_STRUCT* param){
	watering_system_params_ptr ws_params;
	char template[] = "%d";
	char data[CGI_STRING_BUFFER_SIZE];
	
	ws_params = watering_system_get_params();
	sprintf(data, template, ws_params->start_delay/MINUTE);
		
    HTTPSRV_ssi_write(param->ses_handle, data, strlen(data));
    
    return 0;
}

/*SSI*-----------------------------------------------------------------
*
* Function Name  : ssi_time_between_watering
* Comments       :
*    SSI Function
*
*END------------------------------------------------------------------*/
static _mqx_int ssi_time_between_watering(HTTPSRV_SSI_PARAM_STRUCT* param){
	watering_system_params_ptr ws_params;
	char template[] = "%d";
	char data[CGI_STRING_BUFFER_SIZE];
	
	ws_params = watering_system_get_params();
	sprintf(data, template, ws_params->time_between_watering/MINUTE);
		
    HTTPSRV_ssi_write(param->ses_handle, data, strlen(data));
    
    return 0;
}

/*SSI*-----------------------------------------------------------------
*
* Function Name  : ssi_watering_cycles
* Comments       :
*    SSI Function
*
*END------------------------------------------------------------------*/
static _mqx_int ssi_watering_cycles(HTTPSRV_SSI_PARAM_STRUCT* param){
	watering_system_params_ptr ws_params;
	char template[] = "%d";
	char data[CGI_STRING_BUFFER_SIZE];
	
	ws_params = watering_system_get_params();
	sprintf(data, template, ws_params->watering_cycles);
		
    HTTPSRV_ssi_write(param->ses_handle, data, strlen(data));
    
    return 0;
}

/*SSI*-----------------------------------------------------------------
*
* Function Name  : ssi_dry_time
* Comments       :
*    SSI Function
*
*END------------------------------------------------------------------*/
static _mqx_int ssi_dry_time(HTTPSRV_SSI_PARAM_STRUCT* param){
	watering_system_params_ptr ws_params;
	char template[] = "%d";
	char data[CGI_STRING_BUFFER_SIZE];
	
	ws_params = watering_system_get_params();
	sprintf(data, template, ws_params->dry_time/MINUTE);
		
    HTTPSRV_ssi_write(param->ses_handle, data, strlen(data));
    
    return 0;
}

void server_mount_tfs(void){
	extern const TFS_DIR_ENTRY tfs_data[];
	_io_tfs_install("tfs:", tfs_data);
}

void server_configure(HTTPSRV_PARAM_STRUCT *params){
	_mem_zero(params,sizeof(HTTPSRV_PARAM_STRUCT));
    params->af = AF_INET;
    params->root_dir = "tfs:";
    params->index_page = "\\index.shtml";
    params->cgi_lnk_tbl = (HTTPSRV_CGI_LINK_STRUCT*)cgi_lnk_tbl;
    params->ssi_lnk_tbl = (HTTPSRV_SSI_LINK_STRUCT*)fn_lnk_tbl;
    params->script_stack = 3000;
}

uint32_t cgi_write(char *data, HTTPSRV_CGI_RES_STRUCT *response, HTTPSRV_CONTENT_TYPE type){
    response->content_type = type;
    response->status_code = 200;        
    response->data = data;
    response->data_length = strlen(data);
    response->content_length = response->data_length;
    /* Send response */
    HTTPSRV_cgi_write(response);
    
    return response->content_length;
}
