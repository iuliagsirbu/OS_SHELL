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

int containsLogicalAND(const char *str) {
    // Search for "&&" in the string
    const char *result = strstr(str, "&&");

    // If result is not NULL, "&&" is present in the string
    if (result != NULL) {
        return 1; // True
    } else {
        return 0; // False
    }
}

int containsLogicalOR(const char *str) {
    // Search for "||" in the string
    const char *result = strstr(str, "||");

    // If result is not NULL, "||" is present in the string
    if (result != NULL) {
        return 1; // True
    } else {
        return 0; // False
    }
}