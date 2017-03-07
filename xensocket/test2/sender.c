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
  if (argc != 3)
    printf("Usage: %s <peer-domid> <grant-ref>\n", argv[0]);

  struct sockaddr_xe sxeaddr;
  sxeaddr.sxe_family = AF_XEN;
  sxeaddr.remote_domid = atoi(argv[1]);
  sxeaddr.shared_page_gref = atoi(argv[2]);

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

  char input[4096];

  while(1) {
    scanf("%s", input);   
    printf("%s\n", input);
    
    int sent = 0;
    int len = strlen(input);
    while(sent < len) {
      sent = sent + send(sock, input + sent, len - sent, 0);
    }
  }
}