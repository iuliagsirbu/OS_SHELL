/* STRUCTS */
struct termios orig_termios; /* holds the original terminal settings */
typedef struct Terminal
{
    char history[MAX_TOKENS][MAX_TOKEN_LEN];

    void (*addHistory)(struct Terminal *terminal, const char input[]);
    void (*listDirectory)(const char *path);
    void (*clearScreen)();
    void (*verifyCharacters)(struct Terminal *terminal,char* input,int* inputIndex, int* historyIndex );
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

