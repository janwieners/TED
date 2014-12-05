#include <QImage>
#include <QProgressDialog>
#include <QMessageBox>

#include "basics.h"

// ***
// convert2grayscale()
// ***
// Convert image into Grayscale Image
// return Values:
// true -> successful
// false -> Processing manually canceled
// ****
bool convert2grayscale(QImage *image)
{
	int width, height;
	width=image->width();
	height=image->height();

	QProgressDialog progressdialog("Converting Image to Grayscales.", "Cancel", 0, height);
	progressdialog.setWindowTitle("Converting Image to Grayscales.");
	progressdialog.setWindowModality(Qt::WindowModal);
	progressdialog.setValue(0);
	progressdialog.show();

	QRgb *RGB;
	float gray;

	// Convert Image formally to 32bpp
	if(image->depth()!=32)
	{
		*image=image->convertToFormat(QImage::Format_RGB32);
	}

	for (int y=0;y<height;y++)
	{
		for (int x=0;x<width;x++)
		{
			RGB=(QRgb *)image->scanLine(y) + x;
			gray=(qRed(*RGB)+qGreen(*RGB)+qBlue(*RGB))/3;
			//gray=0.299*qRed(*RGB)+0.587*qGreen(*RGB)+0.144*qBlue(*RGB);
			*RGB=qRgb(gray,gray,gray);
		}
		progressdialog.setValue(y);
		if(progressdialog.wasCanceled())
			return false;
	}
	return true;
}

// ***
// negate()
// ***
// Negates Pixel Values
// ***
bool negate(QImage *image)
{
	int width, height;
	width=image->width();
	height=image->height();

	unsigned char oldVal, newVal;
	unsigned int oldRed, oldGreen, oldBlue, newRed, newGreen, newBlue;
	QRgb *RGB;

	QProgressDialog progressdialog("Negate.", "Cancel", 0, height);
	progressdialog.setWindowTitle("Negating Pixel Values.");
	progressdialog.setWindowModality(Qt::WindowModal);
	progressdialog.setValue(0);
	progressdialog.show();
	
	if (image->depth()==8)
	{
	   for (int y=0;y<height;y++)
	   {
		   for (int x=0;x<width;x++)
		   {
			   oldVal=*(image->scanLine(y)+x);
			   newVal=255-oldVal;
			   *(image->scanLine(y)+x)=newVal;
		   }
		   progressdialog.setValue(y);
		   if(progressdialog.wasCanceled())
			   return false;
	   }
	   return true;
	}
	
	if (image->depth() == 32)
	{
	   for (int y=0;y<height;y++)
	   {
		   for (int x=0;x<width;x++)
		   {
			   RGB=(QRgb *)image->scanLine(y) + x;
			   oldRed=qRed(*RGB);
			   newRed=255-oldRed;
			   oldGreen = qGreen(*RGB);
			   newGreen=255-oldGreen;
			   oldBlue = qBlue(*RGB);
			   newBlue=255-oldBlue;
			   *RGB = qRgb(newRed,newGreen,newBlue);
		   }
		   progressdialog.setValue(y);
		   if(progressdialog.wasCanceled())
			   return false;
	   }
	   return true;
	}
	return false;
}

QImage cutrelevant(QImage *image, QImage *copyimage)
{
	int countx=0;
	int width, height;
	width=image->width();
	height=image->height();
	QRgb *RGB;

	// Search for longest horizontal black lines
	int *maxx=0;
	maxx=new int [height];
	for (int y=0; y<height; y++)
	{
		maxx[y]=0;
		countx=0;
		for (int x=0;x<width;x++)
		{
			RGB=(QRgb *)image->scanLine(y)+x;
			if(qRed(*RGB)==0)
			{
				countx++;
			}
			else
			{
				if(maxx[y] < countx)
					maxx[y]=countx;
				countx=0;
			}
		}
		if(maxx[y] < countx)
			maxx[y]=countx;
	}
	int templengthytop=0;
	int processtopy=0;
	for(int y=height/2; y>0; y--)
	{
		if(templengthytop < maxx[y])
		{
			processtopy=y;
			templengthytop=maxx[y];
		}
	}
	int templengthybottom=0;
	int processbottomy=0;
	for(int y=height/2; y<height; y++)
	{
		if(templengthybottom < maxx[y])
		{
			processbottomy=y;
			templengthybottom=maxx[y];
		}
	}
	delete []maxx;

	// Walk into white field from topy
	bool stop=false;
	int y=processtopy;
	while(stop!=true)
	{
		RGB=(QRgb *)image->scanLine(y)+width/2;
		if(qRed(*RGB)==255)
		{
			// suppose that found white pixel includes tolerance (~25px)
			processtopy=y+25;
			stop=true;
		}
		y++;
	}
	// Walk into white field from bottomy
	stop=false;
	y=processbottomy;
	while(stop!=true)
	{
		RGB=(QRgb *)image->scanLine(y)+width/2;
		if(qRed(*RGB)==255)
		{
			processbottomy=y-25;
			stop=true;
		}
		y--;
	}
	// ***

	if(templengthytop==width)
		for (int x=0; x<width; x++)
		{
			RGB=(QRgb *)image->scanLine(processtopy)+x;
			*RGB=qRgb(255, 0, 0);
		}
	else
		processtopy=1;

	if(templengthybottom==width)
		for (int x=0; x<width; x++)
		{
			RGB=(QRgb *)image->scanLine(processbottomy)+x;
			*RGB = qRgb(255, 0, 0);
		}
	else
		processbottomy=height-1;
	
	// Search for longest vertical black lines	
	int *maxy=0;
	maxy=new int [width];
	for (int x=0; x<width; x++)
	{
		maxy[x]=0;
		countx=0;
		for (int y=0; y<height; y++)		
		{
			RGB=(QRgb *)image->scanLine(y)+x;
			if(qRed(*RGB)==0)
			{
				countx++;
			}
			else
			{
				if(maxy[x] < countx)
					maxy[x]=countx;
				countx=0;
			}
		}
		if(maxy[x] < countx)
			maxy[x]=countx;
	}	
	int templengthxleft=0;
	int processleftx=0;
	for(int x=width/2; x>0; x--)
	{
		if(templengthxleft < maxy[x])
		{
			processleftx=x;
			templengthxleft=maxy[x];
		}
	}
	int templengthxright=0;
	int processrightx=0;
	for(int x=width/2; x<width; x++)
	{
		if(templengthxright < maxy[x])
		{
			processrightx=x;
			templengthxright=maxy[x];
		}
	}
	delete []maxy;

	// Walk into white field from leftx
	stop=false;
	int x=processleftx;
	while(stop!=true)
	{
		RGB=(QRgb *)image->scanLine(height/2)+x;
		if(qRed(*RGB)==255)
		{
			processleftx=x+25;
			stop=true;
		}
		x++;
	}
	// Walk into white field from rightx
	stop=false;
	x=processrightx;
	while(stop!=true)
	{
		RGB=(QRgb *)image->scanLine(height/2)+x;
		if(qRed(*RGB)==255)
		{
			processrightx=x-25;
			stop=true;
		}
		x--;
	}

	if(templengthxleft>=height/2)
		for (int y=0; y<height; y++)
		{
			RGB=(QRgb *)image->scanLine(y)+processleftx;
			*RGB=qRgb(255, 0, 0);
		}
	else
		for (int y=0; y<height; y++)
		{
			RGB=(QRgb *)image->scanLine(y)+0;
			*RGB=qRgb(255, 0, 0);
			processleftx=1;
		}

	if(templengthxright>=height/2)
		for (int y=0; y<height; y++)
		{
			RGB=(QRgb *)image->scanLine(y)+processrightx;
			*RGB = qRgb(255, 0, 0);
		}
	else
		for (int y=0; y<height; y++)
		{
			RGB=(QRgb *)image->scanLine(y)+width-1;
			*RGB = qRgb(255, 0, 0);
			processrightx=width-1;
		}

	QRgb *RGB2;
	QImage tempimage(processrightx-processleftx, processbottomy-processtopy, QImage::Format_RGB32);
	int tempx=0;
	int tempy=0;
	for(int y=processtopy; y<processbottomy; y++)
	{
		tempx=0;
		for(int x=processleftx; x<processrightx; x++)
		{
			RGB=(QRgb *)copyimage->scanLine(y)+x;
			RGB2=(QRgb *)tempimage.scanLine(tempy)+tempx;
			*RGB2=*RGB;
			tempx++;
		}
		tempy++;
	}

	return tempimage;
}

void isolatedecoration(QImage *image, QImage *ornamentimage)
{
	// deal with decoration (glyphs with high amount of red and blue colour information) separately
	int width=image->width();
	int height=image->height();

	QRgb *RGB;

	// First: Generate Histogram for each colour channel to check for maxima's
	char hred[256];
	char hgreen[256];
	char hblue[256];
	// Initialize
	for(int i=0; i<256; i++)
	{
		hred[i]=0;
		hgreen[i]=0;
		hblue[i]=0;
	}
	// Generate Histogram
	for (int y=0;y<height;y++)
	{
		for (int x=0;x<width;x++)
		{
			RGB=(QRgb *)image->scanLine(y)+x;
			hred[qRed(*RGB)]++;
			hgreen[qGreen(*RGB)]++;
			hblue[qBlue(*RGB)]++;			
		}
	}
	// Check Values
	char tmpr=0, tmpg=0, tmpb=0;
	for(int i=0; i<256; i++)
	{
		if(hred[i]>tmpr) tmpr=hred[i];
		if(hgreen[i]>tmpg) tmpg=hgreen[i];
		if(hblue[i]>tmpb) tmpb=hblue[i];
	}

	for (int y=0;y<height;y++)
	{
		for (int x=0;x<width;x++)
		{
			RGB=(QRgb *)image->scanLine(y)+x;
			// High red values
			if(qRed(*RGB) >=(tmpr+(tmpr/5)) && qGreen(*RGB) <=128 && qBlue(*RGB) <=128)
			{
				*RGB=qRgb(255, 255, 255);
				ornamentimage->setPixel(x, y, qRgb(255, 0, 0));
			}
			// High blue values
			else if(qGreen(*RGB) <=128 && qGreen(*RGB) <=128 && qBlue(*RGB) >= (tmpr-(tmpr/10)))
			{
				*RGB=qRgb(255, 255, 255);
				ornamentimage->setPixel(x, y, qRgb(0, 0, 255));
			}
			else
			{
				ornamentimage->setPixel(x, y, qRgb(255, 255, 255));
			}
		}
	}
}

// Practical Algorithms for Image Analysis
// Copyright (c) 1997, 1998, 1999 MLMSoftwareGroup, LLC
// HISTEX: program performs histogram expansion upon a b/w image - removing
// the tails of the probability density function and expanding that 
// function to the entire range, where <hiValue> is the max intensity
bool histogramexpansionsimple(QImage *image)
{
	int NBINS=256;
	double pctKeep=98.0;		/* percent of histogram to keep */
	double tail,			/* percent of histogram tail to remove */
		expand,			/* factor for histogram expansion */
		value;			/* pixel value after hist. expansion */
	long nTotal,			/* total no. of elements in image */
		hiValue,			/* high intensity value */
		nRemove,			/* no. pixels to remove in hist. tail */
		sum,			/* running sum of pixels in tail */
		shift,			/* shift (down to zero) for hist. exp. */
		top,			/* top value for expansion */
		i;
	int hist[256];		/* image histogram array */
	hiValue=255;

	int height=image->height();
	int width=image->width();
	
	tail=(100.0-pctKeep)/2.0;
	
	/* zero histogram array */
	for (i=0; i<NBINS; i++)
		hist[i]=0;
	
	/* construct histogram */
	QRgb *RGB;
	for (int y=0; y<image->height(); y++)
		for (int x=0; x<image->width(); x++)
		{
			RGB=(QRgb *)image->scanLine(y) + x;
			hist[qRed(*RGB)]++;
		}
	nTotal=height*width;
	nRemove=(int) (nTotal * tail / 100.0);
	
	/* find lower tail */
	for(i=0, sum=0; i<NBINS; i++)
	{
		sum+=hist[i];
		if(sum>=nRemove)
			break;
	}
	shift = i;
	
	/* find upper tail */
	for(i=NBINS-1, sum=0; i>0; --i)
	{
		sum+=hist[i];
		if(sum>=nRemove)
			break;
	}
	top=i;
	expand=(double) (hiValue) / (double) (top - shift + 1);
	
	/* do histogram expansion on input data and write output */
	for(int y=0; y<image->height(); y++)
		for(int x=0; x<image->width(); x++)
		{
			RGB=(QRgb *)image->scanLine(y) + x;
			value = (((double) qRed(*RGB)) - shift) * expand;
			if (value < 0.0)
				value = 0.0;
			else if (value > hiValue)
				value = hiValue;

			*RGB=qRgb(value, value, value);
		}
	return true;
}

// Practical Algorithms for Image Analysis
// Copyright (c) 1997, 1998, 1999 MLMSoftwareGroup, LLC
// HISTEXX:     program expands intensity range about a chosen input
// intensity, X, and by a chosen level of expansion

bool histogramexpansion(QImage *image)
{
	int NBINS=256;		/* no. of histogram bins */
	float XMAX=255.0;		/* maximum x-value */
	float YMAX=255.0;		/* maximum y-value */

	int XCENTER_DFLT=128;	/* default center of transform */
	float SLOPE_DFLT=2.0;		/* default transform slope */
	float SLOPE_TAPER_DFLT=0.2;	/* default taper slope */

	register long x;		/* image coordinates */
	unsigned char transform[256];		/* transform array */
	long nCol, nRow;		/* image dimensions */
	long xCenter;			/* intensity center of transformation */
	double xC;			/* (double) of xCenter */
	double slope,			/* slope of mapping transform */
		slopeTaper;			/* slope of tapering transform */
	double y1, y2;		/* tapered y-bounds */
	double a, b, c, temp;		/* intermediate variables */
	double xD;			/* (double) of x */
	
	nRow=image->height();
	nCol=image->width();

	xCenter=XCENTER_DFLT;
	slope=SLOPE_DFLT;
	slopeTaper=SLOPE_TAPER_DFLT;
	
	/* calculate transform */
	xC=(double)xCenter;
	a = (1.0 - slope) / (XMAX * XMAX - 3.0 * xC * XMAX + 3.0 * xC * xC);
	b = -3.0 * a * xC;
	c = slope + 3.0 * a * xC * xC;
	for (x = 0; x < NBINS; x++)
	{
		xD = (double) x;
		temp = a * xD * xD * xD + b * xD * xD + c * xD;
		y1 = slopeTaper * xD;
		y2 = slopeTaper * xD + YMAX - slopeTaper * XMAX;
		if(temp < y1)
			temp = y1;
		else if(temp > y2)
			temp = y2;
		transform[x] = (unsigned char) (temp + 0.5);
	}

	/* transform image intensities */
	QRgb *RGB;
	int value;
	for(int y=0; y<image->height(); y++)
		for(int x=0; x<image->width(); x++)
		{
			RGB=(QRgb *)image->scanLine(y) + x;
			value=transform[qRed(*RGB)];
			*RGB=qRgb(value, value, value);
		}
	return true;
}

bool histogramexpansionrgb(QImage *image)
{
	int NBINS=256;
	double pctKeep=98.0;		/* percent of histogram to keep */
	double tail;			/* percent of histogram tail to remove */
	long nTotal,			/* total no. of elements in image */
		hiValue,			/* high intensity value */
		nRemove,			/* no. pixels to remove in hist. tail */
		sum,			/* running sum of pixels in tail */
		i;
	int histr[256];
	int histg[256];
	int histb[256];
	hiValue=255;

	int height=image->height();
	int width=image->width();
	
	tail=(100.0-pctKeep)/2.0;
	
	/* zero histogram array */
	for (i=0; i<NBINS; i++)
	{
		histr[i]=0;
		histg[i]=0;
		histb[i]=0;
	}
	
	/* construct histogram */
	QRgb *RGB;
	for (int y=0; y<image->height(); y++)
		for (int x=0; x<image->width(); x++)
		{
			RGB=(QRgb *)image->scanLine(y) + x;
			histr[qRed(*RGB)]++;
			histg[qGreen(*RGB)]++;
			histb[qBlue(*RGB)]++;
		}
	nTotal=height*width;
	nRemove=(int) (nTotal * tail / 100.0);
	
	/* find lower tail */
	for(i=0, sum=0; i<NBINS; i++)
	{
		sum+=histr[i];
		if(sum>=nRemove)
			break;
	}
	int shiftr=i;
	
	/* find upper tail */
	for(i=NBINS-1, sum=0; i>0; --i)
	{
		sum+=histr[i];
		if(sum>=nRemove)
			break;
	}
	int topr=i;
	double expandr=(double) (hiValue) / (double) (topr - shiftr + 1);

	/* find lower tail */
	for(i=0, sum=0; i<NBINS; i++)
	{
		sum+=histg[i];
		if(sum>=nRemove)
			break;
	}
	int shiftg=i;
	
	/* find upper tail */
	for(i=NBINS-1, sum=0; i>0; --i)
	{
		sum+=histg[i];
		if(sum>=nRemove)
			break;
	}
	int topg=i;
	double expandg=(double) (hiValue) / (double) (topg - shiftg + 1);

	/* find lower tail */
	for(i=0, sum=0; i<NBINS; i++)
	{
		sum+=histb[i];
		if(sum>=nRemove)
			break;
	}
	int shiftb=i;
	
	/* find upper tail */
	for(i=NBINS-1, sum=0; i>0; --i)
	{
		sum+=histb[i];
		if(sum>=nRemove)
			break;
	}
	int topb=i;
	double expandb=(double) (hiValue) / (double) (topb - shiftb + 1);
	
	int valuer, valueg, valueb;
	/* do histogram expansion on input data and write output */
	for(int y=0; y<image->height(); y++)
		for(int x=0; x<image->width(); x++)
		{
			RGB=(QRgb *)image->scanLine(y) + x;
			valuer = (((double) qRed(*RGB)) - shiftr) * expandr;
			if (valuer < 0.0)
				valuer = 0.0;
			else if (valuer > hiValue)
				valuer = hiValue;

			valueg = (((double) qGreen(*RGB)) - shiftg) * expandg;
			if (valueg < 0.0)
				valueg = 0.0;
			else if (valueg > hiValue)
				valueg = hiValue;

			valueb = (((double) qBlue(*RGB)) - shiftb) * expandb;
			if (valueb < 0.0)
				valueb = 0.0;
			else if (valueb > hiValue)
				valueb = hiValue;

			*RGB=qRgb(valuer, valueg, valueb);
		}
	return true;
}

