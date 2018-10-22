#ifndef _GMATH_OPTIMIZATION_H
#define _GMATH_OPTIMIZATION_H

bool	powell (float p[], float **xi, int n, float ftol, int *iter, float *fret, float (*func)(float []));
void	linmin (float p[], float xi[], int n, float *fret, float (*func)(float []));
void	mnbrak (float *ax, float *bx, float *cx, float *fa, float *fb, float *fc, float (*func)(float));
float	f1dim (float x);
float	brent (float ax, float bx, float cx, float (*f)(float), float tol,	float *xmin);

#else
#endif // _GMATH_OPTIMIZATION_H