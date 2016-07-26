/*******************************************************************************
 *
 *      Autor:      Jan Johansson (ejanjoh)
 *      Copyright:  Copyright (c) 2016 Jan Johansson (ejanjoh). All rights reserved.
 *      Created:    2016-07-08
 *      Updated:
 *
 *      Project:    fhost
 *      File name:  fhost.h
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

#ifndef fhost_h
#define fhost_h

#include <sys/errno.h>
#include <syslog.h>
#include <stdio.h>

#ifdef DAEMON
    #define PRGM_NAME "fhostd"
#else
    #define PRGM_NAME "fhost"
#endif

#ifdef DEAMON_DEBUG
    #undef PRGM_NAME
    #define PRGM_NAME "fhostd"
#endif

#define MAX_LEN_HOSTNAME 128
#define SERVERPORT 50001            // to be changed to whatever you like...
#define SERV_RESP "SERVER_FOUND"
#define SERV_RESP_LEN 12

// Notification of termination
#ifdef DAEMON
    #define NOT_OF_TERM(...) \
    do { \
        syslog(LOG_ERR|LOG_DAEMON, "%s will terminate due to an unexpected error or occurrence...\n", PRGM_NAME ); \
        syslog(LOG_ERR|LOG_DAEMON, "file: %s\n line: %d\n function: %s\n", __FILE__,__LINE__, __func__); \
        syslog(LOG_ERR|LOG_DAEMON, "errno: %d\n", errno); \
        syslog(LOG_ERR|LOG_DAEMON, __VA_ARGS__); \
    } while(0)
#else
    #define NOT_OF_TERM(...) \
    do { \
        fprintf(stderr, "%s will terminate due to an unexpected error or occurrence...\n", PRGM_NAME); \
        fprintf(stderr, "file: %s\n line: %d\n function: %s\n", __FILE__,__LINE__, __func__); \
        fprintf(stderr, "errno: %d\n", errno); \
        fprintf(stderr,  __VA_ARGS__); \
    } while(0)
#endif

#endif /* fhost_h */
