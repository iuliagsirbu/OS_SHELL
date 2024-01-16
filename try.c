#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("Current working dir: %s\n", cwd);


    struct addrinfo hints, *info, *p;
int gai_result;

char hostname[1024];
hostname[1023] = '\0';
gethostname(hostname, 1023);

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_CANONNAME;

if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
    exit(1);
}

for(p = info; p != NULL; p = p->ai_next) {
    printf("hostname: %s\n", p->ai_canonname);
}

freeaddrinfo(info);
}