/* BUILT-IN HEADERS */
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* CUSTOM HEADERS */
#include "user_details.h"

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

/* STRUCTS */
struct termios orig_termios; /* holds the original terminal settings */
typedef struct Terminal
{
    char history[MAX_TOKENS][MAX_TOKEN_LEN];
    void (*addHistory)(struct Terminal *terminal, const char input[]);
    void (*listDirectory)(const char *path);
    void (*clearScreen)();
} Terminal;

/* FUNCTIONS */

/* TERMINAL */
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
        // S_ISDIR -> folosit ca sa ne dam seama daca un director
        // primeste cv de tip mode_t (vezi lab 2)
        if (S_ISDIR(statbuf.st_mode))
        {
            printf(GREEN_UNDERLINE "%s\n" RESET_BCKGROUND, entry->d_name);
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

// folosim pipe
int isPackageAccessible(const char *command)
{
    char check_command[100];
    snprintf(check_command, sizeof(check_command), "command -v %s >/dev/null 2>&1", command);

    FILE *fp = popen(check_command, "r");
    if (fp == NULL)
    {
        return 0;
    }

    int result = pclose(fp);

    // pclose returns -1 in case of an error
    if (result == -1)
    {
        return 0;
    }

    // Check if the command is accessible
    return (result == 0);
}
void handle_tab_press(char *partial_path, char* input) {

    // Add your tab completion logic here
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(".")) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if (strncmp(entry->d_name, partial_path, strlen(partial_path)) == 0) {
                char temp[1000] = "";
                strcpy(temp, entry->d_name + strlen(partial_path));
                strcat(input, temp);
                printf("%s",temp);

            }
        }
        closedir(dir);
    } else {
        perror("opendir");
    }
}

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
    Terminal terminal = {.addHistory = add_to_history, .listDirectory = list_directory, .clearScreen = clear_screen};
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
        // continous input
        while (1)
        {
            int c = getchar();

            // Detect Enter key or end of file
            if (c == '\n' || c == EOF)
            {
                input[inputIndex] = '\0';
                printf("\n");
                if (inputIndex > 0)
                {
                    terminal.addHistory(&terminal, input);
                    historyIndex = countHistory - 1;
                }
                break;
            }
            else if (c == 127)
            {
                // Backspace handling
                if (inputIndex > 0)
                {
                    inputIndex--;
                    input[inputIndex] = '\0';
                    printf("\b \b"); // Move back, print space, move back
                }
            }
            // Detect arrow keys
            else if (c == '\033') // First byte of escape sequence
            {
                getchar();     // Skip '['
                c = getchar(); // Get final byte
                if (c == 'A')  // Up arrow key
                {
                    if (historyIndex >= 0 && historyIndex < countHistory)
                    {
                        strcpy(input, terminal.history[historyIndex]);
                        printf("\r\033[K"); // Clear line
                        printf(BOLD_GREEN "%s@%s:" RESET_COLOUR, user, computer);
                        printf(BOLD_BLUE "%s" RESET_COLOUR, current_cwd);
                        printf(GREEN "$ " RESET_COLOUR "%s", input);
                        inputIndex = strlen(input);

                        if (historyIndex > 0)
                        {
                            historyIndex--;
                        }
                    }
                }
                else if (c == 'B') // Down arrow key
                {
                    if (historyIndex < countHistory - 1)
                    {
                        historyIndex++;
                        strcpy(input, terminal.history[historyIndex]);
                    }
                    else if (historyIndex == countHistory - 1)
                    {
                        historyIndex++;
                        memset(input, 0, sizeof(input));
                    }
                    else
                    {
                        historyIndex = countHistory - 1;
                    }

                    printf("\r\033[K"); // Clear the line
                    printf(BOLD_GREEN "%s@%s:" RESET_COLOUR, user, computer);
                    printf(BOLD_BLUE "%s" RESET_COLOUR, current_cwd);
                    printf(GREEN "$ " RESET_COLOUR "%s", input);
                    inputIndex = strlen(input);
                }
            }
            else if (c == '\t') {
                
                // Call the function to handle Tab press
                char temp[1000];
                strcpy(temp, input);
                if (strlen(temp) > 3) {
                // Use strcpy to copy the substring starting from the third character
                    strcpy(temp, temp + 3);
                    handle_tab_press(temp , input);
                    inputIndex = strlen(input);
                }
            }
            else if (c >= 32 && c < 127)
            {
                // Handle regular characters
                input[inputIndex] = c;
                inputIndex++;

                putchar(c);
            }
            
        }

        // fgets(input, sizeof(input), stdin);
        // input[strcspn(input, "\n")] = 0;
        // terminal.addHistory(&terminal, input);
        // citirea datelor de la tastatura
        
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