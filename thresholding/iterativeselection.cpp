#include <QImage>
#include "iterativeselection.h"

void thr_is(QImage *image)
{
	float tt, tb, to, t2;
	int   i, j, t;
	long  N, no, nb;
	QRgb *RGB;

	N = (long)image->width()*(long)image->height();
	tb = 0.0;       to = 0.0;       no = 0;
	for (i=0; i<image->height(); i++) 
	  for (j=0; j<image->width(); j++)
	  {
		  RGB=(QRgb *)image->scanLine(i) + j;
		  to=to+(qRed(*RGB));
	  }
	tt = (to/(float)N);

	while (N) 
	{
	  no = 0; nb = 0; tb=0.0; to = 0.0;
	  for (i=0; i<image->height(); i++) 
	    for (j=0; j<image->width(); j++)
	    {
			RGB=(QRgb *)image->scanLine(i) + j;
			if((float)(qRed(*RGB)) >= tt)
			{
				to = to + (float)(qRed(*RGB));
				no++;
			}
			else
			{
				tb=tb+(float)(qRed(*RGB));
				nb++;
			}
		}
	  if (no == 0) no = 1;
	  if (nb == 0) nb = 1;
	  t2 = (tb/(float)nb + to/(float)no )/2.0;
	  if (t2 == tt) N=0;
	  tt = t2;
	}
	t = (int) tt;

	/* Threshold */
	for (i=0; i<image->height(); i++)
	  for (j=0; j<image->width(); j++)
	  {
		  RGB=(QRgb *)image->scanLine(i) + j;
		  if (qRed(*RGB) < t)
			  *RGB=qRgb(0, 0, 0);
		  else
			  *RGB=qRgb(255, 255, 255);
	  }
}

void thr_is_fast (QImage *image)
{
	long i, j, told, tt, a, b, c, d;
	long N, *hist;

	QRgb *RGB;

	hist = (long *) malloc(sizeof(long)*257);
	for (i=0; i<256; i++) hist[i] = 0;

/* Compute the mean and the histogram */
	N = (long)image->width() * (long)image->height();
	tt = 0;
	for (i=0; i<image->height(); i++)
	  for (j=0; j<image->width(); j++)
	  {
		  RGB=(QRgb *)image->scanLine(i) + j;
		  hist[qRed(*RGB)]++;
		  tt=tt+(qRed(*RGB));
	  }
	tt = (tt/(float)N);

	do
	{
	  told = tt;
	  a = 0; b = 0;
	  for (i=0; i<=told; i++)
	  {
	    a += i*hist[i];
	    b += hist[i];
	  }
	  b += b;

	  c = 0; d = 0;
	  for (i=told+1; i<256; i++)
	  {
	    c += i*hist[i];
	    d += hist[i];
	  }
	  d += d;

	  if (b==0) b = 1;
	  if (d==0) d = 1;
	  tt = a/b + c/d;
	} while (tt != told);
	free (hist);

/* Threshold */
	 for (i=0; i<image->height(); i++)
	  for (j=0; j<image->width(); j++)
	  {
		  RGB=(QRgb *)image->scanLine(i) + j;
		  if (qRed(*RGB) < tt)
			  *RGB=qRgb(0, 0, 0);
		  else
			  *RGB=qRgb(255, 255, 255);
	  }
}



