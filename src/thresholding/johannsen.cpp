#include <math.h>
#include <QImage>
#include <QProgressDialog>

#include "johannsen.h"

bool thr_joh(QImage *image)
{
	QRgb *RGB;
	int i, j, t, start, end;
	float Sb, Sw, *Pt, *hist, *F, *Pq;

	QProgressDialog progressdialog("Computing Threshold based on Johannsen Algorithm.", "Cancel", 0, image->height());
	progressdialog.setWindowTitle("Computing Threshold based on Johannsen Algorithm.");
	progressdialog.setWindowModality(Qt::WindowModal);
	progressdialog.show();
	progressdialog.setValue(0);

	/* Histogram */
	Pt = (float *)malloc(sizeof(float)*256);
	hist = (float *)malloc(sizeof(float)*256);
	F = (float *)malloc(sizeof(float)*256);
	Pq = (float *)malloc(sizeof(float)*256);

	histogram(image, hist);

	/* Compute the factors */
	Pt[0] = hist[0];
	Pq[0] = 1.0 - Pt[0];
	for (i=1; i<256; i++)
	{
	  Pt[i] = Pt[i-1] + hist[i];
	  Pq[i] = 1.0 - Pt[i-1];
	}

	start = 0;
	while (hist[start++] <= 0.0) ;
	end = 255;
	while (hist[end--] <= 0.0) ;

	// Calculate the function to be minimized at all levels
	t = -1;
	for (i=start; i<=end; i++)
	{
	  if(hist[i] <= 0.0)continue;
	  Sb = flog(Pt[i]) + (1.0/Pt[i])*
		(entropy(hist[i])+entropy(Pt[i-1]));
	  Sw = flog (Pq[i]) + (1.0/Pq[i])*
		(entropy(hist[i]) + entropy(Pq[i+1]));
	  F[i] = Sb+Sw;
	  if (t<0) t = i;
	  else if (F[i] < F[t]) t = i;
	}
	free(hist); free (Pt); free (F); free (Pq);

	// Threshold
	int oldVal, newVal;
	for (i=0; i<image->height(); i++)
	  for (j=0; j<image->width(); j++)
	  {
		  RGB=(QRgb *)image->scanLine(i)+j;
		  oldVal=qRed(*RGB);

		  if(oldVal<t)
				newVal=0;
			else 
				newVal=255;
	        *RGB = qRgb(newVal,newVal,newVal);
			
			progressdialog.setValue(i);
		  if(progressdialog.wasCanceled())
			return false;
	  }
	  return true;
}

float entropy (float h)
{
	if (h > 0.0)
	  return (-h * (float)log((double)(h)));
	else return 0.0;
}



