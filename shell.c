#include <stdio.h>
#include <string.h>

#define MAX_TOKENS 100
#define MAX_TOKEN_LEN 100

#define GREEN "\033[32m"
#define RESET_COLOUR "\033[0m"

static int countHistory = 0;

// void add_to_history(struct Terminal *terminal, const char input[]);

typedef struct Terminal
{
    char history[MAX_TOKENS][MAX_TOKEN_LEN];
    void (*addHistory)(struct Terminal *terminal, const char input[]);
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

int main()
{
    char input[1000];
    Terminal terminal = {.addHistory = add_to_history};

    while (1)
    {
        char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
        int tokenCount = 0;

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
        if (strcmp(tokens[0], "cd") == 0)
        {
            printf("So far so good...\n");
        }

        if (strcmp(tokens[0], "history") == 0)
        {
            for (int i = 0; i < countHistory; i++)
            {
                printf("%d: %s\n", i, terminal.history[i]);
            }
        }
    }
    return 0;
}