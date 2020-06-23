#include <string.h>
#include <time.h>

#include <util/util.h>
#include "dbg.h"

#include "colors.h"

#define PARAMS 10
#define SAMPLES 11
#define STR_SIZE 50

void delay(int milliseconds);

int main()
{
    double data[PARAMS][SAMPLES];

    int i, j; // loop counters

    int rc = 1; // return value storage

    // array indexing offsets
    int start_offset = 0, end_offset = SAMPLES - 1;

    // arrays that keep track of how many outliers
    // are removed in order to find appropriate Vi values
    int start[PARAMS] = {};
    int end[PARAMS] = {};

    // observed values for each parameter (row)
    double vi[PARAMS];

    // ideal values of each param, 0 except for PH = 7
    double vd[PARAMS] = {0};
    vd[0] = 7;

    // quality index (qi) of each parameter
    double qi[PARAMS];

    // weight index of each parameter
    double wi[PARAMS] = {0.2190, 0.3710, 0.0037, 0.0062, 0.0250, 0.0074, 0.0412, 0.0124, 0.3723, 0.3723};

    // temporary standard values for the time being
    double fake_si[PARAMS] = {9, 96, 50, 40, 9, 4, 2, 10, 3, 1.2};

    // water quality index
    double wqi = 0;

    char water_type[STR_SIZE];

    double var, interq;

    FILE *input = fopen("river1.dat", "r");
    check(input, "failed to open river1.dat");

    // read the samples from file river1.dat
    read_samples(input, data, PARAMS, SAMPLES);

    BOLD_YELLOW
    printf("reading data from file 'river1.dat'... \n");
    delay(2000);

    RESET_COLOR
    for (i = 0; i < PARAMS; i++)
    {
        for (j = 0; j < SAMPLES; j++)
        {
            delay(10);
            printf(" %6.2lf", data[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");

    // delay(1000);

    // sort all the rows
    bubble_sort(data, PARAMS, SAMPLES);
    check(is_sorted(data, PARAMS, SAMPLES), "failed to sort the rows.");

    printf("\033[2;1f");
    BOLD_YELLOW
    printf("sorting arrays...                     \n");
    delay(800);
    RESET_COLOR

    BOLD_CYAN
    delay(2000);
    for (i = 0; i < PARAMS; i++)
    {
        // delay(50);
        for (j = 0; j < SAMPLES; j++)
        {
            delay(20);
            printf(" %6.2lf", data[i][j]);
            fflush(stdout);
        }
        printf("\n");
    }
    RESET_COLOR
    printf("\n\n");

    // for each row...
    for (i = 0; i < PARAMS; i++)
    {
        while (rc)
        {
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
        rc = 1;
    }

    printf("\033[2;1f");
    delay(200);
    BOLD_YELLOW
    printf(" removing outliers...                    \n");
    RESET_COLOR

    delay(2000);
    for (i = 0; i < PARAMS; i++)
    {
        for (j = 0; j < SAMPLES; j++)
        {
            BOLD_CYAN
            delay(20);
            if ((int)data[i][j] == -1)
            {
                RESET_COLOR
                RED
            }
            printf(" %6.2lf", data[i][j]);
            fflush(stdout);
            RESET_COLOR
        }
        printf("\n");
    }
    printf("\n\n");
    RESET_COLOR

    // find Vi for each parameter (row);
    for (j = 0; j < PARAMS; j++)
    {
        vi[j] = average(data[j], start[j], end[j]);
    }

    // find qi for each parameter
    printf("qi: ");
    for (i = 0; i < PARAMS; i++)
    {
        qi[i] = (vi[i] - vd[i]) / (fake_si[i] - vd[i]) * 100;
        printf("%.2lf ", qi[i]);
    }
    printf("\n");

    // find wqi using qi and wi
    for (i = 0; i < PARAMS; i++)
    {
        wqi += qi[i] * wi[i];
    }
    printf("\nwqi of this river is: %.2lf\n\n", wqi);

    // determine the water type according ot wqi
    if (wqi <= 50)
    {
        strncpy(water_type, "Excellent water", STR_SIZE);
    }
    else if (wqi <= 100)
    {
        strncpy(water_type, "Good water", STR_SIZE);
    }
    else if (wqi <= 200)
    {
        strncpy(water_type, "Poor water", STR_SIZE);
    }
    else if (wqi <= 300)
    {
        strncpy(water_type, "Very poor water", STR_SIZE);
    }
    else
    {
        strncpy(water_type, "Unfit for drinking", STR_SIZE);
    }

    printf("Water type: %s\n", water_type);

    fclose(input);
    return 0;
error:
    if (input)
        fclose(input);
    return 1;
}

void delay(int milliseconds)
{
    long pause;
    clock_t now, then;

    pause = milliseconds * (CLOCKS_PER_SEC / 1000);
    now = then = clock();
    while ((now - then) < pause)
        now = clock();
}