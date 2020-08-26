#include <stdio.h>
#include <cs50.h>


#define candidate_count 6

// locked[i][j] means i is locked in over j
bool locked[candidate_count][candidate_count];

bool check_lock(int ini, int src, int tar);

int main(void)
{

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }
    // Locked in pairs:
    locked[0][1] = true;
    locked[1][2] = true;
    locked[1][3] = true;
    locked[1][4] = true;
    locked[2][3] = true;
    locked[3][4] = true;
    locked[4][5] = true;

    int src = 5;
    int tar = 0;

    printf("%d\n", check_lock(src, src, tar));
}

/*
In general this function checks if  you can go from "src" to "ini" via "tar"
"src" --> "tar" --> (...) --> "ini"
It checks for cycles when initially called using "ini" = "src"
It assumes the arrow "src" --> "tar" does not yet exist.
It returns false if the arrow "src" --> "tar" lets you get from "src" to "ini"

I created this algorithm by intuitively constructing a flow diagram and
figuring out how to implement it. However, after doing some research it
turns out _maybe_ this is some sort of "depth-first search" algorithm.
There is another cool solution using algebra here:
https://github.com/Federico-abss/CS50-intro-course/blob/master/C/pset3/tideman/tideman.c
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
            else if (!check_lock(ini, tar, branches[branch]))
            {
                continue;
            }
            // If there are no cicles in the first place (if statemen)
            // but there are in the second place (else if : false <--> cycle)
            // then return false (i.e. there are cycles)
            else
            {
                return false;
            }
        }
        // If there was a cycle return false (do not add arrow)
        // Else return true
        return !count_cicle;
    }
}
