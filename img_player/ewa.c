#include "ewa.h"

#include <stdio.h>

#include <algorithm>
using namespace std;

void ewaWeigths(double *weigths, int length, double alpha)
{
  double r;
  int i;
  for(int i = 0; i < length; i++)
  {
    r = i/(double) (length - 1);
    weigths[ i ] = exp( - alpha * r);
  }
}

void ewaJacobianCoefficients( double x, double y, double *H, 
                             double *Ux, double *Uy, double *Vx, double *Vy)
{
  /* 
	 Considering H = [a d g]
	                 [b e h]
								   [c f i]
  
	 Calculating the parcial derivates, for Jacobian we should have as result:
     dU/dx = Ux = ( y(ah-bg) + ai - gc ) / (gx+hy+i)^2
     dU/dy = Uy = ( x(bg-ah) + bi - hc ) / (gx+hy+i)^2
     dV/dx = Vx = ( y(dh-eg) + di - gf ) / (gx+hy+i)^2
     dV/dx = Vy = ( y(eg-dh) + ei - hf ) / (gx+hy+i)^2

		 By simplication:
		 DENOM = (gx + hy + i)^2;
  */
  double DENOM = H[6]*x + H[7] * y + H[8];
  DENOM *= DENOM;

  /*
    and A = ah-bg, and B = dh-eg
  */
  double A = H[0]*H[7] - H[1]*H[6];
  double B = H[3]*H[7] - H[4]*H[6];

  /* resulting: */ 
  *Ux = (    A*y + (H[0]*H[8] - H[6]*H[2]) ) / DENOM;
  *Uy = ( (-A)*x + (H[1]*H[8] - H[7]*H[2]) ) / DENOM;
  *Vx = (    B*y + (H[3]*H[8] - H[6]*H[5]) ) / DENOM;
  *Vy = ( (-B)*y + (H[4]*H[8] - H[7]*H[5]) ) / DENOM;
}

//
//  Bilinear filter: used by the normal EWA
//  when the ellise fall between pixels.
//
void bilinear_filter(Image *image, double u0, double v0, float *rgb)
{
	int c;
	double fracu, fracv;
	float rgb_p0[3], rgb_p1[3], rgb_p2[3], rgb_p3[3];

	fracu = u0 - (int)u0;
	fracv = v0 - (int)v0;

	imgGetPixel3fv(image, (int)floor(u0), (int) ceil(v0), rgb_p0);
	imgGetPixel3fv(image, (int) ceil(u0), (int) ceil(v0), rgb_p1);
	imgGetPixel3fv(image, (int)floor(u0), (int)floor(v0), rgb_p2);
	imgGetPixel3fv(image, (int) ceil(u0), (int)floor(v0), rgb_p3);

	for (c = 0; c < 3; c++)
	  rgb[c] = (float)((1-fracu) * fracv * rgb_p0[c]
	 	     + fracu * fracv * rgb_p1[c]
		     + (1-fracu) * (1-fracv) * rgb_p2[c]
		     + fracu * (1-fracv) * rgb_p3[c]);
}

void ewaEllipseCoefficients( double Ux, double Uy, double Vx, double Vy,
                             double *A, double *B, double *C, double *F,
														 double quality)
{
   double alpha;

	if(quality == EWA_NORMAL) { /*NORMAL EWA*/
    *A = Vx*Vx + Vy*Vy;
    *B = (-2)*(Ux*Vx + Uy*Vy);
    *C = Ux*Ux + Uy*Uy;
    *F = pow((Ux*Vy - Uy*Vx), 2);
  }
  else { /* High Quality EWA. */
    *A = Vx * Vx + Vy * Vy + 1;
	  *B = -2 * (Ux * Vx + Uy * Vy);
    *C = Ux * Ux + Uy * Uy + 1;
    *F = (*A) * (*C) - (*B) * (*B) / 4;
  }

  // Ellipse is AU^2 + BUV + CV^2 = F, where U = u - U0, V = v - V0.
  // Scale A, B, C and F equally so that F = WTAB length.
  alpha = (double)EWA_WTAB_SIZE/(*F);
  *A = *A * alpha;
  *B = *B * alpha;
  *C = *C * alpha;
  *F = (double)EWA_WTAB_SIZE;
}


void ewaEllipseCenter( double x, double y, double *H, 
                       double *U0, double *V0)
{
  double dest[3], source[3];

  /* Converts to homogeneous coordinates */
  source[0] = x;
  source[1] = y;
  source[2] = 1;

  m3MultAb(H, source, dest);

  /* Convert again to 2D coordinates */
  *U0 = dest[0]/dest[2];
  *V0 = dest[1]/dest[2];
}

void ewaEllipseBoundingBox( double U0, double V0,
														double w, double h,
                            double A, double B, double C, double F,
                            int *u1, int *u2, int *v1, int *v2)
{
  double factor;
  double delta = 4.0 * A * C - B * B;
	factor = 2.0 * sqrt(C * F / delta);
  *u1 = ROUND(max(U0 - factor, (double)(0)));
	*u2 = ROUND(min(U0 + factor, (double)(w - 1)));

	factor = 2.0 * sqrt(A * F / delta);
	*v1 = ROUND(max(V0 - factor, (double)(0)));
	*v2 = ROUND(min(V0 + factor, (double)(h - 1)));
}

void ewaFilter ( Image *img, double U0, double V0,
                 double *wtab,
                 double A, double B, double C, double F,
								 float *rgb)
{
  int w, h;
  int u1, u2, v1, v2;

  int u, v;
  double NUM[3] = {0, 0, 0};
  double DEN = 0.0, DDQ = 2*A, DQ, Q;
  double U = u1-U0;
  double V;
  float texture[3];

  double W;

  w = imgGetWidth(img);
  h = imgGetHeight(img);
  
	//calculates the eclipse bounding box (u1, v1) and (u2, v2)
  ewaEllipseBoundingBox(U0, V0, w, h, A, B, C, F, &u1, &u2, &v1, &v2);

  for(v=v1; v < v2; v++)
  {
    V = v - V0;
    DQ = A * (2*U + 1) + B*V;
    Q = (C*V + B*U)*V + A*U*U;
		
    for(u=u1; u<u2; u++)
    {
      if(Q < F)
      {
        imgGetPixel3fv(img, u, v, texture);
        W = wtab[(int)floor(Q)];
				
        NUM[0] = NUM[0] + W*texture[0];
        NUM[1] = NUM[1] + W*texture[1];
        NUM[2] = NUM[2] + W*texture[2];

        DEN = DEN + W;
      }
      Q = Q+DQ;
      DQ = DQ + DDQ;
   }
  }
  
	if (DEN == 0.0) {
    bilinear_filter(img, U0, V0, rgb);
  }
  else
  {
	  //printf("EWA works\n");
    rgb[0] = (float)NUM[0]/DEN;
    rgb[1] = (float)NUM[1]/DEN;
    rgb[2] = (float)NUM[2]/DEN;
  }
  return;
}

