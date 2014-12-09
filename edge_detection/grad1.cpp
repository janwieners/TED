#include <QImage>

/* Gradient edge detector - no templates */
/* G1 */
#include "grad1.h"
#include <math.h>

void grad1(QImage *image)
{
	int i,j;
	double z, dx, dy;
	int pmax=0, pmin=255, thresh = 128;

	QRgb *RGB;
	QRgb *RGBTemp;
	int width=image->width();
	int height=image->height();

	for (i=height-1; i>=1; i--)
	  for (j=width-1; j>=1; j--)
	  {
		  RGB=(QRgb *)image->scanLine(i)+j;
		  RGBTemp=(QRgb *)image->scanLine(i-1)+j;
		  dx=qRed(*RGB)-qRed(*RGBTemp);
		  
		  RGB=(QRgb *)image->scanLine(i) + j;
		  RGBTemp=(QRgb *)image->scanLine(i) + j-1;
		  dy =qRed(*RGB)-qRed(*RGBTemp);

		  z=sqrt(dx*dx + dy*dy);

	    if (z > 255.0)
			z=255;
		else if(z < 0.0)
			z = 0;
		
		RGB=(QRgb *)image->scanLine(i) + j;
		*RGB=qRgb(z, z, z);

	    if (pmax < (unsigned char)z) pmax = (unsigned char)z;
	    if (pmin > (unsigned char)z) pmin = (unsigned char)z;
	  }
	thresh = (pmax-pmin)/2;

	for (i=0; i<height; i++)
	{
		RGB=(QRgb *)image->scanLine(i)+0;
		*RGB=qRgb(0, 0, 0);

		RGB=(QRgb *)image->scanLine(i)+width-1;
		*RGB=qRgb(0, 0, 0);
	}
	for (j=0; j<width; j++)
	{
		RGB=(QRgb *)image->scanLine(0)+j;
		*RGB=qRgb(0, 0, 0);

		RGB=(QRgb *)image->scanLine(height-1)+j;
		*RGB=qRgb(0, 0, 0);
	}

	/*	Threshold	*/
	for (i=0; i<height; i++)
	  for (j=0; j<width; j++)
	  {
		  RGB=(QRgb *)image->scanLine(i)+j;
		  if (qRed(*RGB) > thresh)
		  {
			  RGB=(QRgb *)image->scanLine(i)+j;
			  *RGB=qRgb(0, 0, 0);
		  }
		  else
		  {
			  RGB=(QRgb *)image->scanLine(i)+j;
			  *RGB=qRgb(255, 255, 255);
		  }
	  }
}


