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

void vyatta_if_write_link_filter(const char * ifname){
   char ipv4path[] = "net/ipv4/conf/";
   char link_filter[] = "/link_filter";
   int lf_val = 2;
   char *outpath;
   outpath = malloc(strlen(PROC_PATH) + strlen(ipv4path) + 
                    strlen(ifname) + strlen(link_filter) + 1);
   strcpy(outpath, PROC_PATH);
   strcat(outpath, ipv4path);
   strcat(outpath, ifname);
   strcat(outpath, link_filter);
   vyatta_if_write_sysctl_value(outpath, lf_val);
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
  vyatta_if_write_link_filter(ifname);
  /* Disable ipv6 for now until we do VRRPv3 support */
  vyatta_if_write_ipv6_disable(ifname);
}

void vyatta_if_setup(const char * ifname){
  vyatta_if_write_sysctl_values(ifname);
}
