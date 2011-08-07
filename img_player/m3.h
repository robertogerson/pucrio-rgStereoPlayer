/**
 *	@file m3.h TAD Matrix 3x3.
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
#ifndef M3_H
#define M3_H

/*
 * Computes the determinant of the matrix [A]
 *
 * @param A The matrix that must be calculated the determant.
 * 
 * @return Det The determinant of matrix A
 */
double m3Det(double* A);

/*
 * Computes the inverse of the matrix [A]
 *
 * @param A the matrix that must be inverted.
 * @param Ainv the inverted matrix.
 *
 * @return 
 */
double m3Inv( double* A, double* Ainv );

/*
 * Computes the matrix product [AB]=[A][B] 
 *
 * @param A the first matrix will be multiplied
 * @param B the second matrix will be multiplied
 * @param AB the returned matrix, representing A*B
 */
void m3MultAB(double* A, double* B, double* AB);

/*
 * Computes the transformation {x}=[A]{b} 
 *
 * @param A the first matrix will be multiplied
 * @param b the double that must multiply A
 * @param x the resulting from [A]*{b}
 */
void m3MultAb(double* A, double* b, double* x);

/*
 * Computes the solution of [A]{x}={b}. 
 *
 * @param A the first matrix will be multiplied
 * @param b the double that must multiply A
 * @param x the resulting matrix from [A]*{b}
 */
double m3SolvAxb(double* A, double* b, double* x);

/*
 * Matrix must be printed. 
 *
 * @param text the name of the matrix A.
 * @param A the first matrix will be printed.
 */
void m3PrintMat(char* text,double* A);

/*
 * Print the vector {v} in console.
 *
 * @param text the name of the vector {v}.
 * @param v the first vector will be printed.
 */
void m3PrintVet(char* text,double* v);

/**
 * Compute the homography matrix from four points.
 *
 * @param H the resulting homography matrix.
 * TODO: Param
 */
void m3Homography4p (double *u, double *v, double *x, double *y, 
                     double *H4p);

#endif
