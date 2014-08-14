/*
 * network.h
 *
 *  Created on: Aug 14, 2014
 *      Author: Dev
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <rtcs.h>
#include <ipcfg.h>
#include "tfs.h"

#define HTTP_SERVER 2
#define RTCS_INIT 3

void http_server_task(uint32_t initial_data); 
void rtcs_init_task(uint32_t initial_data); 

#define ENET_DEVICE 0
#define RTCS_DHCP 1

#define ENET_IPADDR IPADDR(192,168,0,102)
#define ENET_IPMASK IPADDR(255,255,255,0)
#define ENET_GATEWAY IPADDR(192,168,0,1)

#endif /* NETWORK_H_ */
