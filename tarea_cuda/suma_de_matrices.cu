#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <armadillo>
#include <vector>
#include <string>
#include <cuda_runtime.h>
#include <cuda.h>
using namespace std;

__global__ void suma(float *A, float *B, float *C, int N){
    // Las matrices se recorren con la ordenacion de Fortran
    int j = blockIdx.x * blockDim.x + threadIdx.x; // indice filas
    int i = blockIdx.y * blockDim.y + threadIdx.y; // indice columnas
    int tid = (i * N) + j;

    if(i < N && j < N)
        C[tid] = A[tid] + B[tid];
}

void fill(float *h_A, float *h_B, float *h_C, int N){
    for(int i = 0; i<N; i++){
        int row = N*i;
        for(int j = 0; j<N; j++){
            h_A[row+j] = row+j+1;
            h_B[row+j] =  row+j+2;
            h_C[row+j] = 0;
        }
    }
}

void parallel_matrix_sum(float *h_A, float *h_B, float *h_C, int N, int numBloques, int numThreadsBloque){
    unsigned int sizeMatrix = N * N;
    unsigned int memSize = sizeMatrix * sizeof(float);
    float * d_A ,* d_B,* d_C;

    cudaMalloc((void **)&d_A, memSize);
    cudaMalloc((void **)&d_B, memSize);
    cudaMalloc((void **)&d_C, memSize);


    cudaMemcpy(d_A, h_A, memSize, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, memSize, cudaMemcpyHostToDevice);


    cout << "GPU: Calculando..." << endl;
    dim3 threadsPerBlock(numThreadsBloque,numThreadsBloque);
    dim3 numBlocks (numBloques, numThreadsBloque);
    suma<<<numBlocks, threadsPerBlock>>> (d_A, d_B, d_C, N);

    cudaMemcpy(h_C, d_C, memSize, cudaMemcpyDeviceToHost);


    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);


int main(void)
{
   float *h_A,*h_B,*h_C;
   int N=1000;
   size_t size=N*sizeof(float);
   h_A = (float *) malloc (size);
   h_B = (float *) malloc (size);
   h_C = (float *) malloc (size);
   fill(*h_A, *h_B,*h_C, int N);
   int numBloques=8;
   int numThreadsBloque=N/numBloques + (N%numBloques ==0 ? 0:1);
   parallel_matrix_sum(*h_A, *h_B, *h_C, N,  numBloques,  numThreadsBloque)
   return(0);



}