/**
 * Implements a dictionary's functionality.
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "dictionary.h"

const int ALPHABET_SIZE = 'z' - 'a' + 3;

typedef struct _node_trie {
    struct _node_trie **subtrees;  
} node_trie;

// root of trie tree
node_trie *root = NULL;

// words count in trie
int node_trie_count = 0;

node_trie* createTrieNode() {
    node_trie* x = malloc(sizeof(node_trie));
    node_trie** subtrees = malloc(ALPHABET_SIZE * sizeof(node_trie*));
    for(int i = 0; i < ALPHABET_SIZE; i++)
        subtrees[i] = NULL;

    x->subtrees = subtrees;
    return x;
}

bool find_trie(const char* word, int pos, node_trie * tree) {
    if (tree == NULL)
        return false;
    
    char c = tolower(word[pos]);
    //printf("* %s = %c", word, c);
    if (c == '\0') {
        return tree->subtrees[ALPHABET_SIZE - 1] != NULL;
    } else {
        int index = c == '\'' ? ALPHABET_SIZE - 2 : c - 'a';
        return find_trie(word, pos + 1, tree->subtrees[index]);
    }
}

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char *word)
{
    return find_trie(word, 0, root);
}

node_trie* put_trie(const char *word, int pos, node_trie *tree) {
    if (tree == NULL)
        tree = createTrieNode();

    char c = tolower(word[pos]);
    //printf("* %s = %c", word, c);
    if (c == '\0') {
        tree->subtrees[ALPHABET_SIZE - 1] = tree;
    } else {
        int index = c == '\'' ? ALPHABET_SIZE - 2 : c - 'a';
        tree->subtrees[index] = put_trie(word, pos + 1, tree->subtrees[index]);
    }
    return tree;
}

void storeWord(const char *word) {
    root = put_trie(word, 0, root);
    node_trie_count += 1;
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char *dictionary)
{
    FILE *fp = fopen(dictionary, "r");
    if (fp == NULL)
    {
        printf("Could not open %s.\n", dictionary);
        return false;
    }
    
    char word[LENGTH+1];
    int index = 0, words = 0;
    
    // spell-check each word in text
    for (int c = fgetc(fp); c != EOF; c = fgetc(fp))
    {
        // allow only alphabetical characters and apostrophes
        if (isalpha(c) || (c == '\'' && index > 0))
        {
            // append character to word
            word[index] = c;
            index++;

            // ignore alphabetical strings too long to be words
            if (index > LENGTH)
            {
                // consume remainder of alphabetical string
                while ((c = fgetc(fp)) != EOF && isalpha(c));

                // prepare for new word
                index = 0;
            }
        }

        // ignore words with numbers (like MS Word can)
        else if (isdigit(c))
        {
            // consume remainder of alphanumeric string
            while ((c = fgetc(fp)) != EOF && isalnum(c));

            // prepare for new word
            index = 0;
        }

        // we must have found a whole word
        else if (index > 0)
        {
            // terminate current word
            word[index] = '\0';

            // update counter
            words++;

            // store word
            storeWord(word);
            
            // prepare for next word
            index = 0;
        }
    }

    // check whether there was an error
    if (ferror(fp))
    {
        fclose(fp);
        printf("Error reading %s.\n", dictionary);
        return false;
    }

    // close text
    fclose(fp);
    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return node_trie_count;
}

void clean_trie(node_trie *tree) {
    for (int i = 0; i < ALPHABET_SIZE - 1; i++) {
        if (tree->subtrees[i] != NULL)
            clean_trie(tree->subtrees[i]);
    }
    
    free(tree->subtrees);
    free(tree);
}

/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */
bool unload(void)
{
    clean_trie(root);
    return true;
}
