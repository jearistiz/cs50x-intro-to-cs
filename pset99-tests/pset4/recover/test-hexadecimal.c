#include <stdio.h>
#include <stdint.h>

int main(void)
{
    printf("%d\n", 4095 == 0xfff);
    printf("%ld", sizeof(uint8_t));
}