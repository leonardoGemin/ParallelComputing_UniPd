#include <stdio.h>
#include <time.h>


#define SIZE 8
#define PLACEHOLDER "u"
typedef unsigned int M_TYPE;


void fill_matrix(M_TYPE m[][SIZE]) {
	static M_TYPE n = 0;
	size_t i, j;
	for (i = 0; i < SIZE; i++)
		for (j = 0; j < SIZE; j++)
			m[i][j] = n++;
}

void print_matrix(M_TYPE m[][SIZE]) {
	size_t i, j;
	for (i = 0; i < SIZE; i++) {
		printf("\n\t| ");
		for (j = 0; j < SIZE; j++)
			printf("%6" PLACEHOLDER " ", m[i][j]);
		printf("|");
	}
}


int main(int argc, char **argv) {
  int i, j, k;

  M_TYPE a[SIZE][SIZE], b[SIZE][SIZE], c[SIZE][SIZE];

  fill_matrix(a);
  fill_matrix(b);

  clock_t start = clock();

  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE; j++) {
        c[i][j] = 0;
        for (k = 0; k < SIZE; k++){
            c[i][j] += a[i][k] * b[k][j];
        }
    }
  }
  
  clock_t end = clock();

  printf("\n\n");
  print_matrix(a);
  printf("\n\n\t       * \n");
  print_matrix(b);
  printf("\n\n\t       = \n");
  print_matrix(c);
  printf("\n\n\n\n");
  printf("Execution time: %1.5f milliseconds.\n", (double)(end - start) * 1000 / CLOCKS_PER_SEC);

  return 0;
}
