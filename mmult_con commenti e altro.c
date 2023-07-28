#include <stdio.h>
#include <mpi.h>


/*#define NRA 4
#define NCA 5
#define NRB NRA
#define NCB 6*/
#define SIZE 8

//#define max(a,b) ((a >= b) ? a : b)


typedef int M_TYPE;
#define M_MPI_TYPE MPI_INT
#define PLACEHOLDER "d"




/*void fill_matrix(size_t rows, size_t cols, M_TYPE m[][cols]) {
	static M_TYPE n = 0;
	size_t i, j;
	for (i = 0; i < rows; i++)
		for (j = 0; j < cols; j++)
			m[i][j] = n++;
}

void print_matrix(size_t rows, size_t cols, M_TYPE m[][cols]) {
	size_t i, j;
	for (i = 0; i < rows; i++) {
		printf("\n\t| ");
		for (j = 0; j < cols; j++)
			printf("%5" PLACEHOLDER " ", m[i][j]);
		printf("|");
	}
}*/
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
			printf("%5" PLACEHOLDER " ", m[i][j]);
		printf("|");
	}
}



int main(int argc, char **argv) {
	int	numprocs, procid;
	size_t from, to;
	size_t i, j, k;
	//M_TYPE a[NRA][NCA], b[NRB][NCB], c[NRA][NCB];
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
		//fill_matrix(NRA, NCA, a);
		//fill_matrix(NRB, NCB, b);
		fill_matrix(a);
		fill_matrix(b);
	}



	// Trasmette i dati da un membro di un gruppo a tutti i membri del gruppo.
	MPI_Bcast(		b,						// Puntatore al buffer dei dati
					SIZE * SIZE,			// Numero di elementi di dati nel buffer
					M_MPI_TYPE,				
					0, MPI_COMM_WORLD);
	


	
	M_TYPE recvbuf[(procid < (SIZE % numprocs)) ? ((SIZE / numprocs) + 1) : (SIZE / numprocs)][SIZE];
	int recvcount = sizeof(recvbuf) / sizeof(M_TYPE);
	
	int sendcounts[numprocs], senddispls[numprocs];
	int senddispl = 0;

	for (i = 0; i < numprocs; i++) {
		sendcounts[i] = (i < (SIZE % numprocs)) ? (((SIZE / numprocs) + 1) * SIZE) : ((SIZE / numprocs) * SIZE);
		senddispls[i] = senddispl;
		senddispl += sendcounts[i];
	}

	// Distribuisce i dati da un membro a tutti i membri di un gruppo.
	MPI_Scatterv(	a,						// Puntatore al buffer che contiene i dati da inviare dal processo radice
					sendcounts,				// Numero di elementi nel buffer di invio
					senddispls,				// Posizioni dei dati da inviare a ogni processo di comunicatore
					M_MPI_TYPE,				
					recvbuf,				// Puntatore al buffer contenente i dati ricevuti in ogni processo
					recvcount,				// Numero di elementi nel buffer di ricezione
					M_MPI_TYPE,
					0, MPI_COMM_WORLD);
	


	/*printf("%d: ", procid);
	to = (procid < (SIZE % numprocs)) ? ((SIZE / numprocs) + 1) : (SIZE / numprocs);
    for (i = 0; i < to; i++){
		for (j = 0; j < SIZE; j++) 
        	printf("%" PLACEHOLDER "\t", recvbuf[i][j]);
    }
    printf("\n");*/
	

//	MPI_Barrier(	MPI_COMM_WORLD);



	/*from = (procid < (SIZE % numprocs)) ? (procid * ((SIZE / numprocs) + 1)) : ((SIZE % numprocs) * ((SIZE / numprocs) + 1)) + ((procid - (SIZE % numprocs)) * (SIZE / numprocs));
	to = ((procid + 1) < (SIZE % numprocs)) ? ((procid + 1) * ((SIZE / numprocs) + 1)) : ((SIZE % numprocs) * ((SIZE / numprocs) + 1)) + (((procid + 1) - (SIZE % numprocs)) * (SIZE / numprocs));
	printf("Computing slice %d (from row %zu to %zu).\n", procid, from, to - 1);
	for (i = from; i < to; i++)
		for (j = 0; j < SIZE; j++) {
			c[i][j] = 0;
			for (k = 0; k < SIZE; k++)
				c[i][j] += a[i][k] * b[k][j];
		}
	*/


	M_TYPE sendbuf[(procid < (SIZE % numprocs)) ? ((SIZE / numprocs) + 1) : (SIZE / numprocs)][SIZE];
	int sendcount = sizeof(sendbuf) / sizeof(M_TYPE);

	for (i = 0; i < sendcount / SIZE; i++)
		for (j = 0; j < SIZE; j++) {
        	sendbuf[i][j] = 0;
			for (k = 0; k < SIZE; k++)
				sendbuf[i][j] += recvbuf[i][k] * b[k][j];
		}
	/*for (i = 0; i < sendcount / SIZE; i++) {
		printf("\n\t| ");
		for (j = 0; j < SIZE; j++)
			printf("%5" PLACEHOLDER " ", sendbuf[i][j]);
		printf("|");
	}*/

	/*for (i = from; i < to; i++)
		for (j = 0; j < SIZE; j++) {
			sendbuf[i][j] = 0;
			for (k = 0; k < SIZE; k++)
				sendbuf[i][j] += recvbuf[i][k] * b[k][j];
		}*/
	
	int recvcounts[numprocs], recvdispls[numprocs];
	int recvdispl = 0;

	for (i = 0; i < numprocs; i++) {
		recvcounts[i] = (i < (SIZE % numprocs)) ? (((SIZE / numprocs) + 1) * SIZE) : ((SIZE / numprocs) * SIZE);
		recvdispls[i] = recvdispl;
		recvdispl += recvcounts[i];
	}
	

	// Raccoglie i dati da tutti i membri di un gruppo a un membro.
	MPI_Gatherv(	sendbuf,				// Puntatore a un buffer che contiene i dati da inviare al processo radice
					sendcount,				// Numero di elementi nel buffer di invio
					M_MPI_TYPE,				
					c,						// Puntatore a un buffer nel processo radice contenente i dati ricevuti da ogni processo
					recvcounts,				// Numero di elementi ricevuti da ogni processo
					recvdispls,				//
					M_MPI_TYPE,
					0, MPI_COMM_WORLD);
	

//	MPI_Barrier(	MPI_COMM_WORLD);


	// Print the two matrices to be added and the solution matrix
	if (procid == 0) {
		printf("\n\n");
		print_matrix(a);//print_matrix(NRA, NCA, a);
		printf("\n\n\t       * \n");
		print_matrix(b);//print_matrix(NRB, NCB, b);
		printf("\n\n\t       = \n");
		print_matrix(c);//print_matrix(NRA, NCB, c);
		printf("\n\n");
	}
	

	// Finalize the MPI environment
	MPI_Finalize();

	return 0;
}