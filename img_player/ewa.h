/**
 *	@file ewa.h Elliptical Weighted Average (EWA) filter.
 *              The EWA filter compensate aliasing artifacts caused by 
 *              perspective projection.
 *
 *	@author 
 *      - Roberto Azevedo
 *
 *	@date
 *			Criado em:		 28 de Abril de 2011
 *			Última Modificação:	01 de Maio de 2011
 *
 *	@version 1.0
 */

#ifndef EWA_H
#define EWA_H


#define EWA_WTAB_SIZE 1024
#define EWA_ALPHA 2.7

#include <math.h>

#include "m3.h"
#include "image.h"

typedef enum {
  EWA_NORMAL = 0,
	EWA_ENHANCED
}EWA_TYPE;

/**
 * Precomputes the weigths used by ewa filter.
 * WTAB from
 * 
 * @param weigths
 * @param length The size of the table.
 * @param alpha
 */
void ewaWeigths(double *weigths, int length, double alpha);

/**
 * Computes Jacobian Matrix from M.
 * (Ux, Vx) = (ux, vx) and (Uy, Vy) =(uy, vy))
 
 * The Jacobian matrix of the transformation at the output point
 * under consideration is defined as follows:
 *
 * Consider the transformations (x,y) -> (u,v) from input locations 
 * to output locations.
 * 
 * The Jacobian Matrix of the transformation at (x,y) is equal to
 *
 * J = [ A, B ] = [ dU/dx, dU/dy ]
 *     [ C, D ]   [ dV/dx, dV/dy ]
 *
 * The H matrix is the homography the matrix that maps the point x, y to the
 * resulting space.
 * 
 *  H = [a d g]
 *      [b e h]
 *      [c f i]
 *
 * X and Y are mapped from the homography matrix H, and are:
 *
 *   . U = (ax + by + c) / (gx + hy + i)
 *   . V = (dx + ey + f) / (gx + hy + i)
 * 
 * Usually, when using this function to ewa filter, we will pass the
 * x, y as the texture (u,v) coordinates, and the matrix H, as the 
 * homography invertion matrix, that maps from the texture space to the
 * original space. This hasn't any difference to this function.
 * 
 * @param x the value of the x coordinate of the source point.
 * @param y the value of the y coordinate of the source point.
 * @param h The invert homography matrix (in our case the invert matrix
            homography matrix i.e. h4pInv).
 * @param Ux returns the dU/dx value.
 * @param Uy returns the de dU/dy value.
 * @param Vx returns the dV/dx value.
 * @param Vy returns the dV/dy value.
 */
void ewaJacobianCoefficients( double x, double y, double *H, 
                              double *Ux, double *Uy, double *Vx, double *Vy);

/**
 * Computes the Ellipse Coefficients
 *
 * TODO: High quality EWA.
 */
void ewaEllipseCoefficients( double Ux, double Uy, double Vx, double Vy,
                             double *A, double *B, double *C, double *F,
														 double quality);

/**
 * Computes the texture space ellipse center u0 v0, from screen 
 * coordinates x, y.
 *
 * TODO: params.
 */
void ewaEllipseCenter( double x, double y, double *H, 
                       double *U0, double *V0);

/**
 * TODO: Description.
 */
void ewaEllipseBoundingBox( double U0, double V0,
                            double w, double h,
                            double A, double B, double C, double F,
                            int *u1, int *u2, int *v1, int *v2);

/**
 * TODO: Description.
 */
void ewaFilter ( Image *img, double U0, double V0,
                 double *wtab,
                 double A, double B, double C, double F,
                 float *rgb);

#endif /*EWA_H*/
