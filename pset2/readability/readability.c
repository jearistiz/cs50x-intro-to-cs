#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <math.h>


// Problem Set 2 - Harvard's CS50x course (2020)
// Author: Juan E. Aristizabal

// This program calculates Coleman-Liau index of user-input text.


int *count(string text);

int main(void)
{
    string text = get_string("Text: "); // user-input text

    // Pointer to array that has number of letters words, and sentences
    int *counts = count(text);

    // Extract info from array counts
    int letters = counts[0];
    int words = counts[1];
    int sentences = counts[2];

    float L = ((float) letters / words) * 100.;    // letters per 100 words
    float S = ((float) sentences / words) * 100.;  // sentences per 100 words
    int grade = round(0.0588 * L - 0.296 * S - 15.8); // Coleman-Liau index

    // Print output as required by pset.
    if (grade < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (grade > 15)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %.d\n", grade);
    }

    // printf("l:%d  w:%d  s:%d  g:%d\n", letters, words, sentences, grade);

    return 0;
}


// This function (count) returns number of letters, words and sentences of a
// given text using the conventions:
// letters == a-z or A-Z,
// word == space separated group of letters
// sentence == group of words separated by "!" "?" or "."
// Returns: pointer of array {letters, words, sentences}
int *count(string text)
{
    int length = strlen(text);
    int letters = 0;
    int words = 1;
    int sentences = 0;

    for (int i = 0; i < length; i++)
    {
        if ((text[i] > 64 && text[i] < 91)
            || (text[i] > 96 && text[i] < 123)
           )
        {
            letters++;
        }
        else if (text[i] == 32)
        {
            words++;
        }
        else if (text[i] == 33 || text[i] == 46 || text[i] == 63)
        {
            sentences++;
        }
    }

    // Meybe there is a more efficient way of doing this:
    static int count_array[3];
    count_array[0] = letters;
    count_array[1] = words;
    count_array[2] = sentences;

    return count_array;
}
