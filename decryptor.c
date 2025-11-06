#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

unsigned char *createPiCollection(int length) {
    const char *fileName = "billion-pi.txt";
    FILE *f = fopen(fileName, "r");
    if (!f) { perror("pi file"); return NULL; }

    unsigned char *key = malloc(length);
    size_t digitsRead = 0;
    int ch;
    while ((ch = fgetc(f)) != EOF && digitsRead < length) {
        if (isdigit(ch)) key[digitsRead++] = ch - '0';
    }
    fclose(f);
    return key;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <hexstring>\n", argv[0]);
        return 1;
    }

    char *hex = argv[1];
    size_t hexLen = strlen(hex);
    size_t textLen = hexLen / 2;

    unsigned char *data = malloc(textLen);

    // convert from hex string to bytes
    for (size_t i = 0; i < textLen; i++) {
        sscanf(hex + 2*i, "%2hhx", &data[i]);
    }

    unsigned char *pi = createPiCollection(textLen);

    printf("Pi collection: ");
    for (size_t i = 0; i < textLen; i++) printf("%d", pi[i]);
    printf("\n");

    // XOR decode
    printf("Decoded text: ");
    for (size_t i = 0; i < textLen; i++) {
        unsigned char decoded = data[i] ^ pi[i];
        putchar(decoded);
    }
    printf("\n");

    free(pi);
    free(data);
    return 0;
}