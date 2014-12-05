#include <QImage>
#include "laplacian.h"

float PCT = 85.0;
static int hist[2048];



void thr_lap(QImage *image)
{
	QRgb *RGB;
	float ** Lap;
	int i, j, t, v;
	int width=image->width();
	int height=image->height();

	// Compute the Laplacian of 'im'
	Lap=f2d(height, width);
	Laplacian(image, Lap);

	// Find the high 85% of the Laplacian values */
	fhist(Lap, hist, height, width);
	hi_pct(hist, 2048, (long)height*(long)width, PCT, &v);

	//Construct histogram of the grey levels of hi Laplacian pixels
	peaks (image, hist, Lap, v, &t);

	//Threshold
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

// Return the level marking the high 85% of pixels
void hi_pct (int *hist, int NH, long N, float pct, int *val)
{
	int i,j=0, m;

	*val = -1;
	m = (pct/100.0) * N;
	for (i=0; i<NH; i++)
	{
	  j += hist[i];
	  if (j>=m)
	  {
	    *val = i;
	    break;
	  }
	}
	//if (*val < 0) printf ("BAD HISTOGRAM in 'hi_pct'.\n");
}

//Construct a histogram of a float matrix
void fhist (float **data, int *hist, int nr, int nc)
{
	int i,j;

	for (i=0; i<2048; i++) hist[i] = 0;
	for (i=0; i<nr; i++)
	  for (j=0; j<nc; j++)
	    hist[(int)(data[i][j])] += 1;
}

void Laplacian (QImage *input, float **output)
{
	int i,j;

	for (i=1; i<input->height()-1; i++)
	  for (j=1; j<input->width()-1; j++)
	    output[i][j]=pix_lap(input, i, j);
}

float pix_lap (QImage *image, int r, int c)
{
	QRgb *RGB;	
	int k=0, i,j;

	for (i= -1; i<=1; i++)
		for (j= -1; j<=1; j++)
			if (i!=0 || j!=0)
			{
				RGB=(QRgb *)image->scanLine(i+r) + j+c;
				k+=qRed(*RGB);
			}	
	RGB=(QRgb *)image->scanLine(r)+c;
	k=k-8*(int)qRed(*RGB);
	
	if (k<=0)
		return 0.0;
	return (float)k/8.0;
}

void peaks (QImage *image, int *hist, float **lap, int lval, int *t)
{
	QRgb *RGB;
	int N, i,j,k;

	for (i=0; i<256; i++) hist[i] = 0;
	*t = -1;

	//Find the histogram
	N = image->height()*image->width();

	for (i=0; i<image->height(); i++)
	  for (j=0; j<image->width(); j++)
	    if (lap[i][j] >= lval)
		{
			RGB=(QRgb *)image->scanLine(i) + j;
			hist[qRed(*RGB)]++;
		}

	//Find the first peak
	j = 0;
	for (i=0; i<256; i++)
	  if (hist[i] > hist[j]) j = i;

	//Find the second peak
	k = 0;
	for (i=0; i<256; i++)
	  if (i>0 && hist[i-1]<=hist[i] && i<255 && hist[i+1]<=hist[i])
	    if ((k-j)*(k-j)*hist[k] < (i-j)*(i-j)*hist[i]) k = i;

	*t = j;
	if (j<k)
	{
	  for (i=j; i<k; i++)
	    if (hist[i] < hist[*t]) *t = i;
	} else {
	  for (i=k; i<j; i++)
	    if (hist[i] < hist[*t]) *t = i;
	}
}



