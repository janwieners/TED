#include <QImage>
#include <math.h>
#include "fuzzy.h"

void thr_fuzz_entropy(QImage *image)
{
	QRgb *RGB;
	double *S, *Sbar, *W, *Wbar;
	double *hist, *F, MN, maxv=0.0, delta;
	int i,j,t, tbest= -1, u0, u1, sum, minsum;
	int start, end;

	S = (double *)malloc(sizeof(double)*256);
	Sbar = (double *)malloc(sizeof(double)*256);
	W = (double *)malloc(sizeof(double)*256);
	Wbar = (double *)malloc(sizeof(double)*256);
	hist = (double *)malloc(sizeof(double)*256);
	F = (double *)malloc(sizeof(double)*256);

	// Find the histogram
	histogram(image, hist);
	MN = (double)(image->height())*(double)(image->width());

	// Find cumulative histogram
	S[0] = hist[0]; W[0] = 0;
	for (i=1; i<256; i++)
	{
	  S[i] = S[i-1] + hist[i];
	  W[i] = i*hist[i] + W[i-1];
	}

	// Cumulative reverse histogram
	Sbar[255] = 0; Wbar[255] = 0;
	for (i=254; i>= 0; i--)
	{
	  Sbar[i] = Sbar[i+1] + hist[i+1];
	  Wbar[i] = Wbar[i+1] + (i+1)*hist[i+1];
	}

	for (t=1; t<255; t++)
	{
	  if (hist[t] == 0.0) continue;
	  if (S[t] == 0.0) continue;
	  if (Sbar[t] == 0.0) continue;

	// Means
	  u0 = (int)(W[t]/S[t] + 0.5);
	  u1 = (int)(Wbar[t]/Sbar[t] + 0.5);

	// Fuzziness measure
	  F[t] = fuzzy_entropy(hist, u0, u1, t)/MN;

	// Keep the minimum fuzziness
	  if (F[t] > maxv) maxv = F[t];
	  if (tbest < 0) tbest = t;
	  else if (F[t] < F[tbest]) tbest = t;
	}

	// Find best out of a range of thresholds
	delta = F[tbest] + (maxv-F[tbest])*0.05;        /* 5% */
	start = (int)(tbest - delta);
	if (start <= 0) start = 1;
	end   = (int)(tbest + delta);
	if (end>=255) end = 254;
	minsum = 1000000;

	for (i=start; i<=end; i++)
	{
	  sum = hist[i-1] + hist[i] + hist[i+1];
	  if (sum < minsum)
	  {
		t = i;
		minsum = sum;
	  }
	}
	fprintf (stderr,"Threshold is %d\n", t);

	// Threshold
	for (i=0; i<image->height(); i++)
	  for (j=0; j<image->width(); j++)
	  {
		  RGB=(QRgb *)image->scanLine(i) + j;
		  if (qRed(*RGB) < t)
			  *RGB=qRgb(0, 0, 0);
		  else
			  *RGB=qRgb(255, 255, 255);
	  }
	free(S); free(Sbar);
	free(W); free(Wbar);
	free(hist); free(F);
}

void thr_fuzz_yager(QImage *image)
{
	QRgb *RGB;
	double *S, *Sbar, *W, *Wbar;
	double *hist, *F, MN, maxv=0.0, delta;
	int i,j,t, tbest= -1, u0, u1, sum, minsum;
	int start, end;

	S = (double *)malloc(sizeof(double)*256);
	Sbar = (double *)malloc(sizeof(double)*256);
	W = (double *)malloc(sizeof(double)*256);
	Wbar = (double *)malloc(sizeof(double)*256);
	hist = (double *)malloc(sizeof(double)*256);
	F = (double *)malloc(sizeof(double)*256);

	// Find the histogram
	histogram(image, hist);
	MN = (double)(image->height())*(double)(image->width());

	// Find cumulative histogram
	S[0] = hist[0]; W[0] = 0;
	for (i=1; i<256; i++)
	{
	  S[i] = S[i-1] + hist[i];
	  W[i] = i*hist[i] + W[i-1];
	}

	// Cumulative reverse histogram
	Sbar[255] = 0; Wbar[255] = 0;
	for (i=254; i>= 0; i--)
	{
	  Sbar[i] = Sbar[i+1] + hist[i+1];
	  Wbar[i] = Wbar[i+1] + (i+1)*hist[i+1];
	}

	for (t=1; t<255; t++)
	{
	  if (hist[t] == 0.0) continue;
	  if (S[t] == 0.0) continue;
	  if (Sbar[t] == 0.0) continue;

	// Means
	  u0 = (int)(W[t]/S[t] + 0.5);
	  u1 = (int)(Wbar[t]/Sbar[t] + 0.5);

	// Fuzziness measure
	  F[t] = fuzzy_yager(u0, u1, t)/MN;

	// Keep the minimum fuzziness
	  if (F[t] > maxv) maxv = F[t];
	  if (tbest < 0) tbest = t;
	  else if (F[t] < F[tbest]) tbest = t;
	}

	// Find best out of a range of thresholds
	delta = F[tbest] + (maxv-F[tbest])*0.05;        /* 5% */
	start = (int)(tbest - delta);
	if (start <= 0) start = 1;
	end   = (int)(tbest + delta);
	if (end>=255) end = 254;
	minsum = 1000000;

	for (i=start; i<=end; i++)
	{
	  sum = hist[i-1] + hist[i] + hist[i+1];
	  if (sum < minsum)
	  {
		t = i;
		minsum = sum;
	  }
	}
	fprintf (stderr,"Threshold is %d\n", t);

	// Threshold
	for (i=0; i<image->height(); i++)
	  for (j=0; j<image->width(); j++)
	  {
		  RGB=(QRgb *)image->scanLine(i) + j;
		  if (qRed(*RGB) < t)
			  *RGB=qRgb(0, 0, 0);
		  else
			  *RGB=qRgb(255, 255, 255);
	  }
	free(S); free(Sbar);
	free(W); free(Wbar);
	free(hist); free(F);
}

void histogram (QImage *image, double *hist)
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

double Ux (int g, int u0, int u1, int t)
{
	double ux, x;

	if (g <= t)
	{
	  x = 1.0 + ((double)abs(g - u0))/255.0;
	  ux = 1.0/x;
	} else {
	  x = 1.0 + ((double)abs(g - u1))/255.0;
	  ux = 1.0/x;
	}
	if (ux< 0.5 || ux>1.0)
	 printf ("Ux = %f\n", ux);
	return ux;
}

double fuzzy_entropy(double *hist, int u0, int u1, int t)
{
	int i;
	double E=0;
	
	for (i=0; i<255; i++)
	  {
	    E += Shannon (Ux(i,u0,u1, t))*hist[i];
	  }
	return E;
}

double Shannon (double x)
{
	if (x > 0.0 && x < 1.0)
	  return (double)(-x*log((double)x) - (1.0-x)*log((double)(1.0-x)));
	else return 0.0;
}

double fuzzy_yager(int u0, int u1, int t)
{
	return Yager (u0, u1, t);
}

double Yager (int u0, int u1, int t)
{
	int i;
	double x, sum=0.0;

	for (i=0; i<256; i++)
	{
	  x = Ux(i, u0, u1, t);
	  x = x*(1.0-x);
	  sum += x*x;
	}
	x = (double)sqrt((double)sum);
	return x;
}



