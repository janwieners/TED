#include <QImage>
#include <QTextEdit>
#include <QProgressDialog>

#include "median.h"
#include "../basics/quicksort.h"

// ***
// filtermedian()
// ***
// Replaces current Pixel Value with Median Pixel Value in 3x3 Matrix
// ***
bool filtermedian3x3(QImage *image)
{
	int width, height;
	width=image->width();
	height=image->height();
	unsigned int baseline;

	QProgressDialog progressdialog("Filtering Image (Median)", "Cancel", 0, height);
	progressdialog.setWindowTitle("Filtering Image (Median)");
	progressdialog.setWindowModality(Qt::WindowModal);

	progressdialog.show();
	progressdialog.setValue(0);

	int Xoffset[] = { -1, 0, 1, -1, 0, 1, -1, 0, 1};
	int Yoffset[] = { -1, -1, -1, 0, 0, 0, 1, 1, 1};

	int blockarray[9];
	QRgb *RGB;

	for (int y=2; y<height-1; y++)
	{
		for (int x=0; x<width; x++)
		{
			baseline=y;
			for (int i=0; i<9; i++)
			{
				RGB=(QRgb *)image->scanLine(baseline + Yoffset[i]) + x + Xoffset[i];
				blockarray[i]=qRed(*RGB);
			}
			QuickSort(blockarray, 0, 8);
			image->setPixel(x, y, qRgb(blockarray[4], blockarray[4], blockarray[4]));
		}
		progressdialog.setValue(y);
		if(progressdialog.wasCanceled())
			return 0;
	}
	return true;
}

// ***
// Adaptive Median Filter
// ***
// As described by Gonzalez / Woods in "Digital Image Processing (3rd Edition, 2008; 332-334)
// ***
bool adaptivemedian(QImage *image, int windowsizemax)
{
	// Supports Filtering from 3x3 up to 7x7 Filtering Windows
	// Start Window Size=3
	int windowsize=3;
	int width=image->width();
	int height=image->height();
	int a1, a2;
	int b1, b2;
	int ready=0;
	windowintensityvals intensityvals;
	QRgb *RGB;

	QProgressDialog progressdialog("Filtering Image (Adaptive Median)", "Cancel", 0, height);
	progressdialog.setWindowTitle("Filtering Image (Adaptive Median)");
	progressdialog.setWindowModality(Qt::WindowModal);

	progressdialog.show();
	progressdialog.setValue(0);

	for (int y=3; y<height-windowsize; y++)
	{
		for (int x=0; x<width; x++)
		{
			ready=0;
			windowsize=3;
			while(ready!=1)
			{
				RGB=(QRgb *)image->scanLine(y) + x;
				if(windowsize<=windowsizemax)
				{
					adaptivemedianwindow(image, x, y, windowsize, &intensityvals);

					// a1 = Median of intensity Values in Window - Minimum intensity Value in Window
					a1=intensityvals.median-intensityvals.min;
					// a2 = Median of intensity Values in Window - Maximum intensity Value in Window
					a2=intensityvals.median-intensityvals.max;

					if(a1>0 && a2>0)
					{
						// b1 = Pixel at current coordinate - Minimum intensity Value in Window
						b1=qRed(*RGB)-intensityvals.min;
						// b2 = Pixel at current coordinate - Maximum intensity Value in Window
						b2=qRed(*RGB)-intensityvals.max;

						if(b1>0 && b2<0)
						{
							// Output Zxy
							ready=1;
						}
						else
						{
							// OUTPUT Zmed
							*RGB=qRgb(intensityvals.median, intensityvals.median, intensityvals.median);
							ready=1;
						}
					}
					else windowsize+=2;
				}
				else
				{
					// OUTPUT Zmed
					*RGB=qRgb(intensityvals.median, intensityvals.median, intensityvals.median);
					ready=1;
				}
			}
		}
		progressdialog.setValue(y);
		if(progressdialog.wasCanceled())
			return 0;
	}
	return true;
}

void adaptivemedianwindow(QImage *image, int startx, int starty, int windowsize, windowintensityvals *intensityvals)
{
	QRgb *RGB;
	int counter=0;
	int *blockarray;
	int navigate=windowsize/2;
	if(navigate==0) navigate=1;
	int maxcount=windowsize*windowsize;
	blockarray=new int[maxcount];

	// Start in the upper left edge of the window
	navigate*=-1;

	for(int y=0; y<windowsize; y++)
	{
		for(int x=0; x<windowsize; x++)
		{
			RGB=(QRgb *)image->scanLine(navigate+y+starty) + x + navigate + startx;
			blockarray[counter]=qRed(*RGB);
			counter++;
		}
		navigate++;
	}
	QuickSort(blockarray, 0, maxcount-1);
	intensityvals->min=blockarray[0];
	intensityvals->median=blockarray[maxcount/2-1];
	intensityvals->max=blockarray[maxcount-1];
}



