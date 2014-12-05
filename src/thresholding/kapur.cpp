#include <math.h>
#include <QImage>
#include "kapur.h"

void thr_kapur(QImage *image)
{
	QRgb *RGB;
	int width=image->width();
	int height=image->height();

	int i, j, t;
	float Hb, Hw, *Pt, *hist, *F;

	/* Histogram */
	Pt = (float *)malloc(sizeof(float)*256);
	hist = (float *)malloc(sizeof(float)*256);  
	F = (float *)malloc(sizeof(float)*256);  
	
	histogram (image, hist);

	/* Compute the factors */
	Pt[0] = hist[0];
	for (i=1; i<256; i++)
	  Pt[i] = Pt[i-1] + hist[i];

	/* Calculate the function to be maximized at all levels */
	t = 0;
	for (i=0; i<256; i++)
	{
	  Hb = Hw = 0.0;
	  for (j=0; j<256; j++)
	    if (j<=i)
	      Hb += entropy (hist, j, Pt[i]);
	    else 
	      Hw += entropy (hist, j, 1.0-Pt[i]);

	  F[i] = Hb+Hw;
	  if (i>0 && F[i] > F[t]) t = i;
	}
	free(hist); free(Pt); free(F);

	/* Threshold */
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

float entropy (float *h, int a, float p)
{
	if (h[a] > 0.0 && p>0.0)
	  return -(h[a]/p * (float)log((double)(h[a])/p));
	return 0.0;
}



