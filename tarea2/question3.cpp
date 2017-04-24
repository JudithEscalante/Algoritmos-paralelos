#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>
using namespace std;
int main(int argc, char * argv[]) 
{
    int npr,proceso;
    long **A,*x,*y, *miFila, *comprueba;               
    double tInicio, tFin; 
 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &npr);
    MPI_Comm_rank(MPI_COMM_WORLD, &proceso);
 
    A = new long *[npr]; 
    x = new long [npr]; 
    
    if (proceso == 0) 
    {
        A[0] = new long [npr * npr];
        for (int i = 1; i < npr; i++) 
        {
            A[i] = A[i - 1] + npr;
        }
        y = new long [npr];
        srand(time(0));
        cout << "La matriz y el vector generados son " << endl;
        for ( int i = 0; i < npr; i++) 
        {
            for ( int j = 0; j < npr; j++) 
            {
                if (j == 0) 
                    cout << "[";
                A[i][j] = rand() % 100;
                cout << A[i][j];
                if (j == npr - 1) 
                    cout << "]";
                else cout << "  ";
            }
            x[i] = rand() % 100;
            cout << "\t  [" << x[i] << "]" << endl;
        }
        cout << "\n";
 
        comprueba = new long [npr];
        for (unsigned int i = 0; i < npr; i++) 
        {
            comprueba[i] = 0;
            for (unsigned int j = 0; j < npr; j++) 
            {
                comprueba[i] += A[i][j] * x[j];
            }
        }
    } 
 
    
    miFila = new long [npr];
 
   
    MPI_Scatter(A[0], npr, MPI_LONG, miFila, npr, MPI_LONG, 0, MPI_COMM_WORLD); 

    MPI_Bcast(x, npr, MPI_LONG, 0,MPI_COMM_WORLD); 
 
    MPI_Barrier(MPI_COMM_WORLD);

    tInicio = MPI_Wtime();
 
    long subFinal = 0;
    for (int i = 0; i < npr; i++) 
    {
        subFinal += miFila[i] * x[i];
    }
 
    MPI_Barrier(MPI_COMM_WORLD);
    tFin = MPI_Wtime();
 
    MPI_Gather(&subFinal, 1, MPI_LONG,  y, 1, MPI_LONG,0, MPI_COMM_WORLD); 
    
 
    if (proceso == 0) {
 
        int errores = 0;
        cout << "El resultado obtenido y el esperado son:" << endl;
        for (int i = 0; i < npr; i++) {
            cout << "\t" << y[i] << "\t|\t" << comprueba[i] << endl;
            if (comprueba[i] != y[i])
                errores++;
        }
 
        delete [] y;
        delete [] comprueba;
        delete [] A[0];
 
        if (errores) {
            cout << "Hubo " << errores << " errores." << endl;
        } else {
            cout << "No hubo errores" << endl;
            cout << "El tiempo tardado ha sido " << tFin - tInicio << " segundos." << endl;
        }
 
    }
 
    delete [] x;
    delete [] A;
    delete [] miFila;

    MPI_Finalize();
 
}