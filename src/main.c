#include <string.h>
#include <time.h>

#include <util/util.h>
#include "dbg.h"

#include "colors.h"

#define PARAMS 10
#define SAMPLES 11
#define STR_SIZE 50

void delay(int milliseconds);

// void functions with no inputs
void validate(void);

void print_samples(void);
void animate_sort(void);
void animate_remove(void);

// globals ...
double data[PARAMS][SAMPLES];
int i, j;  // loop counters
int x = 0; // main while loop counter

// arrays that keep track of how many outliers
// are removed in order to find appropriate Vi values
int start[PARAMS] = {};
int end[PARAMS] = {};

int main()
{
    // observed values for each parameter (row)
    double vi[PARAMS];

    // ideal values of each param, 0 except for PH = 7
    double vd[PARAMS] = {7, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // quality index (qi) of each parameter
    double qi[PARAMS];

    // weight index of each parameter
    double wi[PARAMS] = {0.2190, 0.3710, 0.0037, 0.0062, 0.0250, 0.0074, 0.0412, 0.0124, 0.3723, 0.3723};

    // standard values
    double si[PARAMS] = {8.5, 2500, 500, 35, 200, 250, 50, 20, 6, 30};

    // water quality index
    double wqi = 0;

    // water type
    char water_type[30];

    char river_name[2][10] = {"river1", "river2"};

    FILE *input, *output;

    // main while loop, repeat the same logic 2 times
    // once for every input file.
    while (x < 2)
    {
        if (x)
        {
            input = fopen("river2.dat", "r");
            check(input, "failed to open river2.dat");

            output = fopen("river2_out.txt", "w");
            check(output, "failed to open river2_out.txt");
        }
        else
        {
            input = fopen("river1.dat", "r");
            check(input, "failed to open river1.dat");

            output = fopen("river1_out.txt", "w");
            check(output, "failed to open river1_out.txt");
        }

        // read the samples from file river1.dat
        read_samples(input, data, PARAMS, SAMPLES);

        //print the samples to the screen
        print_samples();

        // sort all the rows
        bubble_sort(data, PARAMS, SAMPLES);
        check(is_sorted(data, PARAMS, SAMPLES), "failed to sort the rows.");

        animate_sort();

        // validate the data by removing outliers
        validate();

        animate_remove();

        printf("vi: ");
        // find Vi for each parameter (row);
        for (j = 0; j < PARAMS; j++)
        {
            vi[j] = average(data[j], start[j], end[j]);
            printf("%.2lf ", vi[j]);
        }
        printf("\n");

        // find qi for each parameter
        printf("qi: ");
        for (i = 0; i < PARAMS; i++)
        {
            qi[i] = (vi[i] - vd[i]) / (si[i] - vd[i]) * 100;
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
            strncpy(water_type, "Excellent water", 30);
            printf("Water type: ");
            BOLD_GREEN;
            printf("%s\n", "Excellent water");
        }
        else if (wqi <= 100)
        {
            strncpy(water_type, "Good water", 30);
            printf("Water type: ");
            BOLD_BLUE;
            printf("%s\n", "Good water      ");
        }
        else if (wqi <= 200)
        {
            strncpy(water_type, "Poor water", 30);
            printf("Water type: ");
            YELLOW;
            printf("%s\n", "Poor water");
        }
        else if (wqi <= 300)
        {
            strncpy(water_type, "Very poor water", 30);
            printf("Water type: ");
            RED;
            printf("%s\n", "Very poor water");
        }
        else
        {
            strncpy(water_type, "Unfit for drinking", 30);
            printf("Water type: ");
            BOLD_RED;
            printf("%s\n", "Unfit for drinking");
        }

        // OUTPUT RESULTS SUMMARY TO FILES.

        // parameter names array
        const char *paramters[20] = {"pH", "E_Cond", "Tot_Ds", "Tot_H", "Ca",
                                     "Cl", "NO3", "SO4", "O2", "BOD"};

        fprintf(output, "Parameter  %-10s %-10s %-10s %-10s\n",
        "|Vi", "|Si", "|Wi", "|Qi");
        fprintf(output, "-----------------------------------------------\n");

        for (i = 0; i < PARAMS; i++)
        {
            fprintf(output, "%-10s %-10.4lf %-10.4lf %-10.4lf %-10.4lf \n",
                   paramters[i], vi[i], si[i], wi[i], qi[i]);
        }
        fprintf(output, "\n\n");
        fprintf(output, "WQI is %.2lf\n", wqi);
        fprintf(output, "%s is... %s\n", river_name[x], water_type);

        fclose(output);
        fclose(input);
        RESET_COLOR
        x++;
    }

    return 0;
error:
    if (input)
        fclose(input);
    return 1;
} // end function main *******************************************

void validate(void)
{
    int rc = 1; // return value storage
    double var, interq;

    // array indexing offsets
    int start_offset = 0, end_offset = SAMPLES - 1;

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

error:
    return;
}

// takes time in milliseconds and stops the program
// for that long
void delay(int milliseconds)
{
    long pause;
    clock_t now, then;

    pause = milliseconds * (CLOCKS_PER_SEC / 1000);
    now = then = clock();
    while ((now - then) < pause)
        now = clock();
}

void print_samples(void)
{
    BOLD_YELLOW
    data[9][0] = 1.2;
    printf("\033[2;1f");

    if (x)
    {
        delay(3000);
        printf("reading data from file 'river2.dat'... \n");
    }
    else
    {
        printf("reading data from file 'river1.dat'... \n");
    }
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
}

void animate_sort(void)
{
    printf("\033[2;1f");
    BOLD_YELLOW
    printf("      sorting arrays...                  \n");
    delay(800);
    RESET_COLOR

    BOLD_CYAN
    delay(2000);
    for (i = 0; i < PARAMS; i++)
    {
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
}

void animate_remove(void)
{
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
}