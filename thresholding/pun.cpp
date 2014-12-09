/* Pun method for using entropy */
#include <math.h>
#include <QImage>
#include "pun.h"

void thr_pun(QImage *image)
{
	int i, j, t;
	float *Ht, HT, *Pt, x, *F, y, z;
	float *hist, to, from;

	QRgb *RGB;
	int width=image->width();
	int height=image->height();

	// Histogram
	Ht = (float *)malloc (sizeof(float)*256);
	Pt = (float *)malloc (sizeof(float)*256);        
	F  = (float *)malloc (sizeof(float)*256);        
	hist = (float *)malloc (sizeof(float)*256);        
	histogram (image, hist);

	// Compute the factors
	HT = Ht[0] = entropy (hist, 0);
	Pt[0] = hist[0];
	for (i=1; i<256; i++)
	{
	  Pt[i] = Pt[i-1] + hist[i];
	  x = entropy(hist, i);
	  Ht[i] = Ht[i-1] + x;
	  HT += x;
	}

	// Calculate the function to be maximized at all levels
	t = 0;
	for (i=0; i<256; i++)
	{
	  to = (maxtot(hist,i));
	  from = maxfromt(hist, i);
	  if (to > 0.0 && from > 0.0)
	  {
	    x = (Ht[i]/HT)* flog(Pt[i])/flog(to);
	    y = 1.0 - (Ht[i]/HT);
	    z = flog(1 - Pt[i])/flog(from);
	  }
	  else x = y = z = 0.0;
	  F[i] = x + y*z;
	  if (i>0 && F[i] > F[t]) t = i;
	}
	fprintf (stderr, "Threshold is %d\n", t);
	free(Ht); free(Pt); free(F);  free(hist); 

	// Threshold
	for (i=0; i<height; i++)
	  for (j=0; j<width; j++)
	  {
		  RGB=(QRgb *)image->scanLine(i) + j;
		  if (qRed(*RGB) < t)
			  *RGB=qRgb(0, 0, 0);
		  else
			  *RGB=qRgb(255, 255, 255);
	  }
}

void histogram(QImage *image, float *hist)
{
	QRgb *RGB;
	int i;
	long N;

	for (i=0; i<256; i++) hist[i] = 0.0;
	N = (long)image->width() * (long)image->height();

	for (int y=0; y<image->height(); y++)
		for (int x=0; x<image->width(); x++)
		{
			RGB=(QRgb *)image->scanLine(y) + x;
			hist[qRed(*RGB)]++;
		}

	for (i=0; i<256; i++)
	  hist[i] /= (float)N;
}

float entropy (float *h, int a)
{
	if (h[a] > 0.0)
	  return -(h[a] * (float)log((double)h[a]));
	return 0.0;
}

float maxtot (float *h, int i)
{
	float x;
	int j;

	x = h[0];
	for (j=1; j<=i; j++)
	  if (x < h[j]) x = h[j];
	return x;
}

float maxfromt (float *h, int i)
{
	int j;
	float x;

	x = h[i+1];
	for (j=i+2; j<=255; j++)
	  if (x < h[j]) x = h[j];
	return x;
}



