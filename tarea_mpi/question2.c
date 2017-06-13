#include <stdio.h>
#include <mpi.h>
int main(int argc, char** argv)
{
	int rank, npr;
	int i, X;
	int A[8], B[2], C[2];
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &npr);

	for(i=0; i<8; i++) A[i] = -1;
	for(i=0; i<2; i++) B[i] = -1;
	for(i=0; i<2; i++) C[i] = -1;
	X = -1;
	if (rank == 0)
	{ 
	    for(i=0; i<8; i++) A[i] = i;
		X = 6;
	}
	// Broadcast de X desde P0 
	MPI_Bcast(&X, 1, MPI_INT, 0, MPI_COMM_WORLD);
	printf(" 1: Broadcast del Proceso0 al Proceso%d y X = %d \n", rank, X);
	MPI_Barrier(MPI_COMM_WORLD);
	// Reparto de A (8 elementos) desde P0 en trozos de tamaño 2, en B 
	MPI_Scatter(&A[0], 2, MPI_INT, &B[0], 2, MPI_INT, 0, MPI_COMM_WORLD);
	printf(" 2: SCATTER soy el Proceso%d y B = %d %d \n", rank, B[0], B[1]);
	MPI_Barrier(MPI_COMM_WORLD);
	// Suma de los 4 vectores B (2 elementos) en el vector C en P0 
	MPI_Reduce(&B[0], &C[0], 2, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	printf(" 3: REDUCE en el Proceso%d C = %d %d \n", rank, C[0], C[1]);
	MPI_Barrier(MPI_COMM_WORLD);
	//Suma de los 4 vectores B (2 elementos) en el vector C en todos los Procesos
	MPI_Allreduce(&B[0], &C[0], 2, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	printf(" 4: ALLREDUCE en el Proceso%d C = %d %d \n", rank, C[0], C[1]);
	MPI_Barrier(MPI_COMM_WORLD);
	//Recoleccion de los vectores B en el vector A en Proceso 0 
	for(i=0; i<2; i++) B[i] = B[i] + 10;
	MPI_Gather(&B[0], 2, MPI_INT, &A[0], 2, MPI_INT, 0, MPI_COMM_WORLD);
	printf(" 5: GATHER en el Proceso%d, A = %d %d %d %d %d %d %d %d\n",rank,
	A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7]);
	MPI_Barrier(MPI_COMM_WORLD);
	// Recoleccion de los vectores B en el vector A en todos los Procesos
	MPI_Allgather(&B[0], 2, MPI_INT, &A[0], 2, MPI_INT, MPI_COMM_WORLD);
	printf(" 6: ALLGATHER en el Proceso%d, A = %d %d %d %d %d %d %d %d\n",rank,
	A[0],A[1],A[2],A[3],A[4],A[5],A[6],A[7]);
	MPI_Finalize();
	return 0;
}
