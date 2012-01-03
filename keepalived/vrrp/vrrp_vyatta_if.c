#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <net/if.h>
#include "vrrp_vyatta_if.h"

void vyatta_if_write_sysctl_value(const char * outpath, int value){
   int rc = 0;
   FILE *fp;
   fp = fopen(outpath, "w");
   if (!fp){
     syslog(LOG_INFO, "couldn't open sysctl path: %s\n",outpath);
     return;
   }
   rc = fprintf(fp, "%d\n", value);
   if (rc < 0)
     syslog(LOG_INFO,"couldn't write to sysctl path: %s\n",outpath);
   fclose(fp);
}

void vyatta_if_write_accept_local(const char * ifname){
   char ipv4path[] = "net/ipv4/conf/";
   char accept_local[] = "/accept_local";
   int al_val = 1;
   char *outpath;
   outpath = malloc(strlen(PROC_PATH) + strlen(ipv4path) + 
                    strlen(ifname) + strlen(accept_local) + 1);
   strcpy(outpath, PROC_PATH);
   strcat(outpath, ipv4path);
   strcat(outpath, ifname);
   strcat(outpath, accept_local);
   vyatta_if_write_sysctl_value(outpath, al_val);
   free(outpath);
}

void vyatta_if_write_arp_filter(const char * ifname){
   char ipv4path[] = "net/ipv4/conf/";
   char arp_filter[] = "/arp_filter";
   int arp_val = 0;
   char *outpath;
   outpath = malloc(strlen(PROC_PATH) + strlen(ipv4path) + 
                    strlen(ifname) + strlen(arp_filter) + 1);
   strcpy(outpath, PROC_PATH);
   strcat(outpath, ipv4path);
   strcat(outpath, ifname);
   strcat(outpath, arp_filter);
   vyatta_if_write_sysctl_value(outpath, arp_val);
   free(outpath);
}

void vyatta_if_write_ipv6_disable(const char * ifname){
   char ipv6path[] = "net/ipv6/conf/";
   char disable_ipv6[] = "/disable_ipv6";
   int ipv6_val = 1;
   char *outpath;
   outpath = malloc(strlen(PROC_PATH) + strlen(ipv6path) + 
                    strlen(ifname) + strlen(disable_ipv6) + 1);
   strcpy(outpath, PROC_PATH);
   strcat(outpath, ipv6path);
   strcat(outpath, ifname);
   strcat(outpath, disable_ipv6);
   vyatta_if_write_sysctl_value(outpath, ipv6_val);
   free(outpath);
}

void vyatta_if_write_sysctl_values(const char * ifname){
  vyatta_if_write_accept_local(ifname);
  vyatta_if_write_arp_filter(ifname);
  /* Disable ipv6 for now until we do VRRPv3 support */
  vyatta_if_write_ipv6_disable(ifname);
}

void vyatta_if_create_iptables_igmp_filter(const char * ifname){
  /* This is to work around an odd limitation in the macvlan driver.
   
   * We must listen for multicast traffic on the vmac interface, 
   * because any traffic sourced from the vmac address will be 
   * filtered by the macvlan driver and not recieved by the 
   * parent ethernet interface.

   * Ideally we don't want to listen on vmac interfaces at all.
   * That is we would ideally be able to send the adverts on the vmac interface
   * and then listen for adverts on the parent. (this can work since a seperate
   * socket descriptor is allocated for send and recieve per vrrp instance).
   
   * We can't send IGMP join messages with the vmac address as this
   * confuses switch learning and could possibly lead to service disruptions. We 
   * join on both parent and vmac, then filter the outbound IGMP on the 
   * vmac interface with iptables.
   */
  char iptables_begin[] = "/sbin/iptables -t raw -I VYATTA_VRRP_OUTPUT_FILTER -o ";
  char iptables_end[] = " -p 2 -j DROP";
  char * cmdout;
  cmdout = malloc(strlen(iptables_begin) + strlen(ifname) + 
                  strlen(iptables_end) + 1);
  strcpy(cmdout, iptables_begin);
  strcat(cmdout, ifname);
  strcat(cmdout, iptables_end);
  system(cmdout);
  free(cmdout);
}

void vyatta_if_drop_iptables_igmp_filter(const char * ifname){
  char iptables_begin[] = "/sbin/iptables -t raw -D VYATTA_VRRP_OUTPUT_FILTER -o ";
  char iptables_end[] = " -p 2 -j DROP";
  char * cmdout;
  cmdout = malloc(strlen(iptables_begin) + strlen(ifname) + 
                  strlen(iptables_end) + 1);
  strcpy(cmdout, iptables_begin);
  strcat(cmdout, ifname);
  strcat(cmdout, iptables_end);
  system(cmdout);
  free(cmdout);
}

void vyatta_if_create_iptables_input_filter(const char * ifname){
  char iptables_begin[] = "/sbin/iptables -t raw -I VYATTA_VRRP_FILTER -i ";
  char iptables_end[] = " ! -p 112 -j DROP";
  char * cmdout;
  cmdout = malloc(strlen(iptables_begin) + strlen(ifname) + 
                  strlen(iptables_end) + 1);
  strcpy(cmdout, iptables_begin);
  strcat(cmdout, ifname);
  strcat(cmdout, iptables_end);
  system(cmdout);
  free(cmdout);
}

void vyatta_if_drop_iptables_input_filter(const char * ifname){
  char iptables_begin[] = "/sbin/iptables -t raw -D VYATTA_VRRP_FILTER -i ";
  char iptables_end[] = " ! -p 112 -j DROP";
  char * cmdout;
  cmdout = malloc(strlen(iptables_begin) + strlen(ifname) + 
                  strlen(iptables_end) + 1);
  strcpy(cmdout, iptables_begin);
  strcat(cmdout, ifname);
  strcat(cmdout, iptables_end);
  system(cmdout);
  free(cmdout);
}

void vyatta_if_setup(const char * ifname){
  vyatta_if_write_sysctl_values(ifname);
  /* Drop Iptables filter before creating to ensure we don't get duplicates */
  vyatta_if_drop_iptables_igmp_filter(ifname);
  vyatta_if_create_iptables_igmp_filter(ifname);
}
