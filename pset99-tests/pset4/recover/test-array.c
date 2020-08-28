#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void)
{
    uint8_t *check_jpeg = malloc(4 * sizeof(uint8_t));
    *check_jpeg = 3;
    printf("%d\n", *check_jpeg);
}