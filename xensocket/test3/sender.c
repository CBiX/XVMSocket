#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <time.h>

#include "../xensocket.h"

int main(int argc, char **argv) {
    if (argc != 3)
        printf("Usage: %s <service>\n", argv[0]);

    struct sockaddr_xe sxeaddr, remote_sxeaddr;
    sxeaddr.sxe_family = AF_XEN;
    strcpy(sxeaddr.service, argv[1]);

    int sock = socket(AF_XEN, SOCK_STREAM, -1);
    if (sock  < 0) {
        errno = ENOTRECOVERABLE;
        perror ("socket");
        exit(EXIT_FAILURE);
    }

    int rc = connect (sock, (struct sockaddr*) &sxeaddr, sizeof(sxeaddr));
    if (rc < 0) {
    	printf("connect failed\n");
    	exit(1);
    }

/*
    bind(sock, (struct sockaddr*) &sxeaddr, sizeof(sxeaddr));
    listen(sock, 5);
    unsigned int addr_len = sizeof(remote_sxeaddr);
    int newsock = accept(sock, (struct sockaddr*) &remote_sxeaddr, &addr_len);
*/
    printf("connected\n");

    char input[32];
    char *ptr;

    while(1) {
        int counter = 0;
        char c;
        while((c = getchar()) != '\n') {
            input[counter] = c;
            counter++;
        }
        input[counter] = '\0';
	// convert input to string
	int count = strtol(input, &ptr, 10);
	printf("sending %d bytes\n", count);

	char *buffer = malloc(count + 2);

	// generate data to be sent
	for(int i = 0; i < count; i++) {
		c = 65 + (i % 26);
		*(buffer + i) = c;
	}
	buffer[count] = '\n';
	buffer[count + 1] = '\0';

	// send data
	int sent = 0;
	int start = clock();
	while(sent < count + 1) {
		sent = sent + send(sock, buffer + sent, count - sent + 1, 0);
	}
	int end = clock();
	printf("bytes sent: %d millis\n", (end - start));

	free(buffer);
    }
}
