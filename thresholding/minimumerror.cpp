#include <QImage>
#include <math.h>
#include "minimumerror.h"

long h[256];

void thr_me(QImage *image)
{
	QRgb *RGB;
	int i,j,tbest=0;
	float F[256];

/* Histogram */
	histogram (image, h);

/* Compute the factors */
	for (i=1; i<256; i++)
	{
	  F[i] = J(i);
	  if (F[i] < F[tbest]) tbest = i;
	}

/* Threshold */
	for (i=0; i<image->height(); i++)
	  for (j=0; j<image->width(); j++)
	  {
		  RGB=(QRgb *)image->scanLine(i) + j;
		  if (qRed(*RGB) < tbest)
			  *RGB=qRgb(0, 0, 0);
		  else
			  *RGB=qRgb(255, 255, 255);
	  }
}

float J(int t)
{
	float a, b, c, d, x1;

	a = P1(t); b = s1(t);
	c = P2(t); d = s2(t);

	x1 = 1.0 + 2.0*(a*flog(b) + c*flog(d)) -
		2.0*(a*flog(a) + c*flog(c));
	return x1;
}

void histogram(QImage *image, long *hist)
{
	QRgb *RGB;
	int i;

	for (i=0; i<256; i++) hist[i]=0;
	for (int y=0; y<image->height(); y++)
		for (int x=0; x<image->width(); x++)
		{
			RGB=(QRgb *)image->scanLine(y) + x;
			hist[qRed(*RGB)]++;
		}
}

float flog (float x)
{
	if (x > 0.0) return (float)log((double)x);
	 else return 0.0;
}

float P1 (int t)
{
	long i, sum = 0;

	for (i=0; i<=t; i++)
	  sum += h[i];
	return (float)sum;
}

float P2 (int t)
{
	long i, sum = 0;

	for (i=t+1; i<=255; i++)
	  sum += h[i];
	return (float)sum;
}

float u1 (int t)
{
	long i;
	float sum=0.0, p;

	p = P1 (t);
	if (p <= 0.0) return 0.0;

	for (i=0; i<=t; i++)
	  sum += h[i]*i;
	return sum/p;
}

float u2 (int t)
{
	long i;
	float sum=0.0, p;
 
	p = P2 (t);
	if (p <= 0.0) return 0.0;
 
	for (i=t+1; i<=255; i++)
	  sum += h[i]*i;
	return sum/p;
}

float s1 (int t)
{
	int i;
	float sum=0.0, p, u, x;

	p = P1(t);
	if (p<=0.0) return 0.0;
	u = u1(t);
	for (i=0; i<=t; i++)
	{
	  x = (i-u)*(i-u);
	  sum += x*h[i];
	}
	return sum/p;
}

float s2 (int t)
{
	int i;
	float sum=0.0, p, u, x;
 
	p = P2(t);
	if (p<=0.0) return 0.0;
	u = u2(t);
	for (i=t+1; i<=255; i++)
	{
	  x = (i-u)*(i-u);
	  sum += x*h[i];
	}
	return sum/p;
}



