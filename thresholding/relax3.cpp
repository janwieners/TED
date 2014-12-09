/* Relaxation method 3 - Rosenfeld & Kak  */
#include <QImage>
#include "relax3.h"
#include <math.h>

float **pp, **qq;

int range(QImage *image, int i, int j)
{
	if ((i<0) || (i>=image->height())) return 0;
	if ((j<0) || (j>=image->width())) return 0;
	return 1;
}

void thr_relax(QImage *image)
{
	int width=image->width();
	int height=image->height();

	float res = 0.0, minres = 10000.0;
	float **p, **q;
	int iter = 0, count = 0;

/* Space allocation */
	p = f2d (height, width);
	q = f2d (height, width);
	pp = f2d (height, width);
	qq = f2d (height, width);

/* Initial assignment of pixel classes */
	assign_class (image, p, q);
	thresh (image, p, q);

/* Relaxation */
	do
	{
	  res = update (image, p, q);
	  iter += 1;
	  if (res < minres)
	  {
		minres = res;
		count = 1;
	  } else if (fabs(res-minres) < 0.0001)
	  {
		if (count > 2) break;
		else count++;
	  }
	  thresh (image, p, q);
	} while (iter < 100 && res > 1.0);

	thresh (image, p, q);
}

/* Threshold */
void thresh (QImage *image, float **p, float **q)
{
	QRgb *RGB;
	int i,j;
	for (i=0; i<image->height(); i++)
	  for (j=0; j<image->width(); j++)
	  {
		  RGB=(QRgb *)image->scanLine(i) + j;
		  if (p[i][j] > q[i][j])
			  *RGB=qRgb(0, 0, 0);
		  else
			  *RGB=qRgb(255, 255, 255);
	  }
}

/* Mean grey level */
float mean (QImage *image)
{
	QRgb *RGB;
	long sum = 0;

	for (int y=0; y<image->height(); y++)
		for (int x=0; x<image->width(); x++)
		{
			RGB=(QRgb *)image->scanLine(y) + x;
			sum+=qRed(*RGB);
		}
	return (float)sum/(float)(image->height())*(float)(image->width());
}

/* Min and max levels */
void minmax (QImage *image, float *gmin, float *gmax)
{
	QRgb *RGB;

	//RGB=(QRgb *)image->scanLine(i) + j;
	//*gmin=*gmax=qRed(*RGB);
	for (int y=0; y<image->height(); y++)
		for (int x=0; x<image->width(); x++)
		{
			RGB=(QRgb *)image->scanLine(y) + x;
			if (*gmin > qRed(*RGB)) *gmin=qRed(*RGB);
			else if (*gmax < qRed(*RGB)) *gmax=qRed(*RGB);
		}
}

void assign_class (QImage *image, float **p, float **q)
{
	QRgb *RGB;
	int i,j;
	float ud2, y, z, u, lm2;

	/* Mean of local area */
	for (i=0; i<image->height(); i++)
		for (j=0; j<image->width(); j++)
		{
		meanminmax (image, i, j, &u, &z, &y);
		ud2 = 2.0*(u-z);
		lm2 = 2.0*(y-u);
		RGB=(QRgb *)image->scanLine(i) + j;
		if (qRed(*RGB) <= u)
		{
		  p[i][j] = 0.5 + (u-qRed(*RGB))/ud2;
		  q[i][j] = 1.0-p[i][j];
		} else {
		  q[i][j] = (0.5 + (qRed(*RGB)-u)/lm2);
		  p[i][j] = 1.0-q[i][j];
		}
	  }
}

void meanminmax (QImage *image, int r, int c, float *mean, float *xmin, float *xmax)
{
	QRgb *RGB;
	int i,j;
	long sum = 0, k=0;
	RGB=(QRgb *)image->scanLine(r) + c;

	*xmin=*xmax=qRed(*RGB);
	for (i=r-10; i<=r+10; i++)
	  for (j=c-10; j<=c+10; j++)
	  {
	    if (range(image, i, j) != 1) continue;
		RGB=(QRgb *)image->scanLine(i) + j;
	    if (*xmin > qRed(*RGB)) *xmin = qRed(*RGB);
	    else if (*xmax < qRed(*RGB)) *xmax = qRed(*RGB);
	    sum += qRed(*RGB);
	    k++;
	  }
	*mean = (float)sum/(float)(k);
}

float Q(QImage *image, float **p, float **q, int r, int c, int cla)
{
	int i,j;
	float sum = 0.0;

	for (i=r-1; i<=r+1; i++)
	  for (j=c-1; j<=c+1; j++)
	    if (i!=r || j!=c)
	      sum += R(image, r, c, i, j,cla, 0)*p[i][j] + 
			R(image, r, c, i, j,cla, 1)*q[i][j];
	return sum/8.0;
}


float update (QImage *image, float **p, float **q)
{
	float z, qw, pk, qb;
	int i,j;

	for (i=1; i<image->height()-1; i++)
	  for (j=1; j<image->width()-1; j++)
	  {
	    qb = (1.0 + Q(image, p, q, i, j, 0));
	    qw = (1.0 + Q(image, p, q, i, j, 1));
	    pk = p[i][j]*qb + q[i][j]*qw;

	    if (pk == 0.0) 
	    {
	      continue;
	    }

	    pp[i][j] = p[i][j]*qb/pk;
	    qq[i][j] = q[i][j]*qw/pk;
	  }

	z = 0.0;
	for (i=1; i<image->height()-1; i++)
	  for (j=1; j<image->width()-1; j++)
	  {
		z += fabs(p[i][j]-pp[i][j]) + fabs(q[i][j]-qq[i][j]);
		p[i][j] = pp[i][j];
		q[i][j] = qq[i][j];
		qq[i][j] = pp[i][j] = 0.0;
	  }
	return z;
}

float R(QImage *image, int r, int c, int rj, int cj, int l1, int l2)
{
	float xd = 0.0;

	QRgb *RGB;
	RGB=(QRgb *)image->scanLine(r) + c;
	int rc=qRed(*RGB);
	RGB=(QRgb *)image->scanLine(rj) + cj;

	xd = 1.0 - (rc - qRed(*RGB))/256.0;
	if (l1 == l2) return xd*0.9;
	return -(1.0-xd)*0.9;
}

float ** f2d (int nr, int nc)
{
	float **x;
	int i;

	x = (float **)calloc ( nr, sizeof (float *) );

	for (i=0; i<nr; i++)
	{  
	  x[i] = (float *) calloc ( nc, sizeof (float)  );
	}
	return x;
}



