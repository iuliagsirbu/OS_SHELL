#include <stdio.h>
#include <string.h>

int main()
{
    char input[1000];
    fgets(input, sizeof(input), stdin);
    char *token = strtok(input, " ");
    while (token != NULL)
    {
        printf("%s\n", token);
        token = strtok(NULL, " ");
    }
    return 0;
}