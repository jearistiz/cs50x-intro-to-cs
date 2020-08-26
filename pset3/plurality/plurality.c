#include <cs50.h>
#include <stdio.h>
#include <string.h>


// Problem Set 2 - Harvard's CS50x course (2020)
// Codebase: CS50 team
// https://cdn.cs50.net/2019/fall/psets/3/plurality/plurality.c
// Modifications: Juan E. Aristizabal

// This program executes a plurality voting as specified by Pset3 guidelines.


// Max number of candidates
#define MAX 9

// Candidates have name and vote count
typedef struct
{
    string name;
    int votes;
}
candidate;

// Array of candidates
candidate candidates[MAX];

// Number of candidates
int candidate_count;

// Function prototypes
bool vote(string name);
void print_winner(void);
void sort_votes(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: plurality [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i].name = argv[i + 1];
        candidates[i].votes = 0;
    }

    int voter_count = get_int("Number of voters: ");

    // Loop over all voters
    for (int i = 0; i < voter_count; i++)
    {
        string name = get_string("Vote: ");

        // Check for invalid vote
        if (!vote(name))
        {
            printf("Invalid vote.\n");
        }
    }

    // Display winner of election
    print_winner();
}


// Update vote totals given a new vote
bool vote(string name)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(name, candidates[i].name) == 0)
        {
            candidates[i].votes++;
            return true;
        }
    }
    return false;
}

// Print the winner (or winners) of the election
void print_winner(void)
{
    // Sort votes max to min
    sort_votes();

    // Print winners
    for (int i = 0; i < candidate_count; i++)
    {
        if (candidates[i].votes == candidates[0].votes)
        {
            printf("%s\n", candidates[i].name);
        }
    }
    return;
}

// Sort votes from max to min using bubble sort algorithm
void sort_votes(void)
{
    int sort_count = 1;
    while (sort_count)
    {
        sort_count = 0;
        for (int i = 0; i < candidate_count - 1; i++)
        {
            candidate v_1 = candidates[i];
            candidate v_2 = candidates[i + 1];
            if (v_1.votes < v_2.votes)
            {
                candidates[i] = v_2;
                candidates[i + 1] = v_1;
                sort_count++;
            }
        }
    }
}
