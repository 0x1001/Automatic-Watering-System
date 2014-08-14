/*
 * network.h
 *
 *  Created on: Aug 14, 2014
 *      Author: Dev
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <rtcs.h>
#include "tfs.h"

#define HTTP_SERVER 2
#define RTCS_INIT 3

void http_server_task(uint32_t initial_data); 
void rtcs_init_task(uint32_t initial_data); 



#include <rtcs.h>
#ifdef APPLICATION_HAS_SHELL
#include <sh_rtcs.h>
#endif
#include <ipcfg.h>



#define ENET_DEVICE 0
#define RTCS_DHCP 1

#define ENET_IPADDR IPADDR(169,254,3,3)
#define ENET_IPMASK IPADDR(255,255,0,0)
#define ENET_GATEWAY IPADDR(0,0,0,0)
#define RTCS_PPP 0

/* PPP device must be set manually and 
** must be different from the default IO channel (BSP_DEFAULT_IO_CHANNEL) 
*/
#define PPP_DEVICE      "ittyb:" 

/*
** Define PPP_DEVICE_DUN only when using PPP to communicate
** to Win9x Dial-Up Networking over a null-modem
** This is ignored if PPP_DEVICE is not #define'd
*/
#define PPP_DEVICE_DUN  1

/*
** Define the local and remote IP addresses for the PPP link
** These are ignored if PPP_DEVICE is not #define'd
*/
#define PPP_LOCADDR     IPADDR(192,168,0,216)
#define PPP_PEERADDR    IPADDR(192,168,0,217)

/*
** Define a default gateway
*/
#define GATE_ADDR       IPADDR(192,168,0,1)

#endif /* NETWORK_H_ */
