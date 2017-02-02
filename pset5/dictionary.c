/****************************************************************************
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Implements a dictionary's functionality.
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <stdbool.h>
#include <string.h>

#include "dictionary.h"

struct Letter {
    bool word;
    struct Letter* next;
};

bool loadWord(char* word);
int prepareChar(char character);

struct Letter list[N] = {{}};
unsigned int dictSize = 0;
unsigned int memsize = sizeof(struct Letter) * N;

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    printf("ceheck word %s\n", word);
    int index;
    int len = strlen(word);
    struct Letter * position = &list[0];

    if (len == 0 || len >= LENGTH) {
        printf("LEN error %i \n\n", len);
        return false;
    }

    for (int i = 0; i < len; i++) {

        if (!isalpha(word[i]) && word[i] != 39) {
            printf("Wrong symbol %c \n\n", word[i]);
            return false;
        }
        index = prepareChar(word[i]);
        if (index > N-1) {
            printf("Wrong index %i: %i, %i  \n\n", i, index, word[i]);
            return false;
        }
        if (!position[index].next) {
            printf("There is no index %i: %i, %i  \n\n", i, index, word[i]);
            return false;
        } 

        if (i < len - 1) {
            position = &position[index].next[0];
        } else {
            printf("Word check %i  \n\n", position[index].word ? 1 : 0);
            return position[index].word;
        }
    }

    return false;
}

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    FILE* file = fopen(dictionary, "r");

    if (file == NULL) {
        return false;
    }

    char buf[LENGTH];
    char* EOL = "\n";
    int i = 0;

    while (fgets(buf, sizeof(buf), file) != NULL && !feof(file)){
        while (strncmp(&buf[i], &EOL[0], 1) != 0) {
            i++;
        }
        buf[i++] = '\0';
        
        char * line = (char *) malloc(sizeof(char) * i);
        strcpy(line, buf);
        if (!loadWord(line)) {
            printf("Wrong word %s \n\n", line);
            // return true;
        }

        i = 0;
    }

    fclose(file);
    return true;
}

bool loadWord(char* word) {
    int index;
    int len = strlen(word);
    struct Letter* position = &list[0];

    if (len == 0 || len >= LENGTH) {
        printf("Wrong Length %i '%s' \n\n", len, word);
        return false;
    }

    for (int i = 0; i < len; i++) {
        // printf("Iteration Position: %i \n\n", position);
        if (!isalpha(word[i]) && word[i] != 39) {
            printf("Wrong symbol %c \n\n", word[i]);
            return false;
        }
        index = prepareChar(word[i]);
        if (index > N-1) {
            printf("Wrong index %i: %i, %i  \n\n", i, index, word[i]);
            return false;
        }
        
        if (!position[index].next) {
            printf("NEW %i\n", index);
            position[index].next = malloc(sizeof(struct Letter) * N);
            memsize += sizeof(struct Letter) * N;
        }

        if (i < len - 1) {
            // printf("\nshould be next %i \n", position[index].next);
            position = &position[index].next[0];
            // printf("check it now %i \n\n", position);
        }
    }

    position[index].word = true;
    dictSize++;
    
    printf("Words %i %s: %i \n\n", dictSize, word, memsize);

    return true;
}


int prepareChar(char character) {
    return character == 39 ? 0 : (character > 96) ? character - 96 : character - 64;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return dictSize;
}

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */

void structFree(struct Letter * letter) 
{
    for (int i = 0; i < N; i++) {
        if (letter[i].next) {
            structFree(&letter[i].next[0]);
        }
    }

    free(letter);
}

bool unload(void)
{
    for (int i = 0; i < N; i++) {
        if (list[i].next) {
            structFree(&list[i].next[0]);
        }
    }
    return true;
}

int main(int argc, char* argv[])
{
    load("./dict.txt");

    if (check("pneumonoultramicroscopicsilicovolcanoconiosis")) {
        printf("\n\nYES\n");
    } else {
        printf("\n\nNO\n");
    }
    unload();
}