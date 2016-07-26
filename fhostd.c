/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) 2016 Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2016-07-12
 *      Updated:
 *
 *      Project:    fhostd
 *      File name:  fhostd.c
 *
 *
 *      Version history mapped on changes in this file:
 *      -----------------------------------------------
 *      ver 1       Created
 *
 *
 *      Reference:
 *
 ******************************************************************************/

#include <sys/socket.h>
//#include <sys/unistd.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fhost.h"

static int hostfound(const char *name);

int main(int argc, const char * argv[]) {
    int socket_fd;
    struct sockaddr_in servaddr, cliaddr;
    int check;
    socklen_t len;
    char buff[INET_ADDRSTRLEN * 5];
    ssize_t n;
    
// if fhostd are supposed to run as a daemon
#ifdef DAEMON
    syslog(LOG_INFO|LOG_DAEMON, "Starting %s\n", PRGM_NAME );
#endif
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
    servaddr.sin_port = htons(SERVERPORT);
    
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (0 >  socket_fd) {
        NOT_OF_TERM("socket error\n");
        exit(EXIT_FAILURE);
    }

    check = bind(socket_fd, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    if (0 > check) {
        NOT_OF_TERM("bind error\n");
        exit(EXIT_FAILURE);
    }
    
    for (;;) {
        len = sizeof(cliaddr);
        bzero(&cliaddr, len);
        
        n = recvfrom(socket_fd, buff, (INET_ADDRSTRLEN * 5), 0, (struct sockaddr *) &cliaddr, &len);
        if (0 > n) {
            NOT_OF_TERM("recvfrom error\n");
            exit(EXIT_FAILURE);
        }
        buff[n] = '\0';
        
        if ( hostfound(buff) ) {
            //inet_ntop(AF_INET, &(cliaddr.sin_addr), buff, INET_ADDRSTRLEN);
            //printf("%s, request from %s\n", PRGM_NAME, buff);
            
            n = sendto(socket_fd, SERV_RESP, SERV_RESP_LEN, 0, (struct sockaddr *) &cliaddr, len);
            if (0 > n) {
                NOT_OF_TERM("sendto error\n");
                exit(EXIT_FAILURE);
            } // if
        } // if
    } // for
    
    return 0;
}


static int hostfound(const char *name)
{
    int ret = 0;
    char hostname[MAX_LEN_HOSTNAME];
    int check;

    check = gethostname(hostname, MAX_LEN_HOSTNAME);
    if (0 != check) {
        NOT_OF_TERM("gethostname error\n");
        exit(EXIT_FAILURE);
    }
    
    check = strncmp(hostname, name, MAX_LEN_HOSTNAME);
    if (0 == check) {
        ret = 1;
    }
    
    return ret;
}
