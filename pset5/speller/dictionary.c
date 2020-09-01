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
// N = 28 ** n [hash table will take into account first n letters]
// See method hash for more info
const unsigned int N = 28 * 28 * 28 ;

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
unsigned int hash(const char *word)
{
    // Initialize variables
    unsigned int hash_val = 0;
    int n_ini = log(N) / log(28);
    int base = 28;

    // base:
    // \0 a b c d e f g h i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z \'
    //  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27
    // We take into account word's n_ini first letters to calculate hash value.

    // This base system will use more memory bc there are a lot of not permitted
    // hashes eg word "\0\0\0" does not exist but there is space in hash table
    // for it

    // Example:
    // "abcdef" ==> hash_value = int(a) * 28^0 + int(b) * 28^1 + int(c) * 28^2
    // where int(a) = 1, int(b) = 2 and int(c) = 3.
    for (int i = 0; i < n_ini; i++)
    {
        if (isalpha(word[i]))
        {
            hash_val += pow(base, i) * (tolower(word[i]) - 'a' + 1);
        }
        else if (word[i] == '\'')
        {
            hash_val += pow(base, i) * 27;
        }
        else
        {
            break;
        }
    }

    return hash_val;
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
