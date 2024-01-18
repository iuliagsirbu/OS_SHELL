//for AND operator

// If RESULT  then ACTION
// ======================
// If SUCCESS then RUN 2nd command
// If FAIL    then END

//for AND operator

// If RESULT  then ACTION
// ======================
// If SUCCESS then END
// If FAIL    then RUN 2nd command

void suppressedPrintf(const char *format, ...) {
    // Do nothing
}

void doWithLogicInstruction(struct Terminal* terminal, char * input)
{
    //creez un array de inturi in care v[0] daca e 1 este &&, daca e 2 este ||
    //v[0] daca 1 isneamna ca face AND intre 1 si 2

    int logic_gates[1000];
    char copy_for_gates[1000];
    strcpy(copy_for_gates, input);
    int tokenIndex = 0;

    char *token = strtok(copy_for_gates, " ");
    while(token != NULL){
        if(strcmp(token, "&&") == 0){
            logic_gates[tokenIndex] = 1;
            tokenIndex ++;
        }
        if(strcmp(token, "||") == 0){
            logic_gates[tokenIndex] = 2;
            tokenIndex ++;
        }
        token = strtok(NULL, " ");
        
    }
    char copy_input[1000];
    strcpy(copy_input, input);

    char logictokens[MAX_TOKENS][MAX_TOKEN_LEN];

    // Use strtok to split the string based on "&&" and "||"
    token = strtok(copy_input, "&&||");

    tokenIndex = 0;

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

    int old_status;
    //aici folosim o functie care face o instructiune de la 0. 
    //Returneaza 0 daca a decurs bine, returneaza 1 daca nu.
    old_status = doAnInstruction(terminal, logictokens[0]);
    for (int i = 1; i < tokenIndex; i++) {
        
        //verificam ce operatie logica este && sau ||
        if(logic_gates[i-1] == 1 && old_status == 0){
            

            old_status = doAnInstruction(terminal, logictokens[i]);
        }
        if(logic_gates[i-1] == 2 && old_status == 1){
            // Redirect stdout to /dev/null temporarily
            FILE *original_stdout = stdout;  // Save the original stdout
            old_status = doAnInstruction(terminal, logictokens[i]);

        }
    }
}


            