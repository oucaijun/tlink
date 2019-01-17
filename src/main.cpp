#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>    //for network interfaces
#include <sys/ioctl.h>    // to talk to interface
#include <fcntl.h>
#include <errno.h>

#include <string>
#include <vector>
#include <iostream>

typedef enum {
    IF_CONNECT_OK,
    IF_PARA_ERROR,
    IF_NOT_FIND,
    IF_IP_NOT_FIND,
    IF_CONNECT_FAIL,
    IF_CONNECT_TIMEOUT,
}iferr_t;
std::vector <std::string> errstr = {
    "IF_CONNECT_OK",
    "IF_PARA_ERROR",
    "IF_NOT_FIND",
    "IF_IP_NOT_FIND",
    "IF_CONNECT_FAIL",
    "IF_CONNECT_TIMEOUT"
};

iferr_t getLocalIP(std::string interfacename, std::string &ipaddr);
iferr_t try_connect(const std::string& addr, int port, const std::string& ifname, int timeoutsec);

#include<unistd.h>
#include<getopt.h>          /*所在头文件 */
int main(int argc, char *argv[])
{
    if(argc<4) {
        printf("usage: %s -a 192.168.15.25 -p 6305 -i eth0 -t 1 \n", argv[0]);
        printf("return stat: 0 OK, else FAIL\n");
        return IF_PARA_ERROR;
    }

    std::string addr;
    int port;
    std::string ifname;
    int timeoutsec;
    int option;
    while ((option=getopt_long(argc, argv, "a:p:i:t:", NULL, NULL)) !=EOF) {
        switch(option)
        {
        case 'a':
            addr=optarg;
            break;
        case 'p':
            port=atoi(optarg);
            break;
        case 'i':
            ifname=optarg;
            break;
        case 't':
            timeoutsec=atoi(optarg);
            break;
        }
    }
    std::string localipaddr;
    iferr_t status = getLocalIP(ifname, localipaddr);
    if(status == IF_CONNECT_OK) {
	errno=0;
        status = try_connect(addr, port, ifname, timeoutsec);
    }

    if(status == IF_CONNECT_OK)
    {
        printf("Connect Port %d on %s via %s OK\n", port, addr.c_str (), ifname.c_str ());
    } else {
        printf("Connect Port %d on %s via %s FAILED[%s], strerror info: %s\n", port, addr.c_str (), ifname.c_str (), errstr[status].c_str (), strerror(errno));
	//system("netstat -nt | grep 8777");
        //system("route -n");
    }

    return (int)status;
}

#include <net/if.h>    //for network interfaces
#include <arpa/inet.h>    // for get hostbyname
#include <sys/ioctl.h>    // to talk to interface
#include <unistd.h>      // for close()
iferr_t getLocalIP(std::string interfacename, std::string &ipaddr)
{
    int sokcfd;
    struct ifconf ifc;
    struct ifreq *ifr, ifreqbuf[32];
    struct sockaddr_in *sin;
    int n;
    char ip[ INET_ADDRSTRLEN ];

    sokcfd = socket(AF_INET, SOCK_DGRAM,0);
    if( sokcfd < 0 ) {
        std::cout << " Cannot open socket " << std::endl;
        return IF_NOT_FIND;
    }

    ifc.ifc_req = ifreqbuf;
    ifc.ifc_len = sizeof( ifreqbuf );

    // Make a call to get a list of all network interface records
    ioctl( sokcfd, SIOCGIFCONF, &ifc );

    // Work out the number of records
    n = ifc.ifc_len / sizeof( struct ifreq );
    if( n <= 0 ) {
        close( sokcfd );
        return IF_NOT_FIND;
    }

    // While there are still records, save to our string vector.
    iferr_t status = IF_IP_NOT_FIND;
    for( ifr = ifc.ifc_req; n > 0; n--, ifr++ ) {
        // ignore non IPv4 records
        if( ifr->ifr_addr.sa_family != AF_INET ) continue;
        // Soure internet address is?
        sin = (struct sockaddr_in *) &ifr->ifr_addr;
        inet_ntop( AF_INET, (void*)&sin->sin_addr, ip, INET_ADDRSTRLEN);

        std::string ifname(ifr->ifr_ifrn.ifrn_name);
        if(0 == ifname.compare(interfacename)) {
            //addresses.push_back( std::string(ifr->ifr_ifrn.ifrn_name) + ":" + ip);
            ipaddr = ip;
            status = IF_CONNECT_OK;
            break;
        }
    }
    close(sokcfd);

    return status;
}


iferr_t try_connect(const std::string& addr, int port, const std::string& ifname, int timeoutsec)
{
    int mysocket;
    iferr_t status = IF_CONNECT_FAIL;;
    struct sockaddr_in my_addr;

    mysocket = socket(AF_INET, SOCK_STREAM,0);
    if(-1 == mysocket ){
        printf("Socket Error\n");
        exit(IF_CONNECT_FAIL);
    }

    //set time out
    {
        struct timeval timeo = {3, 0};
        timeo.tv_sec = timeoutsec;
        setsockopt(mysocket, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(struct timeval));
    }

    //set ifname
    {
        struct ifreq ifr;
        memset(&ifr, 0x00, sizeof(ifr));
        strncpy(ifr.ifr_name, ifname.c_str (), IF_NAMESIZE);//"eth0"
        setsockopt(mysocket, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr));
    }

    //try connect
    {
        my_addr.sin_family = AF_INET;
        my_addr.sin_port = htons(port);
        my_addr.sin_addr.s_addr = inet_addr(addr.c_str ());
        // If  the connection or binding succeeds, zero is returned
        if(0 == connect(mysocket,(struct sockaddr *)&my_addr,sizeof(my_addr)))
        {
            if (errno == EINPROGRESS) {
                //printf ("connect %s:%d timeout, %s\n", addr.c_str(), port, strerror(errno));
                status = IF_CONNECT_TIMEOUT;
            } else {
                status = IF_CONNECT_OK;
            }
        } else {
            //printf ("connect %s:%d fail, %s\n", addr.c_str(), port, strerror(errno));
            status = IF_CONNECT_FAIL;
        }
    }
    close(mysocket);
    return status;
}

/*
root@imx6qsabresd:~# ./TLink
usage: ./TLink -a 192.168.15.25 -p 6305 -i eth0 -t 1

root@imx6qsabresd:~# ./TLink -a 171.221.254.214 -p 1111 -i usb0 -t 1; echo $?
Connect Port 1111 on 171.221.254.214 via usb0 OK
0

root@imx6qsabresd:~# ./TLink -a 171.221.254.214 -p 1111 -i eth0 -t 1; echo $?
connect: Operation now in progress
Connect Port 1111 on 171.221.254.214 via eth0 FAILED
1

*/
