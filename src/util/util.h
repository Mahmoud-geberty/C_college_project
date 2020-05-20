#ifndef util_Util_h
#define util_Util_h

#include <stdio.h>
#include <stdlib.h>


void read_samples(FILE *, double data[][11], int x_dim, int y_dim);

int is_sorted(double data[][11], int x_dim, int y_dim);

void bubble_sort(double src[][11], int x_dim, int y_dim);

void split(double src[], double* dest_part1, double *dest_part2, int start_idx, int end_idx);
double median(double arr[], int size);

double variance(double arr[], int start_idx, int end_idx);
double iqr( double arr[], int start_idx, int end_idx);

int remove_outliers(double data[], int size, double variance, double iqr);

double average(double data[], int start_idx, int end_idx);

#endif