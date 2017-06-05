#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <omp.h>
#include <time.h>
using namespace std;     
int juego(int num_lanzamientos)
{
    long double pi, x, y;
    int puntaje = 0;
    double minimo=-1.0,maximo=1.0;
    for (int i = 0; i < num_lanzamientos; i++)
    {
        x = minimo + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maximo-minimo)));
        y = minimo + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maximo-minimo)));
        if (x*x + y*y < 1.0)
            puntaje++;
    }
    return puntaje;
}

int main()
{
   

    long double pi,pi2;
    long double pi_original = 3.141592653589;
    int puntaje = 0, num_lanzamientos;
    int num_dardos=8000000,jugadores=10; 
    num_lanzamientos = num_dardos / jugadores;
    double Ini1 = omp_get_wtime();
    #pragma omp parallel for shared( puntaje)
    for (int i = 1; i <= jugadores; i++)
         puntaje += juego(num_lanzamientos);
    double Fin1 = omp_get_wtime();
    pi = 4.0 * ((long double)puntaje / (long double)num_dardos);
    
    
    cout<<"-------------PROGRAMA EN FORMA PARALELA-------------"<<endl;
    cout<<"PI calculado: "<<pi<<" PI real: "<<pi_original<<" error: "<<pi_original-pi<<endl;
    cout<<"Tiempo del programa paralelos: "<<Fin1-Ini1<<endl;


    double Ini2 = omp_get_wtime();
    pi2 = 4.0 * ((long double) juego(num_dardos) / (long double)num_dardos);
    double Fin2 = omp_get_wtime();
    cout<<"-------------PROGRAMA EN FORMA SERIAL-------------"<<endl;
    cout<<"PI calculado: "<<pi2<<" PI real: "<<pi_original<<" error: "<<pi_original-pi<<endl;
    cout<<"Tiempo del programa serial: "<<Fin2-Ini2<<endl;
}