// Checks all functions defined in dictionary.c
#include <stdio.h>
#include "dictionary.c"

int main(void)
{
    // Load dictionary
    char small_dictionary[] = "dictionaries/large";
    load(small_dictionary);

    // Check if word is in dictionary
    char word[] = "cateRPillars";
    char *ck_val = check(word) ? "yes" : "no";

    // print relevant info
    printf("\nNumber of words in dictionary: %d\n",size());
    printf("«%s» in dictionary? %s,\n", word, ck_val);
    printf("one word in dictionary: %s,\n", table[hash("cat")]->word);
    printf(
        "other word in dictionary: %s,\n\n", table[hash("cate")]->next->word
    );

    // Unload dictionary
    unload();
}
