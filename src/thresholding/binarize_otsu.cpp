#include <QImage>

#include "otsu.h"

#define MAXVAL 256

void binarize_otsu(QImage *image)
{
	QRgb *RGB;
	int    hist[MAXVAL];
	double pdf[MAXVAL]; //probability distribution
    double cdf[MAXVAL]; //cumulative probability distribution
    double myu[MAXVAL];   // mean value for separation 
    double max_sigma, sigma[MAXVAL]; // inter-class variance 

    /* Histogram generation */
    for(int i=0; i<MAXVAL; i++)
	{ 
        hist[i] = 0;
    }

    for(int x=0; x<image->width(); x++)
	{
        for(int y=0; y<image->height(); y++)
		{
			RGB=(QRgb *)image->scanLine(y)+x;
            hist[qRed(*RGB)]++;
        }
    }

    /* calculation of probability density */
    for(int i=0; i<MAXVAL; i++){
        pdf[i] = (double)hist[i] / (image->width() * image->height());
    }

    /* cdf & myu generation */
    cdf[0] = pdf[0];
    myu[0] = 0.0;       /* 0.0 times prob[0] equals zero */
    for(int i=1; i<MAXVAL; i++){
        cdf[i] = cdf[i-1] + pdf[i];
        myu[i] = myu[i-1] + i*pdf[i];
    }

    /* sigma maximization
       sigma stands for inter-class variance 
       and determines optimal threshold value */
    int threshold = 0;
    max_sigma = 0.0;
    for(int i=0; i<MAXVAL-1; i++){
        if(cdf[i] != 0.0 && cdf[i] != 1.0){
            double p1p2 = cdf[i]*(1.0 - cdf[i]);
            double mu1mu2diff = myu[MAXVAL-1]*cdf[i]-myu[i];
            sigma[i] = mu1mu2diff * mu1mu2diff / p1p2;
        }
        else
            sigma[i] = 0.0;
        if(sigma[i] > max_sigma){
            max_sigma = sigma[i];
            threshold = i;
        }
    }

    for(int x=0; x<image->width(); x++){
        for(int y=0; y<image->height(); y++)
		{
			RGB=(QRgb *)image->scanLine(y)+x;
			
			if (qRed(*RGB) > threshold)
				*RGB=qRgb(MAXVAL-1, MAXVAL-1, MAXVAL-1);
			else
				*RGB=qRgb(0, 0, 0);
        }
    }
}


