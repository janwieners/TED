/* Digital Desk - adaptive thresholding */
#include <QImage>
#include "movingaverages.h"

float pct = 15.0;       /* Make smaller to darken the image */
float Navg = 8.0;       /* Fraction of a row in the average (ie 1/8) */

void thrdd (QImage *image)
{
	int NC, row, col, inc;
	float mean, s, sum;
	unsigned char *p;
	long N, i;

	N = (long)image->width() * (long)image->height();
	NC = image->width();
	s = (int)(float)(NC/Navg);
	sum = 127*s;

	row = col = 0;

	p = image->scanLine(0) + 0;
	inc = 1;

	for (i=0; i<N-1; i++)
	{
	  if (col >= NC)
	  {
	    col=NC-1; row++;
	    p=image->scanLine(row) + col;
	    inc=-1;
	  }
	  else if (col < 0)
	  {
	    col = 0;
	    row++;
	    p=image->scanLine(row) + col;
	    inc = 1;
	  }

/* Estimate the mean of the last NC/8 pixels. */
	  sum = sum - sum/s + *p;
	  mean = sum/s;
	  if (*p < mean*(100-pct)/100.0)
	  {
		  *p=qRgb(0, 0, 0);
	  }
	  else
	  {
		  *p=qRgb(255, 255, 255);
	  }
	  p += inc;
	  col += inc;
	}
}


