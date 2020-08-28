#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


// Problem Set 4 - Harvard's CS50x course (2020)
// Author: Juan E. Aristizabal


// This program recovers JPEGs from a forensic image
// taking into account the guidelines of pset 4
// (the file called card.raw)


// Define byte type
typedef uint8_t  BYTE;


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./recover file.raw\n");
        return 1;
    }

    // Open card file
    char *card_file_name = argv[1];
    FILE *card_file = fopen(card_file_name, "r");

    // Check if file opened correctly
    if (card_file == NULL)
    {
        printf("File '%s' could'n be opened correctly.\n", card_file_name);
        return 1;
    }

    // Declare relevant variables:
    int signature_size = 4;
    int FAT_block_size = 512;
    // Counts the number of FAT blocks already visited
    int n_FAT_block = 0;
    // Counts number of recovered JPG files
    int n_jpegs = 0;
    // Allocates memory for name of recovered jpg
    char *name_jpeg = malloc(10 * sizeof(BYTE));
    // Temporarily stores FAT blocks to be saved in jpg file
    BYTE *FAT_block_to_write = malloc(FAT_block_size);
    // Saves first 4 bytes of each FAT block in order to check jpg's signature
    BYTE *check_jpeg = malloc(signature_size * sizeof(BYTE));

    // Update check_jpeg to see if new block begins with jpeg signature
    fread(check_jpeg, sizeof(BYTE), signature_size, card_file);

    while (!feof(card_file))
    {
        // Check if new block begins with jpeg signature
        if (
            check_jpeg[0] == 0xff && check_jpeg[1] == 0xd8
            && check_jpeg[2] == 0xff && check_jpeg[3] > 0xdf
            && check_jpeg[3] < 0xff
        )
        {
            // Create name of new jpeg file and open a new file with its name
            sprintf(name_jpeg, "%03d.jpg", n_jpegs);
            FILE *image_file = fopen(name_jpeg, "w");

            // Restart ceck_jpeg to false condition
            check_jpeg[0] = 0;

            // Save data until new jpeg signature is found
            while (
                !(
                    check_jpeg[0] == 0xff && check_jpeg[1] == 0xd8
                    && check_jpeg[2] == 0xff && check_jpeg[3] > 0xdf
                    && check_jpeg[3] < 0xff
                )
                && !feof(card_file)
            )
            {
                // Seek stream to beginning of FAT block
                fseek(card_file, n_FAT_block * FAT_block_size, SEEK_SET);

                // Read FAT block from card and write it to image file
                fread(
                    FAT_block_to_write, sizeof(BYTE),
                    FAT_block_size, card_file
                );
                fwrite(
                    FAT_block_to_write, sizeof(BYTE),
                    FAT_block_size, image_file
                );

                // Update check_jpeg to see if new block begins with
                // jpeg signature
                fread(check_jpeg, sizeof(BYTE), signature_size, card_file);

                // Set stream to the beginning of next FAT block
                n_FAT_block++;
            }
            // Close image file
            fclose(image_file);

            // Step back the stream so that outer while loop finds next jpeg file
            n_FAT_block--;
            fseek(card_file, n_FAT_block * FAT_block_size + 4, SEEK_SET);

            // Print jpeg succesfully saved!
            printf(
                "Success! (jpeg #%03d) last FAT block number: %d\n",
                n_jpegs, n_FAT_block
            );
            n_jpegs++;
        }

        // If beginning of last FAT block wasn't a signature, or if we just
        // finished saving a jpeg, then set stream to beginning of next
        // FAT block
        fseek(card_file, FAT_block_size - signature_size, SEEK_CUR);

        // Update check_jpeg to see if new block begins with jpeg signature
        fread(check_jpeg, sizeof(BYTE), signature_size, card_file);
        n_FAT_block++;
    }
    free(check_jpeg);
    free(name_jpeg);
    free(FAT_block_to_write);
    fclose(card_file);
    return 0;
}
