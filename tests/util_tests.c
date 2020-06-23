#include "minunit.h"
#include <util/util.h>
#include <assert.h>

#define PARAMS 10
#define SAMPLES 11

// double data[PARAMS][SAMPLES] = {
//     {7.8, 7.5, 7, 7, 8, 2, 7, 4, 13, 6, 7},
//     {96, 100, 20, 10, 44, 102, 113, 90, 88, 108, 95},
//     {45.5, 50, 43.8, 51, 44, 43, 58, 70, 55, 46, 42},
//     {29.5, 30, 20, 29, 28.5, 40, 25, 24, 27, 27.5, 30},
//     {10, 9, 10.5, 10, 30, 7, 9, 9.5, 7, 8, 7.5},
//     {3, 3.2, 3, 2.8, 2.9, 7, 3, 3.2, 1, 1.5, 3.5},
//     {1.8, 1, 2, 2.1, 1.9, 2, 10, 1.7, 1.5, 2, 1.4},
//     {8.3, 20, 10, 9, 10.2, 7.9, 8.2, 8, 9, 8.1, 9.2},
//     {3.0, 2.8, 2.9, 3, 7, 2.7, 2.6, 3, 3.1, 3.3, 3.8},
//     {1.2, 1.2, 1.1, 1, 1.2, 1.3, 4, 1.1, 1, 1.3, 1.4}};

double data[PARAMS][SAMPLES];

char *test_read_samples()
{

    FILE *stream = fopen("river1.dat", "r");
    check(stream, "failed to open file river1.dat");
    read_samples(stream, data, PARAMS, SAMPLES);
    mu_assert(data[0][0] == 7.8, "file read failed, expected different value at postiion");
    mu_assert(data[0][10] == 7, "file read failed, expected different value at postiion");
    mu_assert(data[1][0] == 96, "file read failed, expected different value at postiion");
    mu_assert(data[3][4] == 28.5, "file read failed, expected different value at position");
    mu_assert(data[9][10] == 1.4, "file read failed, expected different value at postiion");

    fclose(stream);
    return NULL;
error:
    fclose(stream);
    return NULL;
}

char *test_is_sorted()
{
    double arr[2][11] = {
        {3.2, 3.3, 3.5, 4, 4.2, 4.5, 4.9, 5, 6, 7, 9},
        {2, 3, 4, 5, 6, 7, 7.5, 8, 9, 10, 11}};

    double bad[2][11] = {
        {4, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

    mu_assert(is_sorted(arr, 2, 11), "is_sorted failed, should be true");
    mu_assert(is_sorted(bad, 2, 11) == 0, "is_sorted failed, should be false");

    return NULL;
}

char *test_bubble_sort()
{
    bubble_sort(data, PARAMS, SAMPLES);
    mu_assert(is_sorted(data, PARAMS, SAMPLES), "bubble sort failed");

    return NULL;
}

// TODO: make sure it can index a 2D-array for rows at the input.
// eg: split(arr[1], dest1, dest2, SIZE);

char *test_split()
{
    double arr[6] = {20, 1.2, 30, 25, 22.5, 23};
    double arr2[7] = {1.1, 3.2, 3, 2, 2.1, 1, 3.4};

    double *part1_arr = NULL;
    double *part2_arr = NULL;
    double *part1_arr2 = NULL;
    double *part2_arr2 = NULL;

    part1_arr = malloc(3 * sizeof(double));
    check_mem(part1_arr); // makes sure memory is allocated

    part2_arr = malloc(3 * sizeof(double));
    check_mem(part2_arr); // makes sure memory is allocated

    part1_arr2 = malloc(3 * sizeof(double));
    check_mem(part1_arr2); // makes sure memory is allocated

    part2_arr2 = malloc(3 * sizeof(double));
    check_mem(part2_arr2); // makes sure memory is allocated

    split(arr, part1_arr, part2_arr, 0, 5);
    split(arr2, part1_arr2, part2_arr2, 0, 6);

    mu_assert((int)(*(part1_arr)*100) == 2000, "split failed");
    mu_assert((int)(*(part1_arr + 2) * 100) == 3000, "split failed");

    mu_assert((int)(*(part1_arr)*100) == 2000, "split failed");
    mu_assert((int)(*(part1_arr + 2) * 100) == 3000, "split failed");

    mu_assert((int)(*(part2_arr)*100) == 2500, "split failed");
    mu_assert((int)(*(part2_arr + 2) * 100) == 2300, "split failed");

    mu_assert(part1_arr2[0] == 1.1, "split failed");
    mu_assert(part1_arr2[2] == 3, "split failed");

    mu_assert(part2_arr2[0] == 2.1, "split failed");
    mu_assert(part2_arr2[2] == 3.4, "split failed");

    free(part1_arr);
    free(part1_arr2);
    free(part2_arr);
    free(part2_arr2);

    return NULL;

error:
    free(part1_arr);
    free(part1_arr2);
    free(part2_arr);
    free(part2_arr2);

    return NULL;
}

char *test_median()
{
    double arr[6] = {1.2, 20, 22.5, 23, 25, 30};
    double arr2[7] = {1, 1.1, 2, 2.1, 3, 3.2, 3.4};

    double median1 = median(arr, 6);
    double median2 = median(arr2, 7);

    mu_assert(median1 == 22.75, "median failed");
    mu_assert(median2 == 2.1, "median failed");

    return NULL;
}

char *test_average()
{
    double arr[6] = {20, 1.2, 30, 25, 22.5, 23};
    double arr2[7] = {1.1, 3.2, 3, 2, 2.1, 1, 3.4};

    double ave = average(arr, 0, 5);
    double ave2 = average(arr2, 0, 6);

    // handle rounding errors
    mu_assert((int)(ave * 100) == 2028, "average failed");
    mu_assert((int)(ave2 * 100) == 225, "average failed");

    ave = average(arr, 1, 5);
    mu_assert((int) (ave * 100) == 2034, "average failed when offset from start");

    return NULL;
}

char *test_var()
{
    double test_data1[8] = {1, 12.5, 13, 14, 15, 16, 18, 80}; // mean: 21.1875 var: 58.81
    double test2[7] = {1, 12.5, 13, 14, 15, 16, 18}; // mean: 12.7857 var: 11.78
    double test3[6] = {12.5, 13, 14, 15, 16, 18}; // mean: 14.75 var: 3.25

    double var;

    var = variance(test_data1, 0, 7);
    mu_assert( (int) (var * 100) == 5881, "var test1 failed");


    var = variance(test2, 0, 6);
    mu_assert( (int) (var * 100) == -1178, "var test2 failed");


    var = variance(test3, 0, 5);
    mu_assert( (int) (var * 100) == 325, "var test3 failed");

    var = variance(test_data1, 0, 6);
    mu_assert( (int) (var * 100) == -1178, "var failed when offset from end");

    var = variance(test_data1, 1, 6);
    mu_assert( (int) (var * 100) == 325, "var failed when offset from start");

    return NULL;
}

char *test_iqr() 
{   
    double test[8] = {1, 12.5, 13, 14, 15, 16, 18, 80}; // mean: 21.1875 var: 58.81 iqr: 4.25
    double iqrv;

    double *part1 = malloc(3 * sizeof(double));
    double *part2 = malloc(3 * sizeof(double));

    split(test, part1, part2, 1, 6);

    mu_assert((int) (part1[1] * 100) == 1300, "failed iqr split...");
    mu_assert((int) (part2[1] * 100) == 1600, "failed iqr split...");

    free(part1);
    free(part2);

    iqrv = iqr(test, 0, 7);
    mu_assert((int) (iqrv * 100) == 425, "wrong iqr");

    iqrv = iqr(test, 0, 6);
    mu_assert((int) (iqrv * 100) == 350, "wrong iqr when offset from end");

    iqrv = iqr(test, 1, 6);
    mu_assert((int) (iqrv * 100) == 300, "wrong iqr when offset from start");

    return NULL;
}

char *test_remove_outliers()
{
    int rc;
    //first (mean: 26.64, var: 53.36 to 80, iqr: 4.25, 80 is an outlier)
    //second (mean: 12.41, var: 11.41 to 1, iqr: 3.5, 1 is an outlier)
    //third (mean: 14.75, var: 3.25 to 18, iqr: 3, 18 is not an outlier)
    double test_data1[8] = {1, 12.5, 13, 14, 15, 16, 18, 80};

    int start_idx = 0, end_idx = 7;

    double var = variance(test_data1, start_idx, end_idx);
    double interquartile = iqr(test_data1, start_idx, end_idx);

    rc = remove_outliers(test_data1, 8, var, interquartile);
    mu_assert(test_data1[7] == -1, "failed to remove an outlier");
    mu_assert(rc == 2, "wrong return value from remove_outliers");

    end_idx--;
    var = variance(test_data1, start_idx, end_idx);
    interquartile = iqr(test_data1, start_idx, end_idx);

    rc = remove_outliers(test_data1, 8, var, interquartile);
    mu_assert(test_data1[0] == -1, "failed to remove an outlier");
    mu_assert(rc == 1, "wrong return value from remove_outliers");

    start_idx++;
    var = variance(test_data1, start_idx, end_idx);
    interquartile = iqr(test_data1, start_idx, end_idx);

    debug("iqr: %.2lf", interquartile);
    debug("var: %.2lf", var);

    rc = remove_outliers(test_data1, 8, var, interquartile);
    mu_assert(rc == 0, "wrong return value from remove_outliers");
    mu_assert(test_data1[1] == 12.5, "removed wrong outlier");
    mu_assert(test_data1[6] == 18, "removed wrong outlier");

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_read_samples);
    mu_run_test(test_is_sorted);
    mu_run_test(test_bubble_sort);
    mu_run_test(test_split);
    mu_run_test(test_median);
    mu_run_test(test_average);
    mu_run_test(test_var);
    mu_run_test(test_iqr);
    mu_run_test(test_remove_outliers);
    

    return NULL;
}

RUN_TESTS(all_tests);