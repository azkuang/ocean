// Create data files for ocean simulation
// Interior grid sizes: 32, 128, 512, 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// initialize comlete grid with specified value, or random value [0,30] if val < 0
void initGrid(size_t rows, size_t cols, double a[rows][cols], double val);

void saveGrid(const char *outfile, size_t rows, size_t cols, double a[rows][cols]);


int main(int argc, char **argv) {
    // default size is 256, overwritten by command-line argument
    int size = 256;
    if (argc == 2) size = atoi(argv[1]);
    const int dim = size + 2;
    printf("%d x %d array\n", dim, dim);

    // allocate grid array
    double (*grid)[dim] = malloc(sizeof(double[dim][dim]));

    // set up file name string and prefix string
    char prefix[30] = "";
    char name[80] = "";
    sprintf(prefix, "grid%u", dim);
    // all random points
    initGrid(dim, dim, grid, -1.0);
    strcpy(name, prefix);
    strcat(name, "_random.txt");
    saveGrid(name, dim, dim, grid);

    // hot on top boundary
    initGrid(dim, dim, grid, 0.0);
    for (int j = 0; j < dim; j++) grid[0][j] = 30.0;
    strcpy(name, prefix);
    strcat(name, "_hotTop.txt");
    saveGrid(name, dim, dim, grid);

    // cold on top boundary and hot on other boundaries
    initGrid(dim, dim, grid, 15.0);
    for (int j = 0; j < dim; j++) {
        grid[0][j] = 0.0;
        grid[dim - 1][j] = 30.0;
    }
    for (int i = 0; i < dim; i++) {
        grid[i][0] = 30.0;
        grid[i][dim - 1] = 30.0;
    }
    strcpy(name, prefix);
    strcat(name, "_coldTop.txt");
    saveGrid(name, dim, dim, grid);

    // hot in top left corner
    initGrid(dim, dim, grid, 0);
    for (int j = 0; j < dim; j++) {
        grid[0][j] = 0.0;
        grid[dim - 1][j] = 0.0;
    }
    for (int i = 0; i < dim; i++) {
        grid[i][0] = 0.0;
        grid[i][dim - 1] = 0.0;
    }
    for (int k = 0; k <= (dim - 2) / 4; k++) {
        grid[0][k] = 30.0;
        grid[k][0] = 30.0;
    }
    strcpy(name, prefix);
    strcat(name, "_hotCorner.txt");
    saveGrid(name, dim, dim, grid);

    free(grid);

    return 0;
}

void initGrid(size_t rows, size_t cols, double a[rows][cols], double val) {
    if (val >= 0) {
        for (int x = 0; x < rows; x++) {
            for (int y = 0; y < cols; y++) {
                a[x][y] = val; // values between 0 and 30
            }
        }
    } else {
        srand(0xbaadc0dd);
        for (int x = 0; x < rows; x++) {
            for (int y = 0; y < cols; y++) {
                a[x][y] = (double) (rand() % 30); // values between 0 and 30
            }
        }
    }
}

void saveGrid(const char *outfile, size_t rows, size_t cols, double a[rows][cols]) {
    FILE *out = fopen(outfile, "w");
    if (!out) {
        printf("Cannot open output file: %s\n", outfile);
        return;
    }
    // skip first and last row/column (don't save boundary points)
    for (size_t x = 0; x < rows; x++) {
        for (size_t y = 0; y < cols; y++) {
            // printing three decimal places
            // data values expected to be between 0.0 and 30.0,
            // but this code still works if the values are outside that range
            fprintf(out, "%6.3lf  ", a[x][y]);
        }
        fprintf(out, "\n");
    }
    fclose(out);
}
