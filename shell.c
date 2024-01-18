/* BUILT-IN HEADERS */
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

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
#include "user_details.h"          // header pentru detalii despre utilizator
#include "terminal.h"              // header pentru functionalitati specifice terminalului
#include "verify_each_character.h" // header pentru verificarea fiecarui caracter

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

// activam modul raw
void enable_raw_mode()
{
    tcgetattr(STDIN_FILENO, &orig_termios); // obtinem atributele terminalului si la iesirea din program dam disable la raw mode
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;      // copiem atributele terminalului in raw
    raw.c_lflag &= ~(ECHO | ICANON | ISIG); // modificam atributele terminalului
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

    // structura pentru functionalitatile specifice terminalului (istoric, listare directoare, curatare ecran, verificare fiecare caracter)
    Terminal terminal = {.addHistory = add_to_history, .listDirectory = list_directory, .clearScreen = clear_screen, .verifyCharacters = verify_each_character, .commandRedirection = command_redirection};
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
        terminal.verifyCharacters(&terminal, input, &inputIndex, &historyIndex);

        // preprocess the data input
        char *token = strtok(input, " ");

        // impartim datele de intrare in token-uri
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
        if (strcmp(tokens[1], ">") == 0)
        {
            terminal.commandRedirection(tokens[0], tokens[2]);
        }

        // verficarea cu fiecare comanda implementata
        else if (strcmp(tokens[0], "exit") == 0)
        {
            break;
        }
        else if (strcmp(tokens[0], "cd") == 0)
        {
            if (tokenCount != 2)
            {
                printf("Cd usage : cd <destination>\n");
            }
            else
            {
                if (chdir(tokens[1]) != 0) // schimbam directorul de lucru curent
                {
                    printf("Cannot go to: %s\n", tokens[1]);
                }
            }
        }
        else if (strcmp(tokens[0], "history") == 0)
        {
            for (int i = 0; i < countHistory; i++)
            {
                printf("%d: %s\n", i, terminal.history[i]); // afisam istoricul comenzilor
            }
        }
        else if (strcmp(tokens[0], "ls") == 0)
        {
            const char *path = tokenCount > 1 ? tokens[1] : "."; // obtinem calea din token-uri; daca nu exista, utilizam directorul curent
            terminal.listDirectory(path);
        }
        else if (strcmp(tokens[0], "clear") == 0)
        {
            terminal.clearScreen();
        }
        else if (strcmp(tokens[0], "cat") == 0)
        {
            if (tokens[1] == NULL)
            {
                printf("Usage: cat <filename>\n");
                continue;
            }
            char *fromFile = tokens[1];
            int sourceFile = open(fromFile, O_RDONLY);

            struct stat st;
            stat(fromFile, &st);

            int sourceSize = st.st_size;
            char *buffer = malloc(sourceSize);
            int bytesRead = read(sourceFile, buffer, sourceSize);
            write(STDOUT_FILENO, buffer, bytesRead);

            free(buffer);
            close(sourceFile);
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
                strncat(package_command, tokens[i], sizeof(package_command) - strlen(package_command) - 1); // concatenam parametrii
            }

            // Apelam comanda
            system(package_command);
        }
    }
    disable_raw_mode();
    return 0;
}