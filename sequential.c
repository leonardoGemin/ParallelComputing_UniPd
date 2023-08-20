#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define PLACEHOLDER "%6u"
typedef unsigned int M_TYPE;


size_t SIZE;


void fill_random_matrix(M_TYPE m[]) {
	size_t i, j;
	for (i = 0; i < SIZE; i++)
		for (j = 0; j < SIZE; j++)
			m[(i * SIZE) + j] = (M_TYPE)(rand() % 100);
}

void print_matrix(M_TYPE m[]) {
	size_t i, j;
	for (i = 0; i < SIZE; i++) {
		printf("\n\t| ");
		for (j = 0; j < SIZE; j++)
			printf(PLACEHOLDER " ", m[(i * SIZE) + j]);
		printf("|");
	}
}


int main(int argc, char **argv) {
	//  Check that the arguments are at least 2: the executable file and the size of the matrix.
	//  If an additional character is entered, the print function is enabled.
	if (argc < 2) {
		printf("\n USAGE:\n\t %s  SIZE  [any character for enable verbose mode]\n", argv[0]);
		exit(1);
	}

	SIZE = (size_t)atoi(argv[1]);

	int i, j, k;

	M_TYPE *a = malloc(SIZE * SIZE * sizeof(M_TYPE)),
	       *b = malloc(SIZE * SIZE * sizeof(M_TYPE)),
		   *c = malloc(SIZE * SIZE * sizeof(M_TYPE));

	// Random initialization
	srand((unsigned int)time(NULL));

	fill_random_matrix(a);
	fill_random_matrix(b);


	clock_t start = clock();

	// standard N^3 algorithm
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			for (k = 0; k < SIZE; k++){
				c[(i * SIZE) + j] += a[(i * SIZE) + k] * b[(k * SIZE) + j];
			}
		}
	}
	
	clock_t end = clock();

	// If "print mode" is enabled, print the two matrices to be multiplied and the solution matrix
	if (argc > 2) {
		printf("\n\n");
		print_matrix(a);
		printf("\n\n\t       * \n");
		print_matrix(b);
		printf("\n\n\t       = \n");
		print_matrix(c);
		printf("\n\n");
	}
	
	printf("\n\n");
	printf("Execution time: %1.5f milliseconds.\n", (double)(end - start) * 1000 / CLOCKS_PER_SEC);

	return 0;
}
