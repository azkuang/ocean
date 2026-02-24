
// A simplified simulator of ocean surface temperatures
// Given an N+2 x N+2 grid, where N is a power of 2, compute the value of the interior points
// by averaging with North, East, West, and South (NEWS) neighbors.
// Perimeter points contribute values but are not updated.

// This is just starter code that will create and initialize the full grid (including border points)
// from a file, and will write the inner grid (without border points) to an output file.
// When you write your simulation code, you only compute the interior points, and only
// those points will be written to the output file.

// Program arguments:
// (1) dimensions (total number of rows and columns, including border points),
//     must be N+2 where N is a power of 2
// (2) time steps, must be an integer (ignored in this code, but will be used for the application)
// (3) name of file for initial data
// (4) name of file for output data, must only contain interior points (not the border)
// Files are expected to be properly sized. For your own sanity, do something sensible if the
// input file does not match the matrix dimensions or if either file cannot be opened.
// (We will not test this.)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Data represents a 2D grid of the ocean, indexed like a typical graphics image,
// with the Y dimension (North-South) beginning with 0 at the top and increasing as you go down.
// The data is arranged as a 2D array, where the Y dimension is the number of rows,
// and the X dimension is the number of columns.  The origin (0,0) is the top left corner.
// The Y dimension (row number) increases from top to bottom.
// The X dimension (column number) increases from left to right.

// NOTE: These functions are just to get you started and to show examples of
// memory allocation, file I/O, and timing. For the assignment, you are encouraged
// to make whatever changes you like, or write your own functions.
void initGrid(const char * infile, int dim, double a[dim][dim]); // read all data from file
void saveGrid(const char *outfile, int dim, double a[dim][dim]); // write interior points to file

int main(int argc, char **argv) {
    // arguments are x-dimension, y-dimension, and number of time steps
    if (argc != 5) {
        printf("Arguments: <dimension> <nsteps> <infile> <outfile>\n");
        return 0;
    }
    const int dim = atoi(argv[1]); // NOLINT(*-err34-c), avoid compiler warning
    const int t = atoi(argv[2]); // NOLINT(*-err34-c)
    printf("%d x %d array, %d time steps\n", dim, dim, t);
    int check = dim-2;
    if ((check <= 0) || ((check & (check-1)) != 0)) printf("dim must be N+2, where N is a positive power of 2\n");
    if (t <= 0) printf("Number of time steps must be > 0\n");

    double (*grid)[dim] = malloc(sizeof(double[dim][dim])); // 2D array with dim rows, dim cols
    // example of how to measure time
    // In the assignment, you will not measure the I/O time, just the computation.
    struct timespec startTime, endTime;
    timespec_get(&startTime, TIME_UTC);
    initGrid(argv[3], dim, grid);
    saveGrid(argv[4], dim, grid);
    timespec_get(&endTime, TIME_UTC);
    long long seconds = endTime.tv_sec - startTime.tv_sec;
    long nanoseconds = endTime.tv_nsec - startTime.tv_nsec;
    double s = (double) seconds + (double) nanoseconds * 1e-9;
    printf("Time = %.9lf seconds\n", s);
    free(grid);   // to avoid warning about a memory leak
    return 0;
}

void initGrid(const char *infile, int dim, double a[dim][dim]) {
    FILE *in = fopen(infile, "r");
    if (!in) {
        printf("Cannot open file: %s\n", infile);
    }
    for (int row = 0; row < dim; row++) {
        for (int col = 0; col < dim; col++) {
            double temp;  // read one value from the file
            if (fscanf(in, "%lf", &temp) != EOF) {
                a[row][col] = temp;
            }
            else {
                printf("File does not have enough data points: %s\n", infile);
                fclose(in);
                return;
            }
        }
    }
    fclose(in);
}

void saveGrid(const char *outfile, int dim, double a[dim][dim]) {
    FILE * out = fopen(outfile, "w");
    if (!out) {
        printf("Cannot open output file: %s\n", outfile);
        return;
    }
    // skip first and last row/column (don't save boundary points)
    for (int row = 1; row < dim-1; row++) {
        for (int col = 1; col < dim-1; col++) {
            // printing three decimal places
            // data values expected to be between 0.0 and 30.0,
            // but this code still works if the values are outside that range
            fprintf(out, "%6.3lf  ", a[row][col]);
        }
        fprintf(out, "\n");
    }
    fclose(out);
}

