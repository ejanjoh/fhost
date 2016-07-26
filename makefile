################################################################################
#
#       Autor:      Jan Johansson (ejanjoh)
#       Copyright:  Copyright (c) 2016 Jan Johansson (ejanjoh). 
#                   All rights reserved.
#       Created:    2016-07-26
#       Updated:    
#
#       Project:    fhost
#       File name:  makefile
#
#
#       Version history mapped on changes in this file:
#       -----------------------------------------------
#       ver1:       Created
#
################################################################################


# Rule to make it all:
all :
	gcc -Wall fhost.c -o fhost
	gcc -Wall -DDAEMON fhostd.c -o fhostd

# Rule to make the client
fhost :
	gcc -Wall fhost.c -o fhost

# Rule to make the service provided server a daemon
fhostd :
	gcc -Wall -DDAEMON fhostd.c -o fhostd






