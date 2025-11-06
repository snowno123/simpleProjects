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
    printf("Pi collection: ");
    for (size_t i = 0; i < digitsRead; i++) {
        printf("%d", key[i]); // print as decimal digit
    }
    printf("\n");
    fclose(f);
    return key;
}

int main (int argc, char *argv[]){
    
    //Check if enough arguments
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    unsigned char *text = (unsigned char *)argv[1];

    size_t textLen = strlen((char*)text);
    // allocate space to hold as many digits as the text length
    unsigned char *data = malloc(textLen);
    pi = createPiCollection(textLen);

    for (size_t i = 0; i < textLen; i++) {
        unsigned char result = text[i] ^ pi[i];
        printf("%02x", result);
    }
    printf("\n");
    return 0;
}