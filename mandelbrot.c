/*
Written by:
    Dhruv Bhagwat
    ens22dbt@cs.umu.se
    SRM Institute of Science and Technology
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libbmp.h"
#include <omp.h>

// This fucntion keeps track of the iteration being made to print out the pixels
int calculate_iterations(float coord_re, float coord_im, int max_iterations, float threshold)
{
    float z_re = 0;
    float z_im = 0;

    int iterations = 0;

    for (iterations = 0; (z_re * z_re + z_im * z_im) < threshold && iterations < max_iterations; iterations++) {
        float temp_z_re = z_re * z_re - z_im * z_im + coord_re;
        z_im = 2 * z_re * z_im + coord_im;
        z_re = temp_z_re;
    }

    return (iterations);
}

//serial mandelbrot function
double mb_serial(unsigned char iterations[], int columns, int rows, int max_iterations, float threshold, float left, float top, float x_side)
{
    //used to get the time at which the program starts running
    double time_post = 0;
    double time_pre = omp_get_wtime();

    float y_side = x_side * (rows / columns);

    float x_scale = x_side / columns;
    float y_scale = y_side / rows;

    for (int x = 0; x < columns; x++) {
        float coord_re = x * x_scale + left;
        for (int y = 0; y < rows; y++) {
            float coord_im = y * y_scale + top;
            iterations[y * columns + x] = calculate_iterations(coord_re, coord_im, max_iterations, threshold);
        }
    }

    // Subtracts the starting time by the ending time which gives us the time elapsed
    time_post = omp_get_wtime();
    return (time_post - time_pre);
}

//parallel for fucntion
double mb_for(unsigned char iterations[], int columns, int rows, int max_iterations, float threshold, float left, float top, float x_side)
{
    double time_post = 0;
    double time_pre = omp_get_wtime();

    float y_side = x_side * (rows / columns);

    float x_scale = x_side / columns;
    float y_scale = y_side / rows;

    //parallelising using parallel for

    #pragma omp parallel for
    for (int x = 0; x < columns; x++) {
        float coord_re = x * x_scale + left;
        for (int y = 0; y < rows; y++) {
            float coord_im = y * y_scale + top;
            iterations[y * columns + x] = calculate_iterations(coord_re, coord_im, max_iterations, threshold);
        }
    }
    // generating elapsed time

    time_post = omp_get_wtime();
    return (time_post - time_pre);
}

//Function which uses the paralell task directive
double mb_task(unsigned char iterations[], int columns, int rows, int max_iterations, float threshold, float left, float top, float x_side)
{
    double time_post = 0;
    double time_pre = omp_get_wtime();

    float y_side = x_side * (rows / columns);

    float x_scale = x_side / columns;
    float y_scale = y_side / rows;

    for (int x = 0; x < columns; x++) {
        float coord_re = x * x_scale + left;
        for (int y = 0; y < rows; y++) {
            float coord_im = y * y_scale + top;
            #pragma omp task
            iterations[y * columns + x] = calc_iterations(coord_re, coord_im, max_iterations, threshold);
        }
    }

    time_post = omp_get_wtime();
    return (time_post - time_pre);
}

// Main function
int main(int argc, char *argv[])
{

    int columns = 1000;
    int rows = 1000;

    int max_iterations = 255;
    float threshold = 32;

    float left = -1.5;
    float top = -1;
    float xside = 2;

    unsigned char serial_iterations[columns * rows];
    unsigned char for_iterations[columns * rows];
    unsigned char task_iterations[columns * rows];

    double time_serial = mb_serial(serial_iterations, columns, rows, max_iterations, threshold, left, top, xside);
    double time_for = mb_for(for_iterations, columns, rows, max_iterations, threshold, left, top, xside);
    double time_task = mb_task(task_iterations, columns, rows, max_iterations, threshold, left, top, xside);

    printf("Time taken by serial program is %f\n", time_serial);
    printf("Time taken by parallel for is: %f\n", time_for);
    printf("Time taken by parallel task is: %f\n", time_task);

    return (0);
}