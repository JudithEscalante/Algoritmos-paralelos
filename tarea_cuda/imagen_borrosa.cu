#include <stdio.h>
#include <fstream>
#include <iostream>
#define BLUR_SIZE 1
using namespace std;

__global__
void Kernel_bor(float * in, float * out, int w, int h)
{
  int Col = blockIdx.x * blockDim.x + threadIdx.x;
  int Row = blockIdx.y * blockDim.y + threadIdx.y;
  if (Col < w && Row < h)
  {
    int pixVal = 0;
    int pixels = 0;
    
    for(int blurRow = -BLUR_SIZE; blurRow < BLUR_SIZE+1; ++blurRow)
    {
      for(int blurCol = -BLUR_SIZE; blurCol < BLUR_SIZE+1; ++blurCol)
      {
        int curRow = Row + blurRow;
        int curCol = Col + blurCol;
        if(curRow > -1 && curRow < h && curCol > -1 && curCol < w)
        {
          pixVal += in[curRow * w + curCol];
          pixels++; 
        }
      }
    }
   
    out[Row * w + Col] = (float)(pixVal / pixels);
  }
}

void guardar(float r[225][225], float g[225][225], float b[225][225])
{
  ofstream archivo("toy_bor.dat");
  for (int i = 0; i < 225; ++i)
  {
    for (int j = 0; j < 225; ++j)
    {
          archivo<<r[i][j]<<" "<<g[i][j]<<" "<<b[i][j]<<" ";
    }
    archivo<<endl;
  }
}

void borroso(float r[225][225], float g[225][225], float b[225][225], int width, int height)
{
  float o_r[225][225];
  float o_g[225][225];
  float o_b[225][225];

  int size = width * height;
  int memSize = size * sizeof(float);

  float *d_A, *d_B;

  cudaMalloc((void **) &d_A, memSize);
  cudaMalloc((void **) &d_B, memSize);
  cudaMemcpy(d_A, r, memSize, cudaMemcpyHostToDevice);


  dim3 DimGrid(floor((width-1)/16 + 1), floor((height-1)/16+1), 1);
  dim3 DimBlock(16, 16, 1);
  Kernel_bor<<<DimGrid,DimBlock>>>(d_A, d_B, width, height);

  cudaMemcpy(o_r, d_B, memSize, cudaMemcpyDeviceToHost);
  Kernel_bor<<<DimGrid,DimBlock>>>(d_A, d_B, width, height);
  cudaMemcpy(o_g, d_B, memSize, cudaMemcpyDeviceToHost);
  cudaMemcpy(d_A, b, memSize, cudaMemcpyHostToDevice);

  Kernel_bor<<<DimGrid,DimBlock>>>(d_A, d_B, width, height);
  
  cudaMemcpy(o_b, d_B, memSize, cudaMemcpyDeviceToHost);
  cudaFree(d_A);
  cudaFree(d_B);
  guardar(o_r,o_g,o_b);
}

void llenar(const char *file, float r[225][225], float g[225][225], float b[225][225])
{
  char buffer[100];
  ifstream archivo2(file);
  for (int ii = 0; ii < 225; ++ii)
  {
    for (int jj = 0; jj < 225; ++jj)
    {
          archivo2>>r[ii][jj]>>g[ii][jj]>>b[ii][jj];
    }
    archivo2.getline(buffer,100);
  }
}

int main()
{
  int width=225, height=225;
  float r[225][225];
  float g[225][225];
  float b[225][225];
  llenar("toy.dat",r,g,b);
  borroso(r,g,b,width,height);
  return EXIT_SUCCESS;
}