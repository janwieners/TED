#include <QImage>
#include "ptile.h"
// ***
// p-tile Thresholding (Parker[1997])
// Thresholding by using the black percentage
// ***

void thr_percent(QImage *image, float pct)
{
	int width=image->width();
	int height=image->height();
	int hist[256], count= 0, t= -1;
	long N, M;
	QRgb *RGB;

	for (int i=0; i<256; i++) hist[i] = 0;

	// Find the histogram
	N=(long)width*(long)height;
	M=(long)((pct/100.0)*N);

	// Generate Histogram
	for (int y=0; y<height; y++)
		for (int x=0; x<width; x++)
		{
			RGB=(QRgb *)image->scanLine(y) + x;
			hist[qRed(*RGB)]++;
		}

	// Find the correct bin
	for (int i=0; i<256; i++)
	{
		count+=hist[i];
		if(count >= M)
		{
			t=i;
			break;
		}
	}
	
	/* Threshold */
	for (int y=0; y<height; y++)
	  for (int x=0; x<width; x++)
	  {
		  RGB=(QRgb *)image->scanLine(y)+x;
		  if(qRed(*RGB) < t)
			  *RGB=qRgb(0, 0, 0);
		  else
			  *RGB=qRgb(255, 255, 255);
	  }
}



