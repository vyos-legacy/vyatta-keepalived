#ifndef _VRRP_VYATTA_IF_H
#define _VRRP_VYATTA_IF_H

/* global includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <net/if.h>

/* consts */
static const char PROC_PATH[] = "/proc/sys/";

/* prototypes */
extern void vyatta_if_write_sysctl_value(const char *, int);
extern void vyatta_if_write_accept_local(const char *);
extern void vyatta_if_write_arp_filter(const char *);
extern void vyatta_if_write_ipv6_disable(const char *);
extern void vyatta_if_write_sysctl_values(const char *);
extern void vyatta_if_create_iptables_igmp_filter(const char *);
extern void vyatta_if_setup(const char *);

#endif


