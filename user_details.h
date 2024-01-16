#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <pwd.h>
#include <stddef.h>

char *get_cwd()
{
    char *cwd = NULL;
    long size = 1024;
    cwd = (char *)malloc(size);
    getcwd(cwd, 1024);
    return cwd;
}

char *get_computer_name()
{
    struct addrinfo hints, *info, *p;
    int gai_result;

    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    // int getaddrinfo(const char *node,
    //                 const char *service,
    //                 const struct addrinfo *hints,
    //                 struct addrinfo **res);

    // struct addrinfo {
    //     int     ai_flags;
    //     int     ai_family;
    //     int     ai_socktype;
    //     int     ai_protocol;
    //     size_t  ai_addrlen;
    //     struct  sockaddr *ai_addr;
    //     char    *ai_canonname;     /* canonical name */
    //     struct  addrinfo *ai_next; /* this struct can form a linked list */
    // };

    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
        exit(1);
    }
    char *local_name = info->ai_canonname;

    // scoate localdomain din el
    // char *substring = ".localdomain";

    // // Find the position of the substring
    // char *position = strstr(local_name, substring);
    // strcpy(position, position + strlen(substring));

    return local_name;
    freeaddrinfo(info);
}

// The getpwuid() function searches the user database for an entry with a matching uid.
// The geteuid() function returns the effective user ID of the calling process.
char *current_user()
{
    struct passwd *pw;
    char *user = NULL;

    pw = getpwuid(geteuid());
    if (pw)
        user = pw->pw_name;
    else if ((user = getenv("USER")) == NULL)
    {
        fprintf(stderr, "I don't know!\n");
        return "nothing";
    }
    return user;
}