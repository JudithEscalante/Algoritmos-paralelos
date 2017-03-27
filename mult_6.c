#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include <sys/time.h>

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

void mult_6(int n, int m, int p)
{	srand(time(NULL));
	//int a[n][m];
	//int b[m][p];
	int **c,**a,**b;
	int i1, i2,j1,j2,k1,k2,l;
	float m_=32768.0;
	int t=round(sqrt(m));

	c = (int **)malloc(n*sizeof(int*)); 
	a = (int **)malloc(n*sizeof(int*)); 
	b = (int **)malloc(m*sizeof(int*)); 
	
	for (l=0;l<n;l++) 
		a[l] = (int*)malloc(m*sizeof(int)); 

	for (l=0;l<m;l++) 
		b[l] = (int*)malloc(p*sizeof(int)); 

	for (l=0;l<n;l++) 
		c[l] = (int*)malloc(p*sizeof(int)); 

	for (i1=0;i1<n;++i1){
		for (j1=0;j1<m;++j1){
			a[i1][j1]=rand()%20;
		}
	}
	for (i2=0;i2<m;++i2){
		for (j2=0;j2<p;++j2){
			b[i2][j2]=rand()%20;
		}
	}

	for (i1=0;i1<n;i1+=t){
		for (j1=0;j1<p;j1+=t){
			for (k1=0;k1<m;k1+=t){
				for (i2=i1;i2<min(i1+t,n);++i2){
					for (j2=j1;j2<min(j1+t,p);++j2){
						int sum=0;
						for (k2=k1;k2<min(k1+t,m);++k2){
							sum=sum+a[i2][k2]*b[k2][j2];
						}
						c[i2][j2]=sum;
					}
				}
			}
		}
	}

	printf("%i\n",c[232][232]);

	for (l=0;l<n;l++) {
		free(c[l]);
		free(a[l]);
	}
	free(c);
	free(a);

	for (l=0;l<m;l++) {
		free(b[l]);
	}
	free(b);
}

int main(int ac, char **av){	
	int n=2000;
	int p=2000;
	int m=2000;
	
	//time_t stop, start;
	//start=time(NULL);
	
	mult_6(n,m,p);
	
	//stop=time(NULL);
	//printf("took %lu\n", stop-start);

	exit(0);
}
