#include <util/util.h>
#include "dbg.h"

#define PARAMS 9
#define SAMPLES 11

int main() {
    double data[PARAMS][SAMPLES];

    int i, j; // loop counters

    int rc = 1; // return value storage

    // array indexing offsets
    int start_offset = 0, end_offset = SAMPLES - 1;

    // arrays that keep track of how many outliers
    // are removed in order to find appropriate Vi values
    int start[SAMPLES];
    int end[SAMPLES];

    // observed values for each parameter (row)
    double vi[PARAMS];

    double var, interq;

    FILE *input = fopen("river1.dat", "r");
    check(input, "failed to open river1.dat");

    // read the samples from file river1.dat    
    read_samples(input, data, PARAMS, SAMPLES);

    // sort all the rows
    bubble_sort(data, PARAMS, SAMPLES);
    check(is_sorted(data, PARAMS, SAMPLES), "failed to sort the rows.");

    // for each row...
    for (i = 0; i < PARAMS; i++) {
        while (rc) {
            var = variance(data[i], start_offset, end_offset);
            interq = iqr(data[i], start_offset, end_offset);

            // remove one element at a time and return an indicator
            // of the position of the removed element to rc
            rc = remove_outliers(data[i], SAMPLES, var, interq);

            // decision based on return value of remove_outliers 'rc'
            switch (rc)
            {
            case 1:
                start_offset++;
                break;
            case 2:
                end_offset--;
                break;
            case 0:
                start[i] = start_offset;
                end[i] = end_offset;
                start_offset = 0;
                end_offset = SAMPLES - 1;
                break;
            
            default:
                sentinel("unexpected rc from remove_outliers: rc = %d", rc);
                break;
            }
        }
    }

    // find Vi for each parameter (row);
    for (j = 0; j < PARAMS; j++) {
        vi[j] = average(data[j], start[j], end[j]);
    }

    


    fclose(input);
    return 0;
error:
    if (input) fclose(input);
    return 1;
}
