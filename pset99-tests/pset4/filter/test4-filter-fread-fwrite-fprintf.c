#include <stdio.h>
#include <string.h>


int main(void)
{
    FILE *file = NULL;
    char text[] = "I am learning CS and DS.";
    char text2[] = "\nWow.";
    int sz_text = sizeof(text);
    int sz_text2 = sizeof(text2);
    char rtext[strlen(text)];
    char rtext2[strlen(text2)];

    printf("This text will be added to file:\n\n");
    printf("%s", text);
    printf("%s", text2);
    printf("\n\n");

    // open file as write and read
    file = fopen("learning.txt", "w+");

    // write lines on file
    fwrite(text, sz_text - 1, 1, file);
    fwrite(text2, sz_text2 - 1, 1, file);

    // go to start of file
    fseek(file, 0, SEEK_SET);

    // read first line of file
    fread(rtext, sz_text - 1, 1, file);
    printf("This text was added to (and read from) file:\n\n");
    rtext[sz_text - 1] = '\0';
    printf("%s", rtext);

    // read second line of file
    fread(rtext2, sz_text2 - 1, 1, file);
    rtext2[sz_text2 - 1] = '\0';
    printf("%s", rtext2);
    printf("\n");

    // close file
    fclose(file);

    char str[] = "Halo";
    char *str2 = "Hola, como estas?";
    printf("\n__Strings and string literals are different!__");
    printf("\nstring: %s,   size of string: %lu\n", str, sizeof(str));
    printf("\nstring: %s,   size of pointer: %lu\n", str2, sizeof(str2));
}
