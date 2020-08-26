#include <stdio.h>
#include <cs50.h>
#include <string.h>


// Problem Set 2 - Harvard's CS50x course (2020)
// Author: Juan E. Aristizabal

// This program ciphers text by mapping (substituting)
// letters to given key letters.


int validate_key(string key);

int lowercase_test(char cr);

char *lowercase_key(string key);

char cipher(char l_key[], char character);


int main(int argc, string argv[])
{
    if (argc != 2)
    {
        printf("Error: you should use one key of 26 characters\n");
        return 1;
    }
    else
    {
        string key = argv[1];               // key
        int not_valid = validate_key(key);  // validates key

        if (not_valid)
        {
            return not_valid;
        }
        else
        {
            // user-input plaintext
            string plaintext = get_string("plaintext: ");

            // number of characters of plaintext
            int len = strlen(plaintext);

            // pointer to array mapping each character in key
            // to 1 or 0 wether character is lowercase or uppercase
            char *lower_key = lowercase_key(key);

            // encoding text and printing it via cipher function
            printf("ciphertext: ");

            for (int i = 0; i < len; i++)
            {
                printf("%c", cipher(lower_key, plaintext[i]));
            }
            printf("\n");

            return 0;
        }
    }
}

// Validates key according to guidelines
// Returns 1 if not valid, 0 if valid.
int validate_key(string key)
{
    int len = strlen(key);

    // Check if key is 26 characters long
    if (len != 26)
    {
        printf("Error: your key must contain exactly 26 characters.\n");
        return 1;
    }
    else
    {
        for (int i = 0; i < len; i++)
        {
            // Checks if all characters in key are letters
            if (!(
                    (key[i] > 64 && key[i] < 91)
                    || (key[i] > 96 && key[i] < 123)
                )
               )
            {
                printf("Error: invalid key: at least one ");
                printf("character is not a letter.\n");
                return 1;
            }
            else
            {
                // Checks if key has repeated characters
                for (int j = i + 1; j < len; j++)
                {
                    if (key[i] == key[j]
                        || key[i] == key[j] + 32
                        || key[i] == key[j] - 32)
                    {
                        printf("Error: at least one character repeated.\n");
                        return 1;
                    }
                }
            }
        }
    }

    // If everything is OK returns 0
    return 0;
}

// Returns 1 if lowercase, 0 if uppercase, 2 if not a-z or A-Z
int lowercase_test(char cr)
{
    if (cr > 96 && cr < 123)
    {
        return 1;
    }
    else if (cr > 64 && cr < 91)
    {
        return 0;
    }
    else
    {
        return 2;
    }
}

// Returns character array with key in lowercase
// Assumes key has 26 characters and
// Assumes character is a letter a-z or A-Z
char *lowercase_key(string key)
{
    static char l_key[27];

    for (int i = 0; i < 26; i++)
    {
        if (lowercase_test(key[i]))
        {
            l_key[i] = key[i];
        }
        else
        {
            l_key[i] = key[i] + 32;
        }
    }
    return l_key;
}

// Substitutes each character according to lowercase key l_key
// Mantains uppercase/lowercase of plaintext
char cipher(char l_key[], char character)
{
    int test_lower = lowercase_test(character);
    if (test_lower == 1)
    {
        return l_key[character - 97];
    }
    else if (test_lower == 0)
    {
        return l_key[character - 65] - 32;
    }
    else
    {
        return character;
    }
}
