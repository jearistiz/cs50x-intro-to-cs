#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Problem Set 3 - Harvard's CS50x course (2020)
// Codebase: CS50 team
// https://cdn.cs50.net/2019/fall/psets/3/tideman/tideman.c
// Modifications: Juan E. Aristizabal

// This program executes a tideman voting as specified by Pset3 guidelines.

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
bool check_lock(int ini, int src, int tar);
void print_winner(void);

////////////////////
void print_pairs(void);
void print_locked(void);
void print_preferences(void);
////////////////////

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
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
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");

        /////////////////////
        //print_preferences();
        /////////////////////
    }

    add_pairs();

    /////////////////////
    //printf("\nUnordered pairs (votes)\n");
    //print_pairs();
    /////////////////////

    sort_pairs();

    /////////////////////
    //printf("\nOrdered pairs (votes)\n");
    //print_pairs();
    /////////////////////

    lock_pairs();

    /////////////////////
    //print_locked();
    /////////////////////

    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (!strcmp(name, candidates[i]))
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count ++;
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count ++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
// JEA: Sorted using bubble sort algorithm
void sort_pairs(void)
{
    int sort_count = 1;
    while (sort_count)
    {
        sort_count = 0;
        for (int i = 0; i < pair_count - 1; i++)
        {
            pair p_i = pairs[i];
            pair p_next = pairs[i + 1];
            int pref_i = preferences[p_i.winner][p_i.loser];
            int pref_next = preferences[p_next.winner][p_next.loser];
            if (pref_i < pref_next)
            {
                pairs[i] = p_next;
                pairs[i + 1] = p_i;
                sort_count++;
            }
        }
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    int win_i;
    int lose_i;
    for (int i = 0; i < pair_count; i++)
    {
        win_i = pairs[i].winner;
        lose_i = pairs[i].loser;
        locked[win_i][lose_i] = check_lock(win_i, win_i, lose_i);
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    // is_candidate_winner
    // registers if there is a value of "true" in the column or row
    // of the adjacency matrix.
    // A winner must be a source i.e. there should be at least one "true"
    // in winner's row values, and all winner's column values should be "false"
    //
    //      i.e winner <-->
    //                           locked[winner_index][i] = ture   (for some i)
    //                      and  locked[i][winner_index] = false  (for all i)
    bool is_candidate_winner[candidate_count][2];
    for (int i = 0; i < candidate_count; i++)
    {
        is_candidate_winner[i][0] = false;
        is_candidate_winner[i][1] = false;
        for (int j = 0; j < candidate_count; j++)
        {
            is_candidate_winner[i][0] = is_candidate_winner[i][0]
                                        || locked[i][j];
            is_candidate_winner[i][1] = is_candidate_winner[i][1]
                                        || locked[j][i];
        }
        if (is_candidate_winner[i][0] == true
            && is_candidate_winner[i][1] == false)
        {
            printf("%s\n", candidates[i]);
        }
    }
    return;
}

/*
In general this function checks if  you can go from "src" to "ini" via "tar"

"src" --> "tar" --> (...) --> "ini"

It checks for cycles when initially called using "ini" = "src"

It assumes the arrow "src" --> "tar" does not yet exist in the adjacency matrix

It returns false if the arrow "src" --> "tar" lets you get from "src" to "ini"
i.e. if lock[src][ini] must be set to false.

It returns true if the arrow "src" --> "tar" does not let you get
from "src" to "ini" i.e. if lock[src][ini] must be set to true.

I created this algorithm by intuitively constructing a flow diagram and
figuring out how to implement it. However, after doing some research, it
turns out _maybe_ this is some sort of "depth-first search" algorithm.

I know this is might not be the best solution but im proud of it.
If you have any idea of how to improve this algorithm
please leave a comment.

There is another cool solution using algebra here:
https://github.com/Federico-abss/CS50-intro-course/blob/master/C/pset3/tideman/tideman.c

There is a good discussion about this approach here:
https://gist.github.com/nicknapoli82/6c5a1706489e70342e9a0a635ae738c9

*/
bool check_lock(int ini, int src, int tar)
{
    // This list stores all the indexes of the targets of the source.
    // maximum of branches for each source is candidate_count.
    int branches[candidate_count];

    // counts number of arrows starting at source
    int branches_count = 0;

    for (int next = 0; next < candidate_count; next++)
    {
        if (locked[tar][next])
        {
            branches[branches_count] = next;
            branches_count++;
        }
    }

    // Tells if adding the arrow src --> tar generates a cycle
    bool count_cicle = false;

    // if there are no branches then return true
    if (!branches_count)
    {
        return true;
    }
    else
    {
        // Run for all branches of target
        for (int branch = 0; branch < branches_count; branch++)
        {

            if (branches[branch] == ini)
            {
                count_cicle = true;
                break;
            }
            // Next line is the clue of success!
            // It checks recursively :)
            else if (check_lock(ini, tar, branches[branch]))
            {
                continue;
            }
            // If there are no cycles in the first place
            //          (if : false <--> no cycle)
            // but there are in the second place
            //          (else if : false <--> cycle)
            // then return false (i.e. there are cycles)
            else
            {
                return false;
            }
        }

        // If there was a cycle (or path reached ini) return false
        // If there was no cycle (or path did not reach ini) return true
        return !count_cicle;
    }
}

// Next functions are only meant to print stuff
void print_preferences(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            printf("%d ", preferences[i][j]);
        }
        printf("\n");
    }
}

void print_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        int votes_w = preferences[pairs[i].winner][pairs[i].loser];
        int votes_l = preferences[pairs[i].loser][pairs[i].winner];
        printf(
            "w: %d (%d)    l: %d (%d) \n",
            pairs[i].winner, votes_w, pairs[i].loser, votes_l
        );
    }
    printf("\n");
}

void print_locked(void)
{
    printf("Final adjacency matrix\n");
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            printf("%d ", locked[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
}
