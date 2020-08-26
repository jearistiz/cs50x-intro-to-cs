#include <stdio.h>
#include <cs50.h>


int collatz(long n);


int main(void)
{
    long n = get_long("n: ");
    int i = collatz(n);
    printf("collatz(%ld) = %d\n", n, i);
    return 0;
}

int collatz(long n)
{
    if (n == 1)
        return 0;
    else if (n % 2 == 0)
        return 1 + collatz(n / 2);
    else
        return 1 + collatz(3 * n + 1);
}