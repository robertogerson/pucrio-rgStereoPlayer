/**
 *	@file m3.c TAD Matrix 3x3 implementation.
 *
 *
 *	@author 
 *			- Marcelo Gattass
 *
 *	@date
 *			Criado em:		 20 de Maio de 2006
 *			Última Modificação:	01 de Maio de 2011
 *
 *	@version 1.0
 */

#include <stdio.h>
#include <math.h>

double m3Det(double* A)
{
   return A[0] * A[4] * A[8] + 
	      A[1] * A[5] * A[6] +
		  A[2] * A[3] * A[7] -
		  A[6] * A[4] * A[2] -
		  A[7] * A[5] * A[0] -
		  A[8] * A[3] * A[1];
}

#define TOL 1.E-8

double m3Inv(double* A, double* Ainv)
{
   double det = m3Det (A);
   
   if (fabs(det) < TOL)
	   return det;
     
   Ainv[0] =  (A[4] * A[8] - A[7] * A[5]) / det;
   Ainv[1] = -(A[1] * A[8] - A[7] * A[2]) / det;
   Ainv[2] =  (A[1] * A[5] - A[4] * A[2]) / det;

   Ainv[3] = -(A[3] * A[8] - A[6] * A[5]) / det;
   Ainv[4] =  (A[0] * A[8] - A[6] * A[2]) / det;
   Ainv[5] = -(A[0] * A[5] - A[3] * A[2]) / det;

   Ainv[6] =  (A[3] * A[7] - A[6] * A[4]) / det;
   Ainv[7] = -(A[0] * A[7] - A[6] * A[1]) / det;
   Ainv[8]=   (A[0] * A[4] - A[3] * A[1]) / det;

   return det;
}

void m3MultAB(double *A, double *B, double *AB)
{
   AB[0] = A[0] * B[0] + A[1] * B[3] + A[2] * B[6];
   AB[1] = A[0] * B[1] + A[1] * B[4] + A[2] * B[7];
   AB[2] = A[0] * B[2] + A[1] * B[5] + A[2] * B[8];

   AB[3] = A[3] * B[0] + A[4] * B[3] + A[5] * B[6];
   AB[4] = A[3] * B[1] + A[4] * B[4] + A[5] * B[7];
   AB[5] = A[3] * B[2] + A[4] * B[5] + A[5] * B[8];

   AB[6] = A[6] * B[0] + A[7] * B[3] + A[8] * B[6];
   AB[7] = A[6] * B[1] + A[7] * B[4] + A[8] * B[7];
   AB[8] = A[6] * B[2] + A[7] * B[5] + A[8] * B[8];
}

void m3MultAb(double *A, double *b, double *x)
{
   x[0] = A[0] * b[0] + A[1] * b[1] + A[2] * b[2];
   x[1] = A[3] * b[0] + A[4] * b[1] + A[5] * b[2];
   x[2] = A[6] * b[0] + A[7] * b[1] + A[8] * b[2];
}

double m3SolvAxb(double *A, double *b, double *x)
{
   double Ainv[9] = {1,0,0, 0,1,0, 0,0,1};
   double det = m3Inv(A, Ainv);
   m3MultAb(Ainv, b, x);
   return det;
}

int m3Homography4p( double *u, double *v, double *x, double *y, 
                  double *H4p)
{
    double N[3*3]; /* [N] transforms the rectangle given by (X,Y) in a unity 
                      square */
    double Linv[3*3]; 
    double L[3*3] = { u[1]-u[0], u[3]-u[0],     0 /*u[0]*/,  
                      v[1]-v[0], v[3]-v[0],     0 /*v[0]*/,   
                              0,         0,     1};

    /* [A] transforms the deformed unit square into the {u,v) */
    double A[3*3] = { x[1]-x[0], x[3]-x[0],     x[0],  
                      y[1]-y[0], y[3]-y[0],     y[0],   
                              0,         0,        1};
    double P[3*3] = { 0,0,0,  0,0,0,  0,0,0}; /* initialize with zeros */
    double b[3]   = {x[2],y[2],1.};
    double xx[3];
    double Temp[3*3];
    double T[3*3]={0,0,0, 0,0,0, 0,0,0};
    double alpha,betha;
    double Ainv[3*3];
    m3Inv(A,Ainv);
    m3Inv(L,Linv);

    m3MultAb(Ainv,b,xx);
    alpha=xx[0];
    betha=xx[1];

    P[0]=alpha; P[4]=betha; P[6]=1-betha; P[7]=1-alpha; P[8]=alpha+betha-1;
    T[0] = 1; T[4] = 1; T[8] = 1; T[2] = -u[0]; T[5] = -v[0];
    
    m3MultAB(Linv, T, N);
    
    m3MultAB(A, P, Temp);
    m3MultAB(Temp, N, H4p);
    return 0;
}

void m3PrintMat(char *text, double *A)
{
   printf("%s\n", text);
   printf("| %.3f   %.3f   %.3f  |\n", A[0], A[1], A[2]);
   printf("| %.3f   %.3f   %.3f  |\n", A[3], A[4], A[5]);
   printf("| %.3f   %.3f   %.3f  |\n", A[6], A[7], A[8]);
   printf("\n");
}

void m3PrintVet(char *text, double *v)
{
   printf("%s", text);
   printf("( %.3f,   %.3f,   %.3f)\n", v[0], v[1], v[2]);
}


/*  Teste 

int main( )
{
   double A[9] = {2,1,-1, -1, 4, 3, 1,-1,8};
   double Ainv[9];
   double I[9];
   double v[3] = {5,7,1};
   double b[3];
   double x[3];
   double det = m3Inv(A,Ainv);
   m3PrintMat("[A]",A);
   m3PrintMat("[Ainv]",Ainv);
   printf("det = %f\n",det);
   m3MultAB(A,Ainv,I);
   m3PrintMat("[I]",I);

   m3PrintVet("v = ",v);
   m3MultAb(A,v,b);
   m3PrintVet("b = ",b);
   m3SolvAxb(A,b,x);
   m3PrintVet("x = ",x);
   return 0;
}

*/
