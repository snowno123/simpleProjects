#include <sys/socket.h> //network api functions
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h> //for struct timeval

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *csvServiceDetect(int port);
char *grabBanner(int sfd, int port);

int main(int argc, char **argv)
{
    if(argc != 3){printf("Usage: ./portscan IPv4 port"); return -1;};
    char *ip = argv[1];
    int port = atoi(argv[2]);
    if(port > 1023 || port < 1) {printf("Port must be between 1-1023"); return -1;};
    //checks for invalid ip done by inet_aton

    struct sockaddr_in remote; //default structure for a socket address for internet connect
    int sfd = socket(AF_INET, SOCK_STREAM, 0);//socket file descriptor = socket(AF_INET = IPv4 internet protocol, SOCK_STREAM = TCP connect, 0 = default protocol)
    if(sfd < 0) //on error, socket() returns -1
    {
        perror("socket() error");
        return -1;
    }

    memset(&remote, 0, sizeof(remote));//fills out structure as zero, prevent leftover trash

    //sin_family = socket family group (IPv4, IPv6, etc), sin_port = socket internet port
    remote.sin_family = AF_INET; //set to internet connection
    remote.sin_port = htons(port); //htons: convert from host byte order to network byte order (big vs little endian issue)


    if(inet_aton(ip, &remote.sin_addr) == 0) //converts from IPv4 to binary form(network byte order) and stores in second argument (struct); returns 0 if IP invalid
    {
        //inet_aton returns 0 if invalid ip
        perror("not a valid ip");
        return -1;
    }

    printf("Scanning %s, port %i... \n", ip, port);

    //1. Connect to port :: connect(descriptor, address struct, length of address)
    int ret = connect(sfd, (struct sockaddr *) &remote, sizeof(struct sockaddr_in));
    if(ret < 0) //if closed, connect() returns -1, else continue
    {
        printf("Port %i closed \n", port);
        return 0; 
    }
    printf("Port %i open:\n", port);

    //2. Grab Banner :: check if we recieved a banner using recv
    char *banner = grabBanner(sfd, port);
    if(banner != NULL) //if a banner was returned
    {
        printf("Raw banner from port %i:\n%s\n", port, banner); //print raw banner
    }
    else
    {
        printf("No banner returned\n");
    }

    //3. Check service assignment :: iterate through ports.csv file and check for an assigned service
    const char *service = csvServiceDetect(port);
    printf("Port %i assigned service: %s\n", port, service);
    return 0;
}

const char *csvServiceDetect( int port) 
{
    static char service[32]; //string to store service name, 32 bytes long
    FILE *file = fopen("ports.csv", "r"); //open file in read mode
    if(!file)
    {
        printf("no ports.csv file");
        return "unknown";
    }

    char line[128]; //string to store line, 128 bytes long

    while(fgets(line, sizeof(line), file))
    {
        char *portStr = strtok(line, ",");
        char *fileService = strtok(NULL, "\n");
        int filePort = atoi(portStr);
        if(filePort == port)
        {
            strcpy(service, fileService);
            fclose(file);
            return service;
        }
    }
    fclose(file);
    return "unknown";
}

char *grabBanner(int sfd, int port)
{
    //add timeout to recv funtion
    struct timeval timeout;
    timeout.tv_sec = 5; timeout.tv_usec = 0; //set to 5 seconds, 0 microsecond
    setsockopt(
        sfd, //socket file descriptor
        SOL_SOCKET, //option for socket layer
        SO_RCVTIMEO, //specify recieve timeout
        &timeout, //specify timeout input from above
        sizeof(timeout)
    );

    //buffer for banner
    static char buffer[1024]; 
    int bannerMax = sizeof(buffer);

    //grab banner
    ssize_t dataLength = recv(sfd, buffer, bannerMax-1, 0); //ssize_t is a special integer that can return negative numbers to indicate errors

    //if no data or error recieved: 0: no data recieved or <0: error 
    if(dataLength <= 0) 
    {
        printf("No Banner received\n");
        return NULL;
    }

    //make string and return
    buffer[dataLength] = '\0'; 
    return buffer;
}