#include <stdio.h>
#include <cs50.h>


// Problem Set 1 - Harvard's CS50x course (2020)
// Author: Juan E. Aristizabal

// Calculates initial checksum of some credit cards
// and states wether it might be VISA, MASTERCARD, AMEX or INVALID.


int main(void)
{
    long number = 0;            // credit card number
    int chksum_2 = 0;           // sum of all 2*(even digit) digits (R to L)
    int chksum_2_1 = 0;         // sum of each 2*(even digit) digits
    int chksum_1 = 0;           // sum of odd digits (right(R) to left(L))

    // User-input credit card number
    do
    {
        number = get_long("Credit card number: ");
    }
    while (!number);

    // Get number of digits of credit card number
    int length = snprintf(NULL, 0, "%ld", number); // number of digits
    char numb_str[length + 1];

    // Save credit card number as string
    snprintf(numb_str, length + 1, "%ld", number);

    // First conditions of invalid card number: negative or less than 13 digits
    if (number < 0 || length < 13)
    {
        printf("INVALID\n");
    }
    else
    {
        // Extract each of the digits by taking modulo operation
        // and then dividing by 10
        while (number)
        {
            // add odd digits (R to L) to checksum
            chksum_1 += number % 10;
            // printf("%ld\n", number % 10); // prints odd digits
            number /= 10;
            if (number)
            {
                chksum_2_1 = 2 * (number % 10); // 2*(even digit)
                // printf("%ld\n", number % 10); // prints even digits
                number /= 10;
                while (chksum_2_1)
                {
                    // add digits of 2*(even digit) to checksum
                    chksum_2 += chksum_2_1 % 10;
                    chksum_2_1 /= 10;
                }
            }
        }

        // printf("1: %d\n", chksum_1); // print checksum odd digits
        // printf("2: %d\n", chksum_2); // print checksum even digits

        // Checksum check
        if ((chksum_1 + chksum_2) % 10 == 0)
        {
            if (length == 15 &&
                (
                    (numb_str[0] == '3' && numb_str[1] == '4')
                    || (numb_str[0] == '3' && numb_str[1] == '7')
                )
               )
            {
                printf("AMEX\n");
            }
            else if (length == 16 &&
                     (
                         (numb_str[0] == '5' && numb_str[1] == '1')
                         || (numb_str[0] == '5' && numb_str[1] == '2')
                         || (numb_str[0] == '5' && numb_str[1] == '3')
                         || (numb_str[0] == '5' && numb_str[1] == '4')
                         || (numb_str[0] == '5' && numb_str[1] == '5')
                     )
                    )
            {
                printf("MASTERCARD\n");
            }
            else if ((length == 13 || length == 16) && numb_str[0] == '4')
            {
                printf("VISA\n");
            }
            else
            {
                printf("INVALID\n");
            }
        }
        else
        {
            printf("INVALID\n");
        }
    }

    return 0;
}