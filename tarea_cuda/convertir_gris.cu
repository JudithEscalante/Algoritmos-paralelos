#include <stdio.h>
#include <fstream>
#include <iostream>
#define CHANNELS 3 
using namespace std;

__global__
void convertir_grises(float * Pout, float * Pin, int width, int height)
{
  int Col = threadIdx.x + blockIdx.x * blockDim.x;
  int Row = threadIdx.y + blockIdx.y * blockDim.y;
  if (Col < width && Row < height)
  {
   
    int greyOffset = Row*width + Col;
    int rgbOffset = greyOffset*CHANNELS;
    float r = Pin[rgbOffset]; 
    float g = Pin[rgbOffset + 1];
    float b = Pin[rgbOffset + 2]; 
    Pout[greyOffset] = 0.21f*r + 0.71f*g + 0.07f*b;
  }
}

void guardar(float o[225][225])
{
  ofstream archivo("toy_gray.dat");
  for (int i = 0; i < 225; ++i)
  {
    for (int j = 0; j < 225; ++j)
    {
          archivo<<o[i][j]<<" ";
    }
    archivo<<endl;
  }
}
void llenar(const char *file, float m[225][225*3])
{
  char buffer[100];
  ifstream archivo2("toy.dat");
  for (int ii = 0; ii < 225; ++ii)
  {
    for (int jj = 0; jj < 225; ++jj)
    {
          archivo2>>m[ii][jj*3]>>m[ii][jj*3+1]>>m[ii][jj*3+2];
    }
    archivo2.getline(buffer,100);
  }
}

void Escala(float m[225][225*3],int width, int height)
{
  float o[225][225];

  int size_in = width * (height*3);
  int size_out = width * height;
  int memSize_in = size_in * sizeof(float);
  int memSize_out = size_out * sizeof(float);

  float *d_A, *d_B;

  cudaMalloc((void **) &d_A, memSize_in);
  cudaMalloc((void **) &d_B, memSize_out);
  cudaMemcpy(d_A, m, memSize_in, cudaMemcpyHostToDevice);

 
  dim3 DimGrid(floor((width-1)/16 + 1), floor((height-1)/16+1), 1);
  dim3 DimBlock(16, 16, 1);
  convertir_grises<<<DimGrid,DimBlock>>>(d_B, d_A, width, height);
  cudaMemcpy(o, d_B, memSize_out, cudaMemcpyDeviceToHost);
  cudaFree(d_A);
  cudaFree(d_B);
  guardar(o);
}




int main()
{
  int width=225, height=225;
  float m[225][225*3];
  llenar("toy.dat",m);
  Escala(m,width,height);
  return EXIT_SUCCESS;
}