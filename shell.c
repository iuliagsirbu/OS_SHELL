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
#include "instruction.h"           // header pentru apelarea unei comenzi
#include "logic.h"                 // header pentru operatii logice
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
    Terminal terminal = {
        .addHistory = add_to_history,
        .listDirectory = list_directory,
        .clearScreen = clear_screen,
        .verifyCharacters = verify_each_character,
        .commandRedirection = command_redirection,
        .doAnInstruction = doAnInstruction,
        .printUser = printUser,
        .doWithLogicInstruction = doWithLogicInstruction
    };
    enable_raw_mode();
    while (1)
    {
        terminal.printUser();
        
        int inputIndex = 0;
        memset(input, 0, sizeof(input)); // Clear the buffer

        // continous input that verifies for each character, arrow keys, and tab
        terminal.verifyCharacters(&terminal, input, &inputIndex, &historyIndex);

        //sa aiba && sau || si sa fie cel putin 3 argumente
        if((strstr(input, "&&") ||  strstr(input, "||")) && strchr(input, ' ') && strchr(strchr(input, ' ') + 1, ' ') && strchr(strchr(strchr(input, ' ') + 1, ' ') + 1, ' ')){
           terminal.doWithLogicInstruction(&terminal, input);
        }else{

            int result = terminal.doAnInstruction(&terminal, input);
            if(result == -1){break;}
            if(result == 2){printf("cannot find the command\n");} //return de 2 e pus la sfarsitul functiei, daca nu a intrat pe nici un caz
        }
        
        
    }
    disable_raw_mode();
    return 0;
}