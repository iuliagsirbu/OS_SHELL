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
    char *cwd = NULL; // initializeaza pointerul pentru directorul de lucru
    long size = 1024; // dimensiunea bufferului 
    cwd = (char *)malloc(size); // aloca memorie pentru cwd
    getcwd(cwd, 1024);
    return cwd;
}


char *get_computer_name()
{
    struct addrinfo hints, *info, *p; // hints -> contine informatii despre tipul de adresa pe care o cautam, 
                                        // info -> contine adresa IP si numele canonice asociate calculatorului
                                        // p -> pointer la o structura addrinfo
    int gai_result; 

    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = AI_CANONNAME; // returneaza numele canonice asociate adresei IP

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

    // Obtine informatii despre adresa IP si numele canonice asociate calculatorului
    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result)); // afiseaza eroare
        exit(1);
    }
    char *local_name = info->ai_canonname; // extrage numele canonice asociate adresei IP

    // scoate localdomain din el
    // char *substring = ".localdomain";

    // // Find the position of the substring
    // char *position = strstr(local_name, substring);
    // strcpy(position, position + strlen(substring));

    return local_name; // returneaza numele canonice
    freeaddrinfo(info);
}

// The getpwuid() function searches the user database for an entry with a matching uid.
// The geteuid() function returns the effective user ID of the calling process.
char *current_user()
{
    struct passwd *pw; // structura pentru informatii despre utilizator
    char *user = NULL; // initializeaza pointerul pentru numele utilizatorului

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

void printUser(){
    char *current_cwd = get_cwd();
    char *computer = get_computer_name();
    char *user = current_user();
    printf(BOLD_GREEN "%s@%s:" RESET_COLOUR, user, computer);
    printf(BOLD_BLUE "%s" RESET_COLOUR, current_cwd);
    printf(GREEN "$ " RESET_COLOUR);
}