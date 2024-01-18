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

void verify_each_character(struct Terminal *terminal,char* input,int* inputIndex, int* historyIndex ){
    char *current_cwd = get_cwd();
    char *computer = get_computer_name();
    char *user = current_user();
    while (1){
    
        int c = getchar();

        // Detect Enter key or end of file
        if (c == '\n' || c == EOF)
        {
            input[(*inputIndex)] = '\0';
            printf("\n");
            if ((*inputIndex) > 0)
            {
                terminal->addHistory(terminal, input);
                (*historyIndex) = countHistory - 1;
            }
            break;
        }
        else if (c == 127)
        {
            // Backspace handling
            if ((*inputIndex) > 0)
            {
                
                (*inputIndex)--;
                input[(*inputIndex)] = '\0';
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
                if ((*historyIndex) >= 0 && (*historyIndex) < countHistory)
                {
                    strcpy(input, terminal->history[(*historyIndex)]);
                    printf("\r\033[K"); // Clear line
                    printf(BOLD_GREEN "%s@%s:" RESET_COLOUR, user, computer);
                    printf(BOLD_BLUE "%s" RESET_COLOUR, current_cwd);
                    printf(GREEN "$ " RESET_COLOUR "%s", input);
                    (*inputIndex) = strlen(input);

                    if ((*historyIndex) > 0)
                    {
                        (*historyIndex)--;
                    }
                }
            }
            else if (c == 'B') // Down arrow key
            {
                if ((*historyIndex) < countHistory - 1)
                {
                    (*historyIndex)++;
                    strcpy(input, terminal->history[(*historyIndex)]);
                }
                else if ((*historyIndex) == countHistory - 1)
                {
                    (*historyIndex)++;
                    memset(input, 0, sizeof(input));
                }
                else
                {
                    (*historyIndex) = countHistory - 1;
                }

                printf("\r\033[K"); // Clear the line
                printf(BOLD_GREEN "%s@%s:" RESET_COLOUR, user, computer);
                printf(BOLD_BLUE "%s" RESET_COLOUR, current_cwd);
                printf(GREEN "$ " RESET_COLOUR "%s", input);
                (*inputIndex) = strlen(input);
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
                    (*inputIndex) = strlen(input);
                
                }
        }
        else if (c >= 32 && c < 127)
        {
            // Handle regular characters
            input[(*inputIndex)] = c;
            (*inputIndex)++;

            putchar(c);
        }
    }
}
