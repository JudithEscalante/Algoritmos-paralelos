#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <omp.h>
#include <time.h>
using namespace std;

int find_pos(vector<float> pos, float num)
{
   for (int i=0;i<pos.size();++i)
   {
      if(num <= pos[i])
      {
        return i;
      }

   }
}
int find_pos_parallel(vector<float> pos, float num)
{

   int pos_i = 0;
   int cont=0;
   #pragma omp parallel for shared( pos_i,cont)
   for (int i=0;i<pos.size();++i)
   {
      if(num <= pos[i] && cont<1)
      {
        pos_i=i;
        cont++;
      }

   }
   return pos_i;
}
vector<int> serial_histogram(int count_num,float maximo,float minimo,float bin_num)
{
    vector<float> data;
    vector<float>posicion;
    vector<int>contador;
    for (int g=0;g<bin_num;++g)
    {
        posicion.push_back(0.0);
        contador.push_back(0);
    }
    for (int i=0;i<count_num;++i)
    {
        //float r= static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/maximo));
        float r = minimo + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maximo-minimo)));
        data.push_back(r);
    }
    auto min_o = min_element(begin(data),end(data));
    auto max_o = max_element(begin(data),end(data));
    //cout<<"minimo: "<<*min_o<<" "<<"maximo: "<<*max_o<<endl;

    float bin_width = (*max_o - *min_o)/ bin_num;
    //cout<<"intervalo: "<<bin_width<<endl;
     //cout<<"minimo: "<<*min_o<<" "<<"maximo: "<<*max_o<<" "<<"intervalo: "<<bin_width<<endl;
    for (int b = 0; b < bin_num ; ++b)
    {
       posicion[b] = *min_o+bin_width*(b+1.0);
        //cout<<"limite: "<<posicion[b]<<" ";
    }
    //cout<<endl;
    for (int s=0;s<count_num;++s)
    {
        //cout<<"elemento: "<< s <<" contenido: " << data[s] <<endl;
       int pos_bin=find_pos(posicion,data[s]);
       //cout<<pos_bin<<"posicion"<<endl;
       contador[pos_bin]=contador[pos_bin]+1;

    }
    //cout<<endl;
    return contador;

}

vector<int> parallel_histogram(int count_num,float maximo,float minimo,float bin_num, int nthreads )
{
    vector<float> data;
    vector<float>posicion;
    vector<int>contador;
    int thread;
    omp_set_num_threads(nthreads);
    #pragma omp parallel private(thread)
    {
         thread = omp_get_thread_num();
         #pragma omp sections
         {
            //#pragma omp for schedule( static, bin_num/nthreads )
            #pragma omp section
            for (int i=0;i<bin_num;++i)
            {
                //cout<<"es hecho por la hebra (posicion,contador):"<<thread <<endl;
                float e=0.0;
                int d=0;
                posicion.push_back(e);
                contador.push_back(d);
            }

            //#pragma omp for schedule( static, count_num/nthreads )
            #pragma omp section
            for (int i=0;i<count_num;++i)
            {
                //float r= static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/maximo));
                float r = minimo + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(maximo-minimo)));
                 //cout<<"elemento: "<< i <<" contenido: " << r<<endl; 
                data.push_back(r);
            }
           

         }
         

    }
    auto min_o = min_element(begin(data),end(data));
    auto max_o = max_element(begin(data),end(data));
    float bin_width = (*max_o - *min_o)/ bin_num;
    // cout<<"minimo: "<<*min_o<<" "<<"maximo: "<<*max_o<<" "<<"intervalo: "<<bin_width<<endl<<endl;
    #pragma omp parallel 
    {
       #pragma omp for schedule( auto )
       for (int b = 0; b < (int)bin_num ; ++b)
       {
         float interval=*min_o+bin_width;
         float increment=1.0;
         posicion[b] = interval*(b+increment);
       }
    }
    //for (int t=0;t<(int)bin_num ; ++t)
    //{
        //cout<<"limite: "<<posicion[t]<<" ";
    //}
    //cout<<endl;
    #pragma omp parallel 
    {
        #pragma omp for schedule( auto )
        for (int s=0;s<count_num;++s)
        {
            int pos_bin=find_pos_parallel(posicion,data[s]);
            contador[pos_bin]=contador[pos_bin]+1;
        }
    }

    //cout<<endl;
     return contador;

}
int main()
{
    double timeIni1, timeFin1,timeIni_par,timeFin_par;
    timeIni1 = omp_get_wtime();
    cout<<"-------------PROGRAMA EN FORMA SERIAL-------------"<<endl;
    /*vector<int> nuevo1=serial_histogram(10,8.0,0.0,6.0);
    for (int s=0;s<nuevo1.size();++s)
    {
       cout<<"contador: "<<nuevo1[s]<<" ";
    }*/
    serial_histogram(3000000,8.0,0.0,6.0);
    timeFin1 = omp_get_wtime();
    cout<<"Tiempo del programa serial = "<< timeFin1 - timeIni1 <<" segundos"<<endl;
    cout<<"-------------PROGRAMA EN FORMA PARALELA-------------"<<endl;
    timeIni_par = omp_get_wtime();
    /*vector<int> nuevo=parallel_histogram(10,8.0,0.0,6.0,10);
    for (int s=0;s<nuevo.size();++s)
    {
       cout<<"contador: "<<nuevo[s]<<" ";
    }
    cout<<endl;*/
    parallel_histogram(3000000,8.0,0.0,6.0,10);
    timeFin_par = omp_get_wtime();
    cout<<"Tiempo del programa paralelo = "<< timeFin_par - timeIni_par <<" segundos"<<endl;

}
