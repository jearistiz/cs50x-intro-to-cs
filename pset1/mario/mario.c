#include <stdio.h>
#include <cs50.h>


// Problem Set 1 - Harvard's CS50x course (2020)
// Author: Juan E. Aristizabal

// This script prints double "mario" triangular blocks of the form

//     #  #
//    ##  ##
//   ###  ###
//  ####  ####
// #####  #####

// Height chosen as user-input parameter 0 < h < 9

// Note cs50 module is needed.


int main(void)
{
    int h = 0;

    do
    {
        h = get_int("Height: ");
    }
    while (h < 1 || h > 8);

    for (int i = 0; i < h; i++) // lines loop
    {
        for (int j = 0; j < 2 * (h + 1); j++) // columns loop
        {
            if (j < h - (i + 1)) // prints first blank spaces
            {
                printf(" ");
            }
            else if (j >= h - (i + 1) && j < h) // prints first hashes
            {
                printf("#");
            }
            else if (j >= h && j < h + 2) // prints two spaces in between blocks
            {
                printf(" ");
            }
            else if (j >= h + 2 && j < h + i + 3) // prints last hashes
            {
                printf("#");
            }
        }
        printf("\n");
    }
}