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

int try_connect(const std::string& addr, int port, const std::string& ifname, int timeoutsec)
{
    int mysocket;
    int stat=1;
    struct sockaddr_in my_addr;

    mysocket = socket(AF_INET, SOCK_STREAM,0);
    if(-1 == mysocket ){
        printf("Socket Error\n");
        exit(1);
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
                perror( "connect timeout");
                stat = 1;
            } else {
                stat = 0;
            }
        } else {
            perror ("connect fail");
        }
    }

    if(stat == 0)
    {
        printf("Connect Port %d on %s via %s OK\n", port, addr.c_str (), ifname.c_str ());
    } else {
        printf("Connect Port %d on %s via %s FAILED\n", port, addr.c_str (), ifname.c_str ());
    }

    close(mysocket);
    return stat;
}


#include<unistd.h>
#include<getopt.h>          /*所在头文件 */
int main(int argc, char *argv[])
{
    if(argc<4) {
        printf("usage: %s -a 192.168.15.25 -p 6305 -i eth0 -t 1 \n", argv[0]);
        return 1;
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

    int stat = try_connect(addr, port, ifname, timeoutsec);

    return stat;
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
