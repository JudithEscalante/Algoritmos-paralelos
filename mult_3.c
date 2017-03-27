#include <time.h>
#include <stdlib.h>
#include <stdio.h>

void mult_3(int n, int m, int p)
{	srand(time(NULL));
	//int a[n][m];
	//int b[m][p];
	//int c[n][p];
	int **c,**b,**a;
	int i1, i2,i,j1,j2,j,k,l;
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
	


	for(i=0;i<n;++i){
		for(j=0;j<p;++j){
			int sum=0;
			for (k=0;k<m;++k)
				sum=sum+a[i][k]*b[k][j];
			c[i][j]=sum;
		}
	}
	//return c;
	printf("%i\n",c[253][253]);
	
	
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
	
	mult_3(n,m,p);

	exit(0);
}
