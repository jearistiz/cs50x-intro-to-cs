#include <math.h>
#include <stdlib.h>
#include "helpers.h"


// Problem Set 4 - Harvard's CS50x course (2020)
// Codebase: CS50 team
// https://cdn.cs50.net/2019/fall/psets/4/filter/more/filter.zip
// Modifications: Juan E. Aristizabal


// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float avg = (image[i][j].rgbtBlue + image[i][j].rgbtGreen
                         + image[i][j].rgbtRed) / 3.0;
            image[i][j].rgbtBlue = (BYTE) round(avg);
            image[i][j].rgbtGreen = (BYTE) round(avg);
            image[i][j].rgbtRed = (BYTE) round(avg);
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE *line = malloc(sizeof(image[0]));
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            line[j] = image[i][j];
        }
        for (int j = 0; j < width; j++)
        {
            image[i][width - (j + 1)] = line[j];
        }
    }
    free(line);
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Allocate memory to copy image
    RGBTRIPLE(*image_copy)[width] = calloc(height, width * sizeof(RGBTRIPLE));

    // Make copy of image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image_copy[i][j] = image[i][j];
        }
    }

    // Declare averages over red green and blue, & number of pixels to average
    float avgG, avgB, avgR;
    int n_avg;

    // Blur image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            avgG = 0.;
            avgB = 0.;
            avgR = 0.;
            n_avg = 0;

            // To blur the image we have to check all 9 possible pixel
            // positions by iterating from -1 to 1 twice
            for (int ip = -1; ip < 2; ip++)
            {
                for (int jp = -1; jp < 2; jp++)
                {
                    int in = i + ip;
                    int jn = j + jp;

                    // Avoid out of range indexes
                    if (in > -1 && jn > -1 && in < height && jn < width)
                    {
                        avgB += image_copy[in][jn].rgbtBlue;
                        avgG += image_copy[in][jn].rgbtGreen;
                        avgR += image_copy[in][jn].rgbtRed;
                        n_avg++;
                    }
                }
            }
            avgB /= n_avg;
            avgG /= n_avg;
            avgR /= n_avg;

            // Finally for each pixel blur each color
            image[i][j].rgbtBlue = (BYTE) round(avgB);
            image[i][j].rgbtGreen = (BYTE) round(avgG);
            image[i][j].rgbtRed = (BYTE) round(avgR);
        }
    }
    free(image_copy);
    return;
}


#define rgb_n  3
// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // Allocate memory to copy image
    RGBTRIPLE(*image_copy)[width] = calloc(height, width * sizeof(RGBTRIPLE));

    // Make copy of image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image_copy[i][j] = image[i][j];
        }
    }

    // Sobel Kernels
    int Gx[rgb_n][rgb_n] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[rgb_n][rgb_n] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    // Initialize Sobel weighted sums and Sobel final value
    int gx[rgb_n]; // gx[0] <-> B, gx[1] <-> G, gx[2] <-> R
    int gy[rgb_n]; // gy[0] <-> B, gy[1] <-> G, gy[2] <-> R
    float g[rgb_n]; // g[0] <-> B, g[1] <-> G, g[2] <-> R

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // For each pixel initialize weighted sums to 0
            for (int k = 0; k < rgb_n; k++)
            {
                gx[k] = 0;
                gy[k] = 0;
            }

            // To apply Sobel filter we have to check all 9 possible pixel
            // positions of weighted sum by iterating from 0 to 2 twice (size
            // of kernels is 3x3)
            for (int ip = 0; ip < rgb_n; ip++)
            {
                for (int jp = 0; jp < rgb_n; jp++)
                {
                    // Positions of pixels contributing to weighted sum of
                    // current pixel (i,j)
                    // Row top, center or bottom depending on value of ip
                    int in = i + (ip - 1);

                    // Column left, center or right depending on value of jp
                    int jn = j + (jp - 1);

                    // If indexes are inside range add values of weighted sum
                    if (in > -1 && jn > -1 && in < height && jn < width)
                    {
                        gx[0] += Gx[ip][jp] * (int)image_copy[in][jn].rgbtBlue;
                        gx[1] += Gx[ip][jp] * (int)image_copy[in][jn].rgbtGreen;
                        gx[2] += Gx[ip][jp] * (int)image_copy[in][jn].rgbtRed;
                        gy[0] += Gy[ip][jp] * (int)image_copy[in][jn].rgbtBlue;
                        gy[1] += Gy[ip][jp] * (int)image_copy[in][jn].rgbtGreen;
                        gy[2] += Gy[ip][jp] * (int)image_copy[in][jn].rgbtRed;
                    }
                    else
                    {
                        continue;
                    }
                }
            }

            // Calculate Sober filter final value for each color
            for (int n = 0; n < rgb_n; n ++)
            {
                g[n] = round(sqrt(pow(gx[n], 2) + pow(gy[n], 2)));
                if (g[n] > 255)
                {
                    g[n] = 255;
                }
            }

            // Finally for each pixel assign filtered value
            image[i][j].rgbtBlue = (BYTE) g[0];
            image[i][j].rgbtGreen = (BYTE) g[1];
            image[i][j].rgbtRed = (BYTE) g[2];
        }
    }
    free(image_copy);
    return;
}
