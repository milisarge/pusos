#include <stdio.h>
#include <stdlib.h>

void allocate_mem(double*** arr, int rows, int cols);
void deallocate_mem(double*** arr, int n);
void printMatrix(double** a, int rows, int cols);
void printMatrixE(double** a, int rows, int cols);
void multMatrixp(double **A, double **B, double **C,int r1,int c1,int r2,int c2);

void allocate_mem(double*** arr, int rows, int cols)
{
  int i;
  *arr = (double**)malloc(rows*sizeof(double*));
  for( i=0; i<rows; i++)
    (*arr)[i] = (double*)malloc(cols*sizeof(double));
} 

//______________________________________________________________________________
void deallocate_mem(double*** arr, int rows){
 int i;
    for (i = 0; i < rows; i++)
        free((*arr)[i]);
    free(*arr); 
}

void printMatrix(double** a, int rows, int cols)
{
    int i, j;
   printf("Matrix[%d][%d]\n",rows,cols);    
   for(i=0;i<rows;i++){
      for(j=0;j<cols;j++)
      printf("%8.3lf ",a[i][j]);
      printf("\n");
   }
   printf("\n");   
}


//______________________________________________________________________________

void multMatrixp(double **A, double **B, double **C,int ro1,int co1,int ro2,int co2)
{
    int i, j, k;
    for(i = 0; i < ro1; i++) {
        for(j = 0; j < co2; j++) {
            C[i][j] = 0;
            for(k = 0; k < co1; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main(){

	int ro1, co1, ro2, co2;
	double **a1, **b1, **c1;    

	ro1=2; co1=3;
	ro2=3; co2=4;

	double (*(a[])) = {
	(double[]) { 1, 3, 5},
	(double[]) {2, 4, 0}};


	double (*(b[])) = {
	(double[]) {6, 2, 4, 8},
	(double[]) {1, 7, 0, 9},
	(double[]) {0, 3, 5, 1}};


	printMatrix(a,ro1,co1);    
	printMatrix(b,ro2,co2);


	printf("MatMult \n");
	double **c;
	allocate_mem(&c,ro1,co2);
	multMatrixp(a, b, c, ro1, co1, ro2, co2);  
	printMatrix(c,ro1,co2);           

	deallocate_mem(&c,ro1);  

	return 0;
}
