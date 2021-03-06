#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "../xensocket.h"

int main(int argc, char **argv) {
    if (argc != 2)
        printf("Usage: %s <service>\n", argv[0]);

    struct sockaddr_xe sxeaddr;
    sxeaddr.sxe_family = AF_XEN;
    strcpy(sxeaddr.service, argv[1]);

    int sock = socket(AF_XEN, SOCK_STREAM, -1);
    if (sock  < 0) {
        errno = ENOTRECOVERABLE;
        perror ("socket");
        exit(EXIT_FAILURE);
    }


    // sender as client
    int rc = connect (sock, (struct sockaddr*) &sxeaddr, sizeof(sxeaddr));
    if (rc < 0) {
        printf("connect failed\n");
        exit(1);
    }
    printf("connected\n");

    /*
    // sender as server
    struct sockaddr_xe remote_sxeaddr;
    bind(sock, (struct sockaddr*) &sxeaddr, sizeof(sxeaddr));
    listen(sock, 5);
    unsigned int addr_len = sizeof(remote_sxeaddr);
    int newsock = accept(sock, (struct sockaddr*) &remote_sxeaddr, &addr_len);
    printf("accept = %d\n", newsock);
    if(newsock < 0) {
    printf("error: %s\n", strerror(errno));
    return 1;
    }
     */

    char input[4096];

    while(1) {
        int counter = 0;
        char c;
        while((c = getchar()) != '\n') {
            input[counter] = c;
            counter++;
        }
        input[counter] = '\0';

        int sent = 0;
        int len = strlen(input);
        while(sent < len && sent >= 0) {
            sent = sent + send(sock, input + sent, len - sent, 0);
            printf("Sent %d bytes\n", sent);
        }
    }
}
