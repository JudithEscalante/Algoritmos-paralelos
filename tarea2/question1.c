#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
double funcion(double x)
{
  double y;
  //Modificar la funcion y=f(x)
  y = x;
  return y;
}
double Trapecio(double p_izquierdo,double p_derecho,int contador,double incremento)
{
  double estimado,temp;
  int i;
  estimado=(funcion(p_izquierdo)+funcion(p_derecho))/2.0;
  for(i=1 ;i<contador;i++)
  {
    temp=p_izquierdo+i*incremento;
    estimado+=funcion(temp);
  }
  estimado=estimado*incremento;
  return estimado;
}

int main(int argc, char** argv) 
{
  int world_rank,world_size,local_n,source,n=1024;
  double local_int,total_int, a=0.0,b=3.0,h,local_a,local_b;
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  h=(b-a)/n;
  local_n=n/world_size;
  local_a= a + world_rank * local_n * h;
  local_b= local_a + local_n*h;
  local_int=Trapecio(local_a,local_b,local_n,h);
  
  if (world_rank != 0)
  {
     MPI_Send(&local_int, 1, MPI_DOUBLE,0, 0, MPI_COMM_WORLD);
  } 
  else 
  {
     total_int=local_int;
     for (source=1;source<world_size;source++)
     {
        MPI_Recv(&local_int, 1, MPI_DOUBLE,source, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        total_int+=local_int;
     }
       
  }
  if(world_rank==0)
  {
    printf("Con n= %d trapezoides, nuestro estimado de la integral desde %f hasta %f es = %.15e\n",n,a,b,total_int);
  }
  
  MPI_Finalize();
  
  return 0;
}
