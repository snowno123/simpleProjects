#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

unsigned char *createPiCollection (int length) {
    const char *fileName = "billion-pi.txt";
    FILE *f = fopen(fileName, "r");

    size_t digitsRead = 0;  
    unsigned char *key = malloc(length);
    int ch;
    length += 1;
    // read character by character until we have enough or hit EOF
    while ((ch = fgetc(f)) != EOF && digitsRead < length) {

        //Add character to collection as int, increment digitsRead
        key[digitsRead++] = ch - '0';
    }

    fclose(f);
    printf("Pi collection: ");
    for (size_t i = 0; i < digitsRead; i++) {
        printf("%d", key[i]); // print as decimal digit
    }
    printf("\n");
    return key;
}

int main (int argc, char *argv[]){
    
    //Check if enough arguments
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <hex_string>\n", argv[0]);
        return 1;
    }

    char *hex = argv[1];
    size_t hexLen = strlen(hex)/2;
    // allocate space to hold as many digits as the hex length
    unsigned char *pi = malloc(hexLen);
    pi = createPiCollection(hexLen);

    // Convert hex string to bytes
    for (size_t i = 0; i <hexLen; i++)
        sscanf(hex + 2*1, "%2hhx", &pi[i]);

    for (size_t i = 0; i < hexLen; i++) {
        unsigned char result = hex[i] ^ pi[i];
        putchar(result);
    }
    printf("\n");
    return 0;
}