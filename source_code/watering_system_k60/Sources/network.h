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
#include "watering_system.h"

void http_server(void); 
void rtcs_init(void);

#define ENET_DEVICE 0
#define RTCS_DHCP 1

#define ENET_IPADDR IPADDR(192,168,0,102)
#define ENET_IPMASK IPADDR(255,255,255,0)
#define ENET_GATEWAY IPADDR(192,168,0,1)

#define CGI_STRING_BUFFER_SIZE 15

#endif /* NETWORK_H_ */
