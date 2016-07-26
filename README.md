# fhost and fhostd

fhost [ | bcast_addr hostname]

fhostd


### SYNOPSIS:

        fhost                           Gives the local host interfaces.

        fhost <bcast_addr> <hostname>   Find a host, running fhostd, with 
                                        given hostname in current subnet
                                        indirect defined by the broadcast
                                        address.
        
        fhostd                          Start the services on the remote 
                                        host



### Description

    You may have encountered the problem finding a server or another remote host on your local network that not contain a name server. What is the ip number of the headless host, or has the DHCP server currently changed it? There may be many reason why you don't have a name server – no matter what it could potentially cause you some problem. It's of cause possible to log on to the router, using static ip addresses or using nmap or similar tools to sort this out.

    fhost is a simple tool to resolve the ip number of a remote host in your local network. Let the remote host run the server fhostd, preferable as a daemon started at system startup. fhost sends a broadcast with the hostname and the remote host will respond with its ip number. fhost can also provide the local ip address, subnetmask, and the broadcast address in your local network.

    On a Debian system you can put the statup of fhostd in /etc/rc.local. But the startup scripts are system dependent, why you need to look into your specific system. An alternative could be to use daemontools – this will also restart your server if it for some reason will crash.


### Revision History

    - ver 1: Project created.

