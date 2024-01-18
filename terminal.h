/* STRUCTS */
struct termios orig_termios; /* holds the original terminal settings */

typedef struct Terminal
{
    char history[MAX_TOKENS][MAX_TOKEN_LEN];

    void (*addHistory)(struct Terminal *terminal, const char input[]); // functie care adauga o comanda in istoric
    void (*listDirectory)(const char *path);
    void (*clearScreen)();
    void (*verifyCharacters)(struct Terminal *terminal, char *input, int *inputIndex, int *historyIndex);
    void (*commandRedirection)(char *command, char *file);
    int (*do_an_instruction)(struct Terminal *terminal, char* input);
} Terminal;

/* FUNCTIONS */

/* TERMINAL */

void add_to_history(struct Terminal *terminal, const char input[])
{
    if (countHistory < MAX_TOKENS)
    {
        strncpy(terminal->history[countHistory], input, MAX_TOKEN_LEN - 1); // adaugam comanda in istoric
        terminal->history[countHistory][MAX_TOKEN_LEN - 1] = '\0';          // Ensure null-termination
        countHistory++;
    }
}

// functie care verifica daca inputul este valid
void list_directory(const char *path)
{
    struct dirent *entry; // contine informatii despre un fisier din director
    DIR *dir = opendir(path);

    if (dir == NULL)
    {
        printf("Error: Unable to open directory.\n");
        return;
    }

    while ((entry = readdir(dir)) != NULL) // citim fiecare fisier din director
    {
        struct stat statbuf;
        stat(entry->d_name, &statbuf); // obtinem informatii despre fisier
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
    snprintf(check_command, sizeof(check_command), "command -v %s >/dev/null 2>&1", command); // verificam daca comanda este disponibila in sistem

    FILE *fp = popen(check_command, "r"); // deschidem un pipe pentru a citi outputul comenzii
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

void command_redirection(char *command, char *file)
{
    pid_t pid = fork();
    if (pid == 0) // Child process
    {
        if (file != NULL)
        {
            int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0)
            {
                perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        char *argv[] = {command, NULL};
        execvp(command, argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) // Parent process
    {
        wait(NULL); // Waiting for the child...
    }
    else
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
}