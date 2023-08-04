#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>


size_t SIZE;


typedef unsigned int M_TYPE;
#define M_MPI_TYPE MPI_UNSIGNED
#define PLACEHOLDER "%6u"



void fill_random_matrix(M_TYPE m[]) {
	size_t i, j;
	for (i = 0; i < SIZE; i++)
		for (j = 0; j < SIZE; j++)
			m[(i * SIZE) + j] = (M_TYPE)(rand() % 100);
}

void matrix_transpose(M_TYPE m[], M_TYPE mT[]) {
	size_t i, j;
	for (i = 0; i < SIZE; i++)
		for (j = 0; j < SIZE; j++)
			mT[(j * SIZE) + i] = m[(i * SIZE) + j];
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



// Use monodimensional arrays instead of matrices for A, B, and C.
void mmult_mono(M_TYPE a[], M_TYPE b[], M_TYPE c[], int numprocs, int procid, double *start, double *end) {
	size_t i, j, k;



	// Get initial time
	*start = MPI_Wtime();



	// Broadcasts data from one member of a group to all members of the group.
	MPI_Bcast(		b,						// Pointer to the data buffer
					SIZE * SIZE,			// Number of data elements in the buffer
					M_MPI_TYPE,				// MPI data type of the elements in the send buffer
					0, MPI_COMM_WORLD);
	


	// Prepare the displacements and counts matrices. They are the same for both MPI_Scatterv and MPI_Gatherv.
	// The idea is to split the rows of matrix A equally among all processors. 
	// The entire row goes to the assigned process, so the rows don't get fragmented.
	int counts[numprocs], displacements[numprocs];
	int displacement = 0;

	for (i = 0; i < numprocs; i++) {
		counts[i] = ((i < (SIZE % numprocs)) ? ((SIZE / numprocs) + 1) : (SIZE / numprocs)) * SIZE;
		displacements[i] = displacement;
		displacement += counts[i];
	}
	


	// Reception buffer declaration.
	// For each process, this contains only the rows of A that have been assigned to that process.
	int recvcount = ((procid < (SIZE % numprocs)) ? ((SIZE / numprocs) + 1) : (SIZE / numprocs)) * SIZE;
	M_TYPE *recvbuf = malloc(recvcount * sizeof(M_TYPE));
	


	// Scatters data from one member across all members of a group.
	MPI_Scatterv(	a,						// Pointer to a buffer that contains the data to be sent by the root process
					counts,					// Number of elements to send to each process
					displacements,			// Locations of the data to send to each communicator process
					M_MPI_TYPE,				// MPI data type of each element in the buffer
					recvbuf,				// Pointer to a buffer that contains the data that is received on each process
					recvcount,				// Number of elements in the receive buffer
					M_MPI_TYPE,				// MPI data type of the elements in the receive buffer
					0, MPI_COMM_WORLD);
	


	//	MPI_Barrier(	MPI_COMM_WORLD);



	// Send buffer declaration.
	// For each process, the rows of C corresponding to the rows of A assigned to that process
	// will be calculated in this buffer.
	int sendcount = ((procid < (SIZE % numprocs)) ? ((SIZE / numprocs) + 1) : (SIZE / numprocs)) * SIZE;
	M_TYPE *sendbuf = malloc(sendcount * sizeof(M_TYPE));
	
	for (i = 0; i < sendcount / SIZE; i++)
		for (j = 0; j < SIZE; j++)
			for (k = 0; k < SIZE; k++)
				sendbuf[(i * SIZE) + j] += recvbuf[(i * SIZE) + k] * b[(k * SIZE) + j];



	// Gathers variable data from all members of a group to one member.
	MPI_Gatherv(	sendbuf,				// Pointer to a buffer that contains the data to be sent to the root process
					sendcount,				// Number of elements in the send buffer
					M_MPI_TYPE,				// MPI data type of each element in the buffer
					c,						// Pointer to a buffer on the root process that contains the data that is received from each process, including data that is sent by the root process
					counts,					// Number of elements that is received from each process
					displacements,			// Location, relative to the recvbuf parameter, of the data from each communicator process
					M_MPI_TYPE,				// MPI data type of each element in buffer
					0, MPI_COMM_WORLD);
	


	//	MPI_Barrier(	MPI_COMM_WORLD);



	// Get final time
	*end = MPI_Wtime();
}

// Monodim. + transposition of B to access both A and B row-wise in order to reduce cache misses.
void mmult_transp(M_TYPE a[], M_TYPE bT[], M_TYPE c[], int numprocs, int procid, double *start, double *end) {
	size_t i, j, k;



	// Get initial time
	*start = MPI_Wtime();



	// Broadcasts data from one member of a group to all members of the group.
	MPI_Bcast(		bT,						// Pointer to the data buffer
					SIZE * SIZE,			// Number of data elements in the buffer
					M_MPI_TYPE,				// MPI data type of the elements in the send buffer
					0, MPI_COMM_WORLD);
	


	// Prepare the displacements and counts matrices. They are the same for both MPI_Scatterv and MPI_Gatherv.
	// The idea is to split the rows of matrix A equally among all processors. 
	// The entire row goes to the assigned process, so the rows don't get fragmented.
	int counts[numprocs], displacements[numprocs];
	int displacement = 0;

	for (i = 0; i < numprocs; i++) {
		counts[i] = ((i < (SIZE % numprocs)) ? ((SIZE / numprocs) + 1) : (SIZE / numprocs)) * SIZE;
		displacements[i] = displacement;
		displacement += counts[i];
	}
	


	// Reception buffer declaration.
	// For each process, this contains only the rows of A that have been assigned to that process.
	int recvcount = ((procid < (SIZE % numprocs)) ? ((SIZE / numprocs) + 1) : (SIZE / numprocs)) * SIZE;
	M_TYPE *recvbuf = malloc(recvcount * sizeof(M_TYPE));
	


	// Scatters data from one member across all members of a group.
	MPI_Scatterv(	a,						// Pointer to a buffer that contains the data to be sent by the root process
					counts,					// Number of elements to send to each process
					displacements,			// Locations of the data to send to each communicator process
					M_MPI_TYPE,				// MPI data type of each element in the buffer
					recvbuf,				// Pointer to a buffer that contains the data that is received on each process
					recvcount,				// Number of elements in the receive buffer
					M_MPI_TYPE,				// MPI data type of the elements in the receive buffer
					0, MPI_COMM_WORLD);
	


	//	MPI_Barrier(	MPI_COMM_WORLD);



	// Send buffer declaration.
	// For each process, the rows of C corresponding to the rows of A assigned to that process
	// will be calculated in this buffer.
	int sendcount = ((procid < (SIZE % numprocs)) ? ((SIZE / numprocs) + 1) : (SIZE / numprocs)) * SIZE;
	M_TYPE *sendbuf = malloc(sendcount * sizeof(M_TYPE));
	
	for (i = 0; i < sendcount / SIZE; i++)
		for (j = 0; j < SIZE; j++)
			for (k = 0; k < SIZE; k++)
				sendbuf[(i * SIZE) + j] += recvbuf[(i * SIZE) + k] * bT[(j * SIZE) + k];
	


	// Gathers variable data from all members of a group to one member.
	MPI_Gatherv(	sendbuf,				// Pointer to a buffer that contains the data to be sent to the root process
					sendcount,				// Number of elements in the send buffer
					M_MPI_TYPE,				// MPI data type of each element in the buffer
					c,						// Pointer to a buffer on the root process that contains the data that is received from each process, including data that is sent by the root process
					counts,					// Number of elements that is received from each process
					displacements,			// Location, relative to the recvbuf parameter, of the data from each communicator process
					M_MPI_TYPE,				// MPI data type of each element in buffer
					0, MPI_COMM_WORLD);
	


	//	MPI_Barrier(	MPI_COMM_WORLD);



	// Get final time
	*end = MPI_Wtime();
}

// Monodim. + change the order of the three loops from i-j-k to k-i-j.
void mmult_kij(M_TYPE a[], M_TYPE b[], M_TYPE c[], int numprocs, int procid, double *start, double *end) {
	size_t i, j, k;



	// Get initial time
	*start = MPI_Wtime();



	// Broadcasts data from one member of a group to all members of the group.
	MPI_Bcast(		b,						// Pointer to the data buffer
					SIZE * SIZE,			// Number of data elements in the buffer
					M_MPI_TYPE,				// MPI data type of the elements in the send buffer
					0, MPI_COMM_WORLD);
	


	// Prepare the displacements and counts matrices. They are the same for both MPI_Scatterv and MPI_Gatherv.
	// The idea is to split the rows of matrix A equally among all processors. 
	// The entire row goes to the assigned process, so the rows don't get fragmented.
	int counts[numprocs], displacements[numprocs];
	int displacement = 0;

	for (i = 0; i < numprocs; i++) {
		counts[i] = ((i < (SIZE % numprocs)) ? ((SIZE / numprocs) + 1) : (SIZE / numprocs)) * SIZE;
		displacements[i] = displacement;
		displacement += counts[i];
	}
	


	// Reception buffer declaration.
	// For each process, this contains only the rows of A that have been assigned to that process.
	int recvcount = ((procid < (SIZE % numprocs)) ? ((SIZE / numprocs) + 1) : (SIZE / numprocs)) * SIZE;
	M_TYPE *recvbuf = malloc(recvcount * sizeof(M_TYPE));
	


	// Scatters data from one member across all members of a group.
	MPI_Scatterv(	a,						// Pointer to a buffer that contains the data to be sent by the root process
					counts,					// Number of elements to send to each process
					displacements,			// Locations of the data to send to each communicator process
					M_MPI_TYPE,				// MPI data type of each element in the buffer
					recvbuf,				// Pointer to a buffer that contains the data that is received on each process
					recvcount,				// Number of elements in the receive buffer
					M_MPI_TYPE,				// MPI data type of the elements in the receive buffer
					0, MPI_COMM_WORLD);
	


	//	MPI_Barrier(	MPI_COMM_WORLD);



	// Send buffer declaration.
	// For each process, the rows of C corresponding to the rows of A assigned to that process
	// will be calculated in this buffer.
	int sendcount = ((procid < (SIZE % numprocs)) ? ((SIZE / numprocs) + 1) : (SIZE / numprocs)) * SIZE;
	M_TYPE *sendbuf = malloc(sendcount * sizeof(M_TYPE));
	
	for (k = 0; k < SIZE; k++)
		for (i = 0; i < sendcount / SIZE; i++)
			for (j = 0; j < SIZE; j++)
				sendbuf[(i * SIZE) + j] += recvbuf[(i * SIZE) + k] * b[(k * SIZE) + j];



	// Gathers variable data from all members of a group to one member.
	MPI_Gatherv(	sendbuf,				// Pointer to a buffer that contains the data to be sent to the root process
					sendcount,				// Number of elements in the send buffer
					M_MPI_TYPE,				// MPI data type of each element in the buffer
					c,						// Pointer to a buffer on the root process that contains the data that is received from each process, including data that is sent by the root process
					counts,					// Number of elements that is received from each process
					displacements,			// Location, relative to the recvbuf parameter, of the data from each communicator process
					M_MPI_TYPE,				// MPI data type of each element in buffer
					0, MPI_COMM_WORLD);
	


	//	MPI_Barrier(	MPI_COMM_WORLD);



	// Get final time
	*end = MPI_Wtime();
}



int main(int argc, char **argv) {
	//  Check that the arguments are at least 3: the executable file and the size of the matrix.
	//  If an additional character is entered, the print function is enabled.
	if (argc < 3) {
		printf("\n USAGE:\n\t %s  SIZE  <0 | 1>  [any character for enable verbose mode]\n", argv[0]);
		printf("\t 0 --> Use monodimensional arrays instead of matrices.\n");
		printf("\t 1 --> (0) + transpose the matrix B to access both A and B row-wise (less cache misses).\n");
		printf("\t 2 --> (0) + change the order of the three loops from i-j-k to k-i-j.\n");
		exit(1);
	}



	int	numprocs, procid;
	double start, end;
	
	SIZE = (size_t)atoi(argv[1]);
	M_TYPE *a = malloc(SIZE * SIZE * sizeof(M_TYPE)),
	       *b = malloc(SIZE * SIZE * sizeof(M_TYPE)),
		   *c = malloc(SIZE * SIZE * sizeof(M_TYPE));
	M_TYPE *bT = malloc(SIZE * SIZE * sizeof(M_TYPE));



	// Random initialization
	srand((unsigned int)time(NULL));
	


	// Initialize the MPI environment
	MPI_Init(&argc, &argv);
	
	// Get the number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	
	// Get the rank of the process
	MPI_Comm_rank(MPI_COMM_WORLD, &procid);



	// Process 0 fills the input matrices and broadcasts them to the rest
	// (actually, only the relevant stripe of A is sent to each process)
	if (procid == 0) {
		fill_random_matrix(a);
		fill_random_matrix(b);
		matrix_transpose(b, bT);
	}



	switch (atoi(argv[2])) {
		case 0:
			mmult_mono(a, b, c, numprocs, procid, &start, &end);
			break;
		
		case 1:
			mmult_transp(a, bT, c, numprocs, procid, &start, &end);
			break;
		
		case 2:
			mmult_kij(a, b, c, numprocs, procid, &start, &end);
			break;

		default:
			printf("\n USAGE:\n\t %s  SIZE  <0 | 1>  [any character for enable verbose mode]\n", argv[0]);
			printf("\t 0 --> Use monodimensional arrays instead of matrices.\n");
			printf("\t 1 --> (0) + transpose the matrix B to access both A and B row-wise (less cache misses).\n");
			printf("\t 2 --> (0) + change the order of the three loops from i-j-k to k-i-j.\n");
			exit(1);
			break;
	}



	if (procid == 0) {
		// If "print mode" is enabled, print the two matrices to be added and the solution matrix
		if (argc > 3) {
			printf("\n\n");
			print_matrix(a);
			printf("\n\n\t       * \n");
			print_matrix(b);
			printf("\n\n\t       = \n");
			print_matrix(c);
			printf("\n\n");
		}
		
		printf("\n\n");
		printf("Execution time: %1.5f milliseconds.\n", (end - start) * 1000);
	}
	


	// Finalize the MPI environment
	MPI_Finalize();



	return 0;
}