#include <stdio.h>
#include <mpi.h>


#define SIZE 10


/*typedef int M_TYPE;
#define M_MPI_TYPE MPI_INT
#define PLACEHOLDER "d"*/
typedef unsigned long long M_TYPE;
#define M_MPI_TYPE MPI_UNSIGNED_LONG_LONG
#define PLACEHOLDER "llu"



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
	int	numprocs, procid;
	size_t from, to;
	size_t i, j, k;
	double start, end;
	
	M_TYPE a[SIZE][SIZE], b[SIZE][SIZE], c[SIZE][SIZE];
	


	// Initialize the MPI environment
	MPI_Init(&argc, &argv);
	
	// Get the number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	
	// Get the rank of the process
	MPI_Comm_rank(MPI_COMM_WORLD, &procid);



	// Process 0 fills the input matrices and broadcasts them to the rest
	// (actually, only the relevant stripe of A is sent to each process)
	if (procid == 0) {
		fill_matrix(a);
		fill_matrix(b);
	}



	// Get initial time
	start = MPI_Wtime();



	// Trasmette i dati da un membro di un gruppo a tutti i membri del gruppo.
	MPI_Bcast(		b,						// Puntatore al buffer dei dati
					SIZE * SIZE,			// Numero di elementi di dati nel buffer
					M_MPI_TYPE,				
					0, MPI_COMM_WORLD);
	


	int counts[numprocs], displacements[numprocs];
	int displacement = 0;

	for (i = 0; i < numprocs; i++) {
		counts[i] = (i < (SIZE % numprocs)) ? (((SIZE / numprocs) + 1) * SIZE) : ((SIZE / numprocs) * SIZE);
		displacements[i] = displacement;
		displacement += counts[i];
	}
	


	M_TYPE recvbuf[(procid < (SIZE % numprocs)) ? ((SIZE / numprocs) + 1) : (SIZE / numprocs)][SIZE];
	int recvcount = sizeof(recvbuf) / sizeof(M_TYPE);



	// Distribuisce i dati da un membro a tutti i membri di un gruppo.
	MPI_Scatterv(	a,						// Puntatore al buffer che contiene i dati da inviare dal processo radice
					counts,					// Numero di elementi nel buffer di invio
					displacements,			// Posizioni dei dati da inviare a ogni processo di comunicatore
					M_MPI_TYPE,				
					recvbuf,				// Puntatore al buffer contenente i dati ricevuti in ogni processo
					recvcount,				// Numero di elementi nel buffer di ricezione
					M_MPI_TYPE,
					0, MPI_COMM_WORLD);
	


//	MPI_Barrier(	MPI_COMM_WORLD);



	M_TYPE sendbuf[(procid < (SIZE % numprocs)) ? ((SIZE / numprocs) + 1) : (SIZE / numprocs)][SIZE];
	int sendcount = sizeof(sendbuf) / sizeof(M_TYPE);

	for (i = 0; i < sendcount / SIZE; i++)
		for (j = 0; j < SIZE; j++) {
        	sendbuf[i][j] = 0;
			for (k = 0; k < SIZE; k++)
				sendbuf[i][j] += recvbuf[i][k] * b[k][j];
		}
	


	// Raccoglie i dati da tutti i membri di un gruppo a un membro.
	MPI_Gatherv(	sendbuf,				// Puntatore a un buffer che contiene i dati da inviare al processo radice
					sendcount,				// Numero di elementi nel buffer di invio
					M_MPI_TYPE,				
					c,						// Puntatore a un buffer nel processo radice contenente i dati ricevuti da ogni processo
					counts,					// Numero di elementi ricevuti da ogni processo
					displacements,			//
					M_MPI_TYPE,
					0, MPI_COMM_WORLD);
	


//	MPI_Barrier(	MPI_COMM_WORLD);



	// Get final time
	end = MPI_Wtime();



	// Print the two matrices to be added and the solution matrix
	if (procid == 0) {
		printf("\n\n");
		print_matrix(a);
		printf("\n\n\t       * \n");
		print_matrix(b);
		printf("\n\n\t       = \n");
		print_matrix(c);
		printf("\n\n\n\n");
		printf("Execution time: %1.5f milliseconds.\n", (end - start) * 1000);
	}
	


	// Finalize the MPI environment
	MPI_Finalize();



	return 0;
}