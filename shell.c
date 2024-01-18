/* BUILT-IN HEADERS */
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>



/* CONSTANTS */
#define MAX_TOKENS 100
#define MAX_TOKEN_LEN 100

/* ANSI COLOR CODES */
#define GREEN_BCKGROUND "\e[42m"
#define BOLD_BLUE "\e[1;34m"
#define BOLD_GREEN "\e[1;32m"
#define GREEN "\033[32m"
#define RESET_COLOUR "\033[0m"
#define RESET_BCKGROUND "\e[0m"
#define GREEN_UNDERLINE "\e[4;32m"

static int countHistory = 0;

/* CUSTOM HEADERS */
#include "user_details.h"
#include "terminal.h"
#include "verify_each_character.h"
#include "logic.h"
/* TERMIOS */
void disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    /*
    set the terminal attributes:
        - file descriptor for standard input
        - discards any unread input before applying the new settings
        - pointer to the original terminal settings
    */
}

void enable_raw_mode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    /*
    raw struct modifies certain flags:
        - disables echoing of input characters
        - enables non-canonical mode (input is processed immediately)
        - disables signal generation
    */
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); /* enables raw mode */
}

int main()
{
    char input[1000];
    int historyIndex = countHistory - 1;
    Terminal terminal = {.addHistory = add_to_history, .listDirectory = list_directory, .clearScreen = clear_screen, .verifyCharacters = verify_each_character};
    enable_raw_mode();
    while (1)
    {
        char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
        int tokenCount = 0;

        char *current_cwd = get_cwd();
        char *computer = get_computer_name();
        char *user = current_user();
        printf(BOLD_GREEN "%s@%s:" RESET_COLOUR, user, computer);
        printf(BOLD_BLUE "%s" RESET_COLOUR, current_cwd);

        printf(GREEN "$ " RESET_COLOUR);

        int inputIndex = 0;
        memset(input, 0, sizeof(input)); // Clear the buffer

        // continous input that verifies for each character, arrow keys, and tab
        terminal.verifyCharacters(&terminal,input, &inputIndex, &historyIndex);
        
        if(containsLogicalAND(input) || containsLogicalOR(input)){
            //creez un array de inturi in care v[0] daca e 1 este &&, daca e 0 este ||
            //v[0] daca 1 isneamna ca face AND intre 1 si 2
            int logic_gates[1000];
            char copy_input[1000];
            strcpy(copy_input, input);

            char logictokens[MAX_TOKENS][MAX_TOKEN_LEN];

            // Use strtok to split the string based on "&&" and "||"
            char *token = strtok(copy_input, "&&||");

            int tokenIndex = 0;

            // Iterate through the tokens
            while (token != NULL && tokenIndex < MAX_TOKENS) {
                // Copy the token to the array
                if(token[0] == ' '){
                    strncpy(logictokens[tokenIndex], token + 1, MAX_TOKEN_LEN - 1);
                }
                else {
                    strncpy(logictokens[tokenIndex], token, MAX_TOKEN_LEN - 1);
                }
                logictokens[tokenIndex][MAX_TOKEN_LEN - 1] = '\0';

                // Increment the index and print the token
                tokenIndex++;

                // Get the next token
                token = strtok(NULL, "&&||");
            }
            for (int i = 0; i < tokenIndex; i++) {
                printf("%s\n", logictokens[i]);
            }
        }
        //preprocess the data input
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

        // verficarea cu fiecare comanda implementata
        if (strcmp(tokens[0], "exit") == 0)
        {
            break;
        }
        else if (strcmp(tokens[0], "cd") == 0 )
        {
            if(tokenCount != 2){
                printf("Cd usage : cd <destination>\n");
            }
            else{
                if(chdir(tokens[1]) != 0){printf("Cannot go to: %s\n", tokens[1]);}
            }
            
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
        // asta trb pusa la sfarsit doarece poate accesa si touch, mkdir, cam orice comanda linux
        // deci asta este doar daca am ratat noi alte comenzi, altfel merge tot
        else if (isPackageAccessible(tokens[0]))
        { // verific daca este o functie dintr-un package pe care il are userul, daca da apelez acea functie
            char package_command[512];
            snprintf(package_command, sizeof(package_command), "command -v %s >/dev/null 2>&1", tokens[0]);
            // Build the package_command with parameters
            snprintf(package_command, sizeof(package_command), "%s", tokens[0]);

            // Concatenate Git package_command parameters
            for (int i = 1; i < tokenCount; ++i)
            {
                strncat(package_command, " ", sizeof(package_command) - strlen(package_command) - 1);
                strncat(package_command, tokens[i], sizeof(package_command) - strlen(package_command) - 1);
            }

            // Apelam comanda
            system(package_command);
        }
    }
    disable_raw_mode();
    return 0;
}