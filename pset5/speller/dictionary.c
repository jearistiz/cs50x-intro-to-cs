// Implements a dictionary's functionality
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <strings.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of buckets in hash table
// N = 2 ** 16 buckets
const unsigned int N = 64 * 64 * 16;

// Hash table
node *table[N];

// Number of words loaded to dictionary
unsigned int dict_size = 0;

// Returns true if word is in dictionary else false
// Easier way to do this: just use strcmp
bool check(const char *word)
{
    // Temporary node used to compare given word and word in hash table.
    node *search_node = table[hash(word)];

    // If table[hash_value] is NULL, then the word is not in the dictionary
    if (!search_node)
    {
        return false;
    }

    // Number of already checked letters from left to right
    int word_len = strlen(word);

    for (; search_node->next != NULL; search_node = search_node->next)
    {
        if (!strcasecmp(search_node->word, word))
        {
            return true;
        }
    }

    // If we reach the last node we must check if it coincides with word
    if (search_node->next == NULL && !strcasecmp(search_node->word, word))
    {
        return true;
    }

    return false;
}

// Hashes word to a number
// djb2 algorithm
unsigned int hash(const char *word)
{
    // djb2 algorithm
    unsigned int hash_val = 5381;

    for (int c = tolower(*(word)), i = 0; c && i < 5; c = tolower(*(word++)), i++)
        hash_val = ((hash_val << 5) + hash_val) + c; // hash * 33 + c

    return hash_val % N;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // Open dictionary file
    FILE *dict_file = fopen(dictionary, "r");
    if (!dict_file)
    {
        printf(
            "load in dictionary.h: could not open dictionary file %s\n",
            dictionary
        );
        return false;
    }

    // Read words in dictionary line by line
    char word_tmp[LENGTH + 1];
    while (fscanf(dict_file, "%s", word_tmp) != EOF)
    {
        // Add word to hash table
        if (!load_word(word_tmp, strlen(word_tmp)))
        {
            return false;
        }

        // Add count to dict_size
        dict_size++;
    }

    // If everything went OK
    fclose(dict_file);
    return true;
}

// Adds word to hash table at beginning of each list
// this is more efficient than adding it to end of list
bool load_word(const char *word, const int word_len)
{
    // Appropiate hash value for word
    unsigned int hash_value = hash(word);

    // Create new node
    node *new_node = malloc(sizeof(node));

    if (!new_node)
    {
        printf("Could not load __ %s __ to hash table\n", word);
        return false;
    }

    // Update new node with word and pointer of previous initial node
    strcpy(new_node->word, word);
    new_node->next = table[hash_value];

    // Update table to point to new first node
    table[hash_value] = new_node;

    return true;
}

// Returns number of words in dictionary if loaded else 0 (if not yet loaded)
unsigned int size(void)
{
    return dict_size;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    node *tmp;
    for (int i = 0; i < N; i++)
    {
        while (table[i])
        {
            tmp = table[i]->next;
            free(table[i]);
            table[i] = tmp;
        }
    }
    return true;
}
