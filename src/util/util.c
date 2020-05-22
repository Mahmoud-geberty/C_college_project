#include <util/util.h>
#include <util/dbg.h>

// takes a 2D-array and its dimensions and returns 1 if it
// is sorted and 0 if it is not sorted
int is_sorted(double data[][11], int x_dim, int y_dim)
{
    int i, j; // loop counters

    for (i = 0; i < x_dim; i++)
    {
        for (j = 0; j < y_dim - 1; j++)
        {
            if (data[i][j] > data[i][j + 1])
            {
                return 0;
            }
        }
    }
    return 1;
}

// takes a 2D-array and array dimensions as input
//
// sort each row in the array (ascending)
void bubble_sort(double arr[][11], int x_dim, int y_dim)
{
    if (!is_sorted(arr, x_dim, y_dim))
    {
        int i = 0, j = 0; // loop counters
        int row = 0;
        int hold; // temporary storage

        do
        {
            for (i = 1; i < y_dim; i++)
            {

                for (j = 0; j < y_dim - 1; j++)
                {
                    if (arr[row][j] > arr[row][j + 1])
                    {
                        hold = arr[row][j];
                        arr[row][j] = arr[row][j + 1];
                        arr[row][j + 1] = hold;
                    }
                }
            }
            i = 1;
            j = 0;
            row++;
        } while (row < x_dim);
    }
}

// takes a linear array and 2 double pointers for dynamic memory
// and start point and end point of the array.
//
// populates each of the dynamic arrays with one half of the linear array.
void split(double src[], double *dest_part1, double *dest_part2, int start_idx, int end_idx)
{
    int size = (end_idx - start_idx) + 1;

    int i = 0;           // loop counter

    // if size is odd, then decrement it by one
    if (size % 2)
        size--;

    // TODO: fix the loop boundaries for each parts with the offsets. they are fucked up.
    // loop through the array in both directions at
    // the same time
    for (i = start_idx; i < start_idx + (size / 2); i++)
    {
        dest_part1[i - start_idx] = src[i];
        dest_part2[(size / 2 - 1) - (i - start_idx)] = src[end_idx - (i - start_idx)];
    }

}

// takes a sorted linear array and its size as input
//
// returns the median of the array
// MAKE SURE THE ARRAY IS SORTED FIRST. not validated here!!!
double median(double arr[], int size)
{
    int index; // index of the median

    // if size is odd
    if (size % 2)
    {
        index = (size - 1) / 2;
        return arr[index];
    }

    index = size / 2;
    return (arr[index] + arr[index - 1]) / 2;
}

// takes a linear array and its length as inputs
//
// returns the average/mean of the given array
double average(double arr[], int start_idx, int end_idx)
{
    int i = 0; // loop counter
    double sum = 0;

    int size = (end_idx - start_idx) + 1;

    for (i = start_idx; i < size + start_idx; i++)
    {
        sum += arr[i];
    }

    return sum / size;
}

double variance(double arr[], int start_idx, int end_idx) 
{
    double mean = average(arr, start_idx, end_idx);
    double max_var = arr[start_idx] - mean;
    if (arr[end_idx] - mean > max_var * -1) max_var = arr[end_idx] - mean;

    return max_var;
}

double iqr( double arr[], int start_idx, int end_idx)
{
    int size = (end_idx - start_idx) + 1;

    // if size is odd, then decrement it by one
    if (size % 2)
        size--;
    
    size /= 2;

    double *part1 = malloc(size * sizeof(double));

    // makes sure the pointer is not NULL
    check_mem("failed to allocate part1"); 

    double *part2 = malloc(size * sizeof(double));

    // makes sure the pointer is not NULL
    check_mem("failed to allocate part2"); 

    split(arr, part1, part2, start_idx, end_idx);

    double iqr_val = median(part2, size) - median(part1, size);

    free(part1);
    free(part2);
    return iqr_val;

error:
    if (part1) free(part1);
    if (part2) free(part2);
    return -1;
}

// takes an array and its size, the highest variance
// the iqr whether the variance is from first or last
// element of the sorted array
//
// returns 1 if removed from beginning of array
// returns 2 if removed from end of array
// returns 0 if there are no outliers
//
// remove one outlier by puting a 0 in its place
int remove_outliers(double data[], int size, double variance, double iqr)
{
    int i;           // loop counter

    double limit = iqr * 1.5;
    debug("iqr: %.2lf", iqr);
    debug("variance: %.2lf", variance);

    // indicate whether to remove element from first or last element of array
    // 1 = start from beginning or array, 0 = start from end.
    int first = 1;

    if (variance > 0) first = 0;

    if (variance < 0) variance *= -1;

    if (variance > limit) {
        if (first) {
            // delete first non_zero element of the array
            for (i = 0; i < size; i++) {
                if (data[i]) {
                    debug("to be deleted: %.2lf when i is: %d", data[i], i);
                    data[i] = 0;
                    return 1;
                }
            }
        } else {
            for (i = size - 1; i > 0; i--) {
                if (data[i] > 0) {
                    debug("to be deleted: %.2lf when i is: %d", data[i], i);
                    data[i] = 0;
                    return 2;
                }
            }
        }
    } 
    return 0;
}


// takes a target file and a target empty array and array dimensions as input
//
// reads the given file and populated the given 2D-array
void read_samples(FILE *file, double data[][11], int x_dim, int y_dim)
{
    int i, j; // loop counters

    for (i = 0; i < x_dim; i++)
    {
        for (j = 0; j < y_dim; j++)
        {
            fseek(file, (5 * j) + (55 * i), SEEK_SET);
            fscanf(file, "%lf", &data[i][j]);
        }
    }
}