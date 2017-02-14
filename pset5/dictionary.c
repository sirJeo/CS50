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

typedef struct LetterTree {
    bool word;
    struct LetterTree* letter[N]; 
} LetterTree;

unsigned int dictSize = 0;

LetterTree* root;

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    // printf("ceheck word %s\n", word);
    int index;
    int len = strlen(word);
    if (dictSize == 0) {
        return false;
    }
    
    LetterTree* node = root;
    
    for (int i = 0; i < len; i++) {
        if (!isalpha(word[i]) && word[i] != 39) {
            return false;
        }
        index = prepareChar(word[i]);
        if (index > N-1) {
            return false;
        }

        if (node->letter[index] == NULL) {
            return false;
        } 
        node = node->letter[index];
    }

    return !!node->word;
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

    root = (LetterTree*)malloc(sizeof(LetterTree));

    char letter;
    int index;
    LetterTree* node;

    while((letter = getc(file)) != EOF) {
        node = root;

        while(letter != '\n' && letter != EOF) {

            if (!isalpha(letter) && letter != 39) {
                return false;
            }
            index = prepareChar(letter);
            if (index > N-1) {
                return false;
            }

            if (node->letter[index] == NULL) {
                node->letter[index] = (LetterTree*)malloc(sizeof(LetterTree));
            } 

            node = node->letter[index];
            letter = getc(file);
        }
        node->word = true;
        dictSize++;
    }

    fclose(file);
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

void treeFree(LetterTree * node) 
{
    for (int i = 0; i < N; i++) {
        if (node->letter[i] != NULL) {
            treeFree(node->letter[i]);
        }
    }
    free(node);
}

bool unload(void)
{
    treeFree(root);
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