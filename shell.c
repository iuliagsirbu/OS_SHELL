#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "user_details.h"

#define MAX_TOKENS 100
#define MAX_TOKEN_LEN 100

#define GREEN_BCKGROUND "\e[42m"
#define BOLD_BLUE "\e[1;34m"
#define BOLD_GREEN "\e[1;32m"
#define GREEN "\033[32m"
#define RESET_COLOUR "\033[0m"
#define RESET_BCKGROUND "\e[0m"

static int countHistory = 0;

typedef struct Terminal
{
    char history[MAX_TOKENS][MAX_TOKEN_LEN];
    void (*addHistory)(struct Terminal *terminal, const char input[]);
    void (*listDirectory)(const char *path);
    void (*clearScreen)();
} Terminal;

void add_to_history(struct Terminal *terminal, const char input[])
{
    if (countHistory < MAX_TOKENS)
    {
        strncpy(terminal->history[countHistory], input, MAX_TOKEN_LEN - 1);
        terminal->history[countHistory][MAX_TOKEN_LEN - 1] = '\0'; // Ensure null-termination
        countHistory++;
    }
}
void list_directory(const char *path)
{
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (dir == NULL)
    {
        printf("Error: Unable to open directory.\n");
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        struct stat statbuf;
        stat(entry->d_name, &statbuf);
        // S_ISDIR -> folosit sa nedam seama daca un director
        // primeste cv de tip mode_t (vezi lab 2)
        if (S_ISDIR(statbuf.st_mode))
        {
            printf(GREEN_BCKGROUND "%s\n" RESET_BCKGROUND, entry->d_name);
        }
        else
        {
            printf(BOLD_BLUE "%s\n" RESET_COLOUR, entry->d_name);
        }
    }
    closedir(dir);
}

void clear_screen()
{
    printf("\033[H\033[J");
}

int main()
{
    char input[1000];
    Terminal terminal = {.addHistory = add_to_history, .listDirectory = list_directory, .clearScreen = clear_screen};

    while (1)
    {
        char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
        int tokenCount = 0;

        char* current_cwd = get_cwd();
        char* computer = get_computer_name();
        char* user = current_user();
        printf(BOLD_GREEN "%s@%s:" RESET_COLOUR, user, computer );
        printf(BOLD_BLUE"%s" RESET_COLOUR, current_cwd);

        printf(GREEN "$ " RESET_COLOUR);

        fgets(input, sizeof(input), stdin);

        input[strcspn(input, "\n")] = 0;

        terminal.addHistory(&terminal, input);

        char *token = strtok(input, " ");
        while (token != NULL)
        {
            strncpy(tokens[tokenCount], token, MAX_TOKEN_LEN);
            tokens[tokenCount][MAX_TOKEN_LEN - 1] = '\0';
            tokenCount++;

            token = strtok(NULL, " ");

            if (tokenCount >= MAX_TOKENS)
            {
                printf("Eroare...\n");
                break;
            }
        }

        if (strcmp(tokens[0], "exit") == 0)
        {
            break;
        }
        else if (strcmp(tokens[0], "cd") == 0)
        {
            printf("So far so good...\n");
        }
        else if (strcmp(tokens[0], "history") == 0)
        {
            for (int i = 0; i < countHistory; i++)
            {
                printf("%d: %s\n", i, terminal.history[i]);
            }
        }
        else if (strcmp(tokens[0], "ls") == 0)
        {
            const char *path = tokenCount > 1 ? tokens[1] : ".";
            terminal.listDirectory(path);
        }
        else if (strcmp(tokens[0], "clear") == 0)
        {
            terminal.clearScreen();
        }
    }
    return 0;
}