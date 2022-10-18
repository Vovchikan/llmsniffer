#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h> /* bzero, memset */


#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/if_packet.h> /* struct sockaddr_ll */
#include <arpa/inet.h>
#include <asm/types.h>
#include <net/if.h> /* struct ifreq */
#include <net/ethernet.h> /* the L2 protocols */

#define OSPF_PROTOCOL 89
typedef enum {
  false,
  true
} bool;

void Bind(int, const struct sockaddr *, socklen_t);
int  Socket(int, int, int);
#endif
