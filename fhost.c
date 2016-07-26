/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) 2016 Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2016-07-08
 *      Updated:
 *
 *      Project:    fhost
 *      File name:  fhost.c
 *
 *
 *      Version history mapped on changes in this file:
 *      -----------------------------------------------
 *      ver 1       Created
 *
 *
 *      Reference:
 *
 *******************************
 *
 *      fhost [ | bcast_addr hostname]
 *
 *      SYNOPSIS:
 *
 *          fhost                           Gives the local host interfaces.
 *
 *          fhost <bcast_addr> <hostname>   Find a host, running fhostd, with 
 *                                          given hostname in current subnet
 *                                          indirect defined by the broadcast
 *                                          address.
 *
 ******************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include "fhost.h"

static void get_local_interfaces(void);
static void get_host(const char *bcast, const char *hname);
static void send_broadcast(int sockfd, const struct sockaddr *paddr, socklen_t slen, const char *sname);
static void receive_feedback(int sockfd, struct sockaddr *paddr, socklen_t slen, const char *sname);


// call expected on the form: fhost [ | bcast_addr hostname]
// IPv4 only
int main(int argc, const char * argv[]) {

    if (argc == 1) {
        get_local_interfaces();
    }
    else if (argc == 3) {
        get_host(argv[1], argv[2]);
    }
    else {
        NOT_OF_TERM("Two, or none, inparameters expected, \"fhost <broadcast addr> <hostname>\"\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}


static void get_host(const char *bcast, const char *hname)
{
    int check;
    int socket_fd;
    struct sockaddr_in servaddr;
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;;
    servaddr.sin_port = htons(SERVERPORT);
    check = inet_pton(AF_INET, bcast, &servaddr.sin_addr);
    if (0 > check) {
        NOT_OF_TERM("inet_pton error\n");
        exit(EXIT_FAILURE);
    }
    else if (0 == check) {
        NOT_OF_TERM("broadcast address not correct\n");
        exit(EXIT_FAILURE);
    }
    
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 >  socket_fd) {
        NOT_OF_TERM("socket error\n");
        exit(EXIT_FAILURE);
    }
    
    send_broadcast(socket_fd, (struct sockaddr *) &servaddr, sizeof(servaddr), hname);
    receive_feedback(socket_fd, (struct sockaddr *) &servaddr, sizeof(servaddr), hname);
    
    check = close(socket_fd);
    if (check  < 0) {
        NOT_OF_TERM("close error\n");
        exit(EXIT_FAILURE);
    }

    return;
}

static void send_broadcast(int sockfd, const struct sockaddr *paddr, socklen_t slen, const char *sname)
{
    int check;
    ssize_t nbytes;
    
    // enable broadcast
    check = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &(int){ 1 }, sizeof(int));
    if (0 > check) {
        NOT_OF_TERM("setsockopt error\n");
        exit(EXIT_FAILURE);
    }

    // send broadcast
    nbytes = sendto(sockfd, sname, strlen(sname), 0, paddr, slen);
    if (0 > nbytes) {
        NOT_OF_TERM("sendto error\n");
        exit(EXIT_FAILURE);
    }
    
    // disable broadcast
    check = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &(int){ 0 }, sizeof(int));
    if (0 > check) {
        NOT_OF_TERM("setsockopt error\n");
        exit(EXIT_FAILURE);
    }
    
    return;
}


static void receive_feedback(int sockfd, struct sockaddr *paddr, socklen_t slen, const char *sname)
{
    socklen_t len = slen;
    struct timeval timeout;
    fd_set read_fds;
    int resp;
    char buff[INET_ADDRSTRLEN * 5];
    ssize_t n;
    
    // timeout after 4.5 seconds...
    timeout.tv_sec = 4;
    timeout.tv_usec = 500000;

    while (1) {
        bzero(((struct sockaddr_in *) paddr), sizeof(struct sockaddr_in));
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        resp = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
        if (0 > resp) {
            NOT_OF_TERM("select error\n");
            exit(EXIT_FAILURE);
        }
        else if (0 == resp) {   // timeout...
            printf("%s timed out...\n", PRGM_NAME);
            break;
        }
        else {                      // the server is found...
            n = recvfrom(sockfd, buff, (INET_ADDRSTRLEN * 5), 0, paddr, &len);
            if (0 > n) {
                NOT_OF_TERM("recvfrom error\n");
                exit(EXIT_FAILURE);
            }
            buff[n] = '\0';
            
            if (0 == strncmp(buff, SERV_RESP, SERV_RESP_LEN)) {
                inet_ntop(AF_INET, &(((struct sockaddr_in *) paddr)->sin_addr), buff, INET_ADDRSTRLEN);
                printf("%s found at %s\n", sname, buff);
                break;
            }
            else {  // to be removed...
                inet_ntop(AF_INET, &(((struct sockaddr_in *) paddr)->sin_addr), buff, INET_ADDRSTRLEN);
                printf("respond from %s, not the fhostd server...\n", buff);
            } // else
        } // else
    } // while
    
    FD_ZERO(&read_fds);
    return;
}


static void get_local_interfaces(void)
{
    char hostname[MAX_LEN_HOSTNAME];
    struct ifaddrs *ifa, *pifa;
    struct sockaddr_in *saddr;
    int check;
    
    check = gethostname(hostname, MAX_LEN_HOSTNAME);
    if (0 != check) {
        NOT_OF_TERM("gethostname error\n");
        exit(EXIT_FAILURE);
    }
    
    check = getifaddrs(&ifa);
    if (0 != check) {
        NOT_OF_TERM("getifaddrs error\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Interface(s) at %s:\n", hostname);
    for (pifa = ifa; pifa; pifa = pifa->ifa_next) {
        if (AF_INET == pifa->ifa_addr->sa_family) {
            printf("%s:\n", pifa->ifa_name);
            
            saddr = (struct sockaddr_in *) pifa->ifa_addr;
            printf("    address:           %s\n", inet_ntoa(saddr->sin_addr));
            
            saddr = (struct sockaddr_in *) pifa->ifa_netmask;
            printf("    subnet mask:       %s\n", inet_ntoa(saddr->sin_addr));
            
            saddr = (struct sockaddr_in *) pifa->ifa_dstaddr;
            printf("    broadcast address: %s\n", inet_ntoa(saddr->sin_addr));
        }
    }
    
    freeifaddrs(ifa);
    return;
}

