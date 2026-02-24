#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

void initGrid(const char *infile, int dim, double a[dim][dim]);
void saveGrid(const char *outfile, int dim, double a[dim][dim]);
void calcGrid(int dim, double a[dim][dim]);

int main(int argc, char **argv) {
  if (argc != 5) {
    printf("Arguments: <dimension> <nsteps> <infile> <outfile>\n");
    return 0;
  }

  const int dim = atoi(argv[1]);
  const int t = atoi(argv[2]);
  int check = dim - 2;
  if ((check <= 0) || ((check & (check-1)) != 0))
    printf("dim must be N+2, where N is a positive power of 2.\n");
  if (t <= 0)
    printf("Number of time steps must be > 0.\n");

  double (*grid)[dim] = malloc(sizeof(double[dim][dim]));
  struct timespec startTime, endTime;
  timespec_get(&startTime, TIME_UTC);
  initGrid(argv[3], dim, grid);
  for (int i = 0; i < t; i++)
    calcGrid(dim, grid);
  saveGrid(argv[4], dim, grid);
  timespec_get(&endTime, TIME_UTC);
  long long seconds = endTime.tv_sec - startTime.tv_sec;
  long nanoseconds = endTime.tv_nsec - startTime.tv_nsec;
  double s = (double) seconds + (double) nanoseconds * 1e-9;
  printf("Time = %.9lf seconds\n", s);
  free(grid);

  return 0;
}

void initGrid(const char *infile, int dim, double a[dim][dim]) {
  FILE *in = fopen(infile, "r");
  if (!in)
    printf("Cannot open file: %s\n", infile);

  for (int row = 0; row < dim; row++) {
    for (int col = 0; col < dim; col++) {
      double temp;
      if (fscanf(in, "%lf", &temp) != EOF) {
        a[row][col] = temp;
      } else {
        printf("File does not have enough data points: %s\n", infile);
        fclose(in);
        return;
      }
    }
  }

  fclose(in);
}

void calcGrid(int dim, double a[dim][dim]) {
  double (*t)[dim] = malloc(sizeof(double[dim][dim]));
  memcpy(t, a, (size_t)dim * dim * sizeof(double));

  #pragma omp parallel for
  for (int row = 1; row < dim - 1; row++) {
    for (int col = 1; col < dim - 1; col++) {
      double value = (a[row][col] + a[row - 1][col] + a[row + 1][col] +  a[row][col + 1] + a[row][col - 1]) / 5;
      t[row][col] = value; 
    }
  }

  memcpy(a, t, (size_t)dim * dim * sizeof(double));
  free(t);
}

void saveGrid(const char *outfile, int dim, double a[dim][dim]) {
  FILE *out = fopen(outfile, "w");
  if (!out) {
    printf("Cannot open output file: %s\n", outfile);
    return;
  }

  for (int row = 1; row < dim - 1; row++) {
    for (int col = 1; col < dim - 1; col++) {
      fprintf(out, "%6.3lf  ", a[row][col]);
    }
    fprintf(out, "\n");
  }

  fclose(out);
}
