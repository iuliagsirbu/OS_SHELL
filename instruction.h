int do_an_instruction(struct Terminal* terminal, char* input){
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];
    int tokenCount = 0;

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
                return -1;
            }
        }
        if (strcmp(tokens[1], ">") == 0)
        {
            terminal->commandRedirection(tokens[0], tokens[2]);
        }

        // verficarea cu fiecare comanda implementata
        else if (strcmp(tokens[0], "exit") == 0)
        {
            return -1;
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
            return 0;
        }
        else if (strcmp(tokens[0], "history") == 0)
        {
            for (int i = 0; i < countHistory; i++)
            {
                printf("%d: %s\n", i, terminal->history[i]); // afisam istoricul comenzilor
            }
            return 0;
        }
        else if (strcmp(tokens[0], "ls") == 0)
        {
            const char *path = tokenCount > 1 ? tokens[1] : "."; // obtinem calea din token-uri; daca nu exista, utilizam directorul curent
            terminal->listDirectory(path);
            return 0;
        }
        else if (strcmp(tokens[0], "clear") == 0)
        {
            terminal->clearScreen();
            return 0;
        }
        else if (strcmp(tokens[0], "cat") == 0)
        {
            if (tokens[1] == NULL)
            {
                printf("Usage: cat <filename>\n");
                return 2; //nu s-a scris bine acea comanda
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
            return 0;
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
            return 0;
        }
        return 1; //nu s-a gasit functia
}