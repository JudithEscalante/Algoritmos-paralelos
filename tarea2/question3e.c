#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#define tam 1024
int main( int argc, char **argv )
{
    double a[tam][tam], b[tam], cpart[tam], ctotal[tam];
    int rank, i, k,nlocal, n=tam,j,npes;
    int tInicio, tFin,result; 

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &npes );


    for (i=0;i<tam ;i++)
    {
      for (k=0;k<tam ;k++) 
        a[i][k] = i;
    }

    
    for (k=0;k<tam;k++) 
    {
      b[k] = k+1;
    }

    tInicio = MPI_Wtime();
   
    nlocal = n/npes; 
    MPI_Allgather(b, nlocal, MPI_DOUBLE, cpart, nlocal, MPI_DOUBLE, MPI_COMM_WORLD); 

     /* Perform the matrix-vector multiplication involving the locally stored submatrix */ 
     for (i=0; i<nlocal; i++) 
     { 
       ctotal[i] = 0.0; 
       for (j=0; j<n; j++) 
         ctotal[i] += a[i][n+j]*cpart[j]; 
     } 
    tFin = MPI_Wtime();
    result=tFin - tInicio;
    printf(" %d ", result);
    

    MPI_Finalize();
    return 0;
}
// Tabla 3.5 
/* 
   comm_sz |   1024      |    2048     |   4096      |   8192     |   16384     |  
      1    |3.736019e-03 |1.496005e-02 |6.107616e-02 |2.405140e-01|3.472693e+00 |            
      2    |2.044916e-03 |8.082151e-03 |3.199005e-02 |1.282492e-01|5.230219e-01 |           
      4    |2.053976e-03 |8.048058e-03 |3.205419e-02 |1.286149e-01|5.068250e-01 |        
      8    |3.664017e-03 |8.631945e-03 |3.245521e-02 |1.570139e-01|5.209410e-01 |           
      16   |2.790928e-03 |1.090097e-02 |4.284501e-02 |1.408250e-01|5.279069e-01 |           

*/
// Tabla 3.6
/* 
   comm_sz |   1024      |    2048     |   4096      |   8192     |   16384     |  
      1    |           1 |           1 |           1 |           1|           1 |            
      2    |       1,824 |       0,185 |       1,909 |       1,875|      0,6639 |
      4    |       1,824 |      0,1858 |       1,9057|      1,0925|      0,6850 |        
      8    |       1,019 |       0,173 |       1,8817|       0,894|      0,6664 |           
      16   |      1,3405 |       1,372 |       1,425 |      0,9978|      0,6577 |           
  
*/

// Tabla 3.7
/* 
   comm_sz |   1024      |    2048     |   4096      |   8192     |   16384     |  
      1    |           1 |           1 |           1 |           1|           1 |            
      2    |       0,912 |      0,0925 |       0,9545|      0,9375|      0,3319 |
      4    |       0,456 |      0,0464 |       0,4764|      1,0925|      0,1712 |        
      8    |       0,127 |      0,0216 |       0,2352|      0,2731|      0,0833 |           
      16   |      0,0837 |      0,0857 |     0,08906 |     0,0623 |     0,041106|           
  
*/