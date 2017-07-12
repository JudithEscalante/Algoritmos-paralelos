#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define TILED 16


__global__ void matrix_mult(int *a,int *b, int *c, int m, int n, int k)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;
    if( col < k && row < m)
    {
        for(int i = 0; i < n; i++)
        {
            sum += a[row * n + i] * b[i * k + col];
        }
        c[row * k + col] = sum;
    }
}


__global__ void matrix_mult_tiled(float*A, float*B, float*C,int ARows, int ACols, int BRows, int BCols, int CRows,int CCols) 
{

  float CValue = 0;
  int Row = blockIdx.y*TILE_DIM + threadIdx.y;
  int Col = blockIdx.x*TILE_DIM + threadIdx.x;

  __shared__ float As[TILE_DIM][TILE_DIM];
  __shared__ float Bs[TILE_DIM][TILE_DIM];

  for (int k = 0; k < (TILE_DIM + ACols - 1)/TILE_DIM; k++) 
    {

      if (k*TILE_DIM + threadIdx.x < ACols && Row < ARows) 
         As[threadIdx.y][threadIdx.x] =A[Row*ACols + k*TILE_DIM + threadIdx.x];
      else
         As[threadIdx.y][threadIdx.x] = 0.0;

      if (k*TILE_DIM + threadIdx.y < BRows && Col < BCols)  
         Bs[threadIdx.y][threadIdx.x] = B[(k*TILE_DIM + threadIdx.y)*BCols + Col];
      else 
         Bs[threadIdx.y][threadIdx.x] = 0.0;

      __syncthreads();

     for (int n = 0; n < TILE_DIM; ++n) 
        CValue += As[threadIdx.y][n] * Bs[n][threadIdx.x];

      __syncthreads();

  }

  if (Row < CRows && Col < CCols) 
    C[((blockIdx.y * blockDim.y + threadIdx.y)*CCols)+(blockIdx.x*blockDim.x)+threadIdx.x]=CValue;

}

__global__ void matrix_mult_tiled1(float*A, float*B, float*C, int ARows, int ACols,int BRows, int BCols, int CRows, int CCols) 
                                  
{

 float CValue = 0 , CValue2=0;
 int Row = blockIdx.y*TILE_DIM + threadIdx.y;
 int Col = blockIdx.x*TILE_DIM + threadIdx.x;

  __shared__ float As[TILE_DIM][TILE_DIM];
  __shared__ float Bs[TILE_DIM][TILE_DIM];
  __shared__ float Bs_sub[TILE_DIM][TILE_DIM];
  
  for (int k = 0; k < (TILE_DIM + ACols - 1)/TILE_DIM; k++) 
  {
     if (k*TILE_DIM + threadIdx.x < ACols && Row < ARows)
        As[threadIdx.y][threadIdx.x] =  A[Row*ACols + k*TILE_DIM + threadIdx.x];
        
      else As[threadIdx.y][threadIdx.x] = 0.0;

      if (k*TILE_DIM + threadIdx.y < BRows && Col < BCols)  
      {
        Bs[threadIdx.y][threadIdx.x] =  B[(k*TILE_DIM + threadIdx.y)*BCols + Col];
        
        Bs_sub[threadIdx.y][threadIdx.x] =  B[(k*TILE_DIM + threadIdx.y)*BCols +(Col+TILE_DIM)];
        
      } 
      else 
      {
        Bs[threadIdx.y][threadIdx.x] = 0.0;
        Bs_sub[threadIdx.y][threadIdx.x] =0.0;
      }
      

      __syncthreads();

      for (int n = 0; n < TILE_DIM; ++n)
      {
        CValue +=As[threadIdx.y][n]*Bs[n][threadIdx.x];
        CValue2+=As[threadIdx.y][n]*Bs_sub[n][threadIdx.x]
      }
      

      __syncthreads();

  }

  if (Row < CRows && Col < CCols) 
  C[((blockIdx.y * blockDim.y + threadIdx.y)*CCols)+ (blockIdx.x*blockDim.x)+threadIdx.x]= CValue;
  
  C[(((blockIdx.y * blockDim.y + threadIdx.y)*CCols)+(blockIdx.x*blockDim.x)+threadIdx.x)+ TILE_DIM]= CValue2;

}



void cpu_matrix_mult(int *h_a, int *h_b, int *h_result, int m, int n, int k) {
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < k; ++j)
        {
            int tmp = 0.0;
            for (int h = 0; h < n; ++h)
            {
                tmp += h_a[i * n + h] * h_b[h * k + j];
            }
            h_result[i * k + j] = tmp;
        }
    }
}


int main(int argc, char const *argv[])
{
    int m, n, k;

    srand(3333);
    printf("Ingrese dimensiones :  m n and k\n");
    scanf("%d %d %d", &m, &n, &k);


    int *h_a, *h_b, *h_c, *h_cc;
    cudaMallocHost((void **) &h_a, sizeof(int)*m*n);
    cudaMallocHost((void **) &h_b, sizeof(int)*n*k);
    cudaMallocHost((void **) &h_c, sizeof(int)*m*k);
    cudaMallocHost((void **) &h_cc, sizeof(int)*m*k);


    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            h_a[i * n + j] = rand() % 1024;
        }
    }


    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < k; ++j) {
            h_b[i * k + j] = rand() % 1024;
        }
    }

    float gpu_elapsed_time_ms, cpu_elapsed_time_ms;


    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);


    cudaEventRecord(start, 0);

    int *d_a, *d_b, *d_c;
    cudaMalloc((void **) &d_a, sizeof(int)*m*n);
    cudaMalloc((void **) &d_b, sizeof(int)*n*k);
    cudaMalloc((void **) &d_c, sizeof(int)*m*k);


    cudaMemcpy(d_a, h_a, sizeof(int)*m*n, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, h_b, sizeof(int)*n*k, cudaMemcpyHostToDevice);

    unsigned int grid_rows = (m + TILED - 1) / TILED;
    unsigned int grid_cols = (k + TILED - 1) / TILED;
    dim3 dimGrid(grid_cols, grid_rows);
    dim3 dimBlock(TILED, TILED);



    //matrix_mult_tiled<<<dimGrid, dimBlock>>>(d_a, d_b, d_c, n);

    matrix_mult_tiled1<<<dimGrid, dimBlock>>>(d_a, d_b, d_c, m, n, k);


    cudaMemcpy(h_c, d_c, sizeof(int)*m*k, cudaMemcpyDeviceToHost);
    cudaThreadSynchronize();

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);


    cudaEventElapsedTime(&gpu_elapsed_time_ms, start, stop);
    printf("Tiempo en  %dx%d . %dx%d con tiled: %f ms.\n\n", m, n, n, k, gpu_elapsed_time_ms);


    cudaEventRecord(start, 0);

    cpu_matrix_mult(h_a, h_b, h_cc, m, n, k);

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&cpu_elapsed_time_ms, start, stop);
    printf("Tiempo %dx%d . %dx%d en CPU: %f ms.\n\n", m, n, n, k, cpu_elapsed_time_ms);

    //printf("all results are correct!!!, speedup = %f\n", cpu_elapsed_time_ms );

    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
    cudaFreeHost(h_a);
    cudaFreeHost(h_b);
    cudaFreeHost(h_c);
    cudaFreeHost(h_cc);
    return 0;
}