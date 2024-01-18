
int do_an_instruction(struct Terminal* terminal, char * input){
    int tokenCount = 0;
    char *token = strtok(input, " ");
    char tokens[MAX_TOKENS][MAX_TOKEN_LEN];

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
            return -1;
        }
        else if (strcmp(tokens[0], "cd") == 0 )
        {
            if(tokenCount != 2){
                printf("Cd usage : cd <destination>\n");
            }
            else{
                if(chdir(tokens[1]) != 0){printf("Cannot go to: %s\n", tokens[1]);}
            }
            return 0;
        }
        else if (strcmp(tokens[0], "history") == 0)
        {
            for (int i = 0; i < countHistory; i++)
            {
                printf("%d: %s\n", i, terminal->history[i]);
            }
            return 0;
        }
        else if (strcmp(tokens[0], "ls") == 0)
        {
            const char *path = tokenCount > 1 ? tokens[1] : ".";
            terminal->listDirectory(path);
            return 0;
        }
        else if (strcmp(tokens[0], "clear") == 0)
        {
            terminal->clearScreen();
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
                strncat(package_command, tokens[i], sizeof(package_command) - strlen(package_command) - 1);
            }

            // Apelam comanda
            system(package_command);
            return 0;
        }
        return 1;
}
