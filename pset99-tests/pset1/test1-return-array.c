#include <stdio.h>


int example(void);
char *hello(char cr);


int main(void)
{
    char *d = hello('c');
    for (int i = 0; i<3; i++)
    {
        printf("%c\n", d[i]);
    }
    printf("%d\n", d[2]);
}

char *hello(char cr)
{
    static char d[2];
    d[0] = 'a';
    d[1] = 'b';
    return d;
}

int example(void)
{
    for (int i = 0; i < 5; i++)
    {
        if (i == 3)
        {
            printf("%d\n", i);
            return 0;
        }
        else
        {
            printf("%d\n", i);
        }
    }
    return 0;
}
