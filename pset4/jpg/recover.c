/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t  BYTE;

typedef enum
{
    false = ( 1 == 0 ),
    true = ( ! false )
} bool;

int nameId = 0;
BYTE pattern[4][2] = {
    {0xff, 0xff},
    {0xd8, 0xd8},
    {0xff, 0xff},
    {0xe0, 0xe1}
};


bool isNewFileHead(BYTE head[4]);

int main(int argc, char* argv[])
{
    int bufferSize = 512;

    FILE* file = fopen("card.raw", "r");

    if (file == NULL) {
        printf("Could not open file");
        return 1;
    }

    FILE* out = NULL;
    BYTE head[4];
    BYTE block[bufferSize-4];

    bool found = false;
    int patternLevel = 0;
    BYTE symbol;

    while (!found) {
        if (fread(&symbol, sizeof(symbol), 1, file) != 1) {
            printf("Nothing found");
            return 0;
        }

        if (symbol == pattern[patternLevel][0] || symbol == pattern[patternLevel][1]) {
            head[patternLevel++] = symbol;
            if (patternLevel > 3) {
                found = true;
                patternLevel = 0;
            }
        } else {
            patternLevel = 0;
        }
    }
    
    fread(&block, sizeof(block), 1, file);
    
    int readLen;

    do {
        readLen = 0;
        if (isNewFileHead(head)) {
            if (out != NULL) {
                fclose(out);
            }
            char fName[7];
            sprintf(fName, "%03i.jpg", nameId++);

            out = fopen(&fName[0], "w");
            if (out == NULL) {
                printf("Could not save new image file");
                return 2;
            }
        }

        fwrite(&head, sizeof(head), 1, out);
        fwrite(&block, sizeof(block), 1, out);


        readLen += fread(&head, sizeof(head), 1, file);
        readLen += fread(&block, sizeof(block), 1, file);
    } while (readLen == 2);

    if (out != NULL) {
        fclose(out);
    }
    fclose(file);

    return 0;
}

bool isNewFileHead(BYTE head[4]) {
    for (int i = 0; i < 4; i++) {
        if (pattern[i][0] != head[i] && pattern[i][1] != head[i]) {
            return false;
        } 
    }
    return true;
}
