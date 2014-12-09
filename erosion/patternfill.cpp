#include <QImage>
#include <QProgressDialog>

#include "patternfill.h"

bool patternfill3x3(QImage *image)
{
	int width, height, baseline;
	int candidate;

	QProgressDialog progressdialog("Patternfill 3x3", "Cancel", 0, image->height());
	progressdialog.setWindowTitle("Patternfill 3x3");
	progressdialog.setWindowModality(Qt::WindowModal);
	progressdialog.show();
	progressdialog.setValue(0);

	width=image->width();
	height=image->height();

	// Post Processing, based on the following patterns
	// a) 0 0 0    0 0 0
	//    0 1 0 => 0 0 0
	//    0 0 0    0 0 0
	//
	// b) 1 1 1    1 1 1
	//    1 0 1 => 1 1 1
	//    1 1 1    1 1 1
	//
	// c) 0 1   => 1 1
	//    1 1      1 1
	//
	// d) 1 0   => 1 1
	//    1 1      1 1
	//
	// e) 1 1   => 1 1
	//    0 1      1 1
	//
	// f) 1 1   => 1 1
	//    1 0      1 1
	int neighbourcount=0;
	
	int Xmuster_offset[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1};
	int Ymuster_offset[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1};

	// Pattern Processing 4 sure inperformant
	// a)
	for (int y=1; y<image->height()-1; y++)
	{
		for (int x=0; x<image->width(); x++)
		{
			baseline=y;
			// Pixmap über offsets abgrasen: Weiße Pixel auf Nachbarpixel prüfen
			if(*(image->scanLine(y) + x)==255)
			for (int i=0; i<9; i++)
			{
				candidate=*(image->scanLine(baseline + Ymuster_offset[i]) + x + Xmuster_offset[i]);
				// Weißer Pixel gefunden
				if(candidate==255)
					neighbourcount++;
			}
		// maxneighbours -> Anzahl der Nachbarn konstituiert Pixelwert
		if(neighbourcount==1)
			*(image->scanLine(y) + x)=0;
		neighbourcount=0;
		}
		progressdialog.setValue(y);
		if(progressdialog.wasCanceled())
			return false;
	}

	// b)
	for (int y=1; y<image->height()-1; y++)
	{
		for (int x=0; x<image->width(); x++)
		{
			baseline=y;
			// Pixmap über offsets abgrasen: Schwarze Pixel auf Nachbarpixel prüfen
			if(*(image->scanLine(y) + x)==0)
			for (int i=0; i<9; i++)
			{
				candidate=*(image->scanLine(baseline + Ymuster_offset[i]) + x + Xmuster_offset[i]);
				// Schwarzer Pixel gefunden
				if(candidate==0)
					neighbourcount++;
			}
		// maxneighbours -> Anzahl der Nachbarn konstituiert Pixelwert
		if(neighbourcount==1)
			*(image->scanLine(y) + x)=255;
		neighbourcount=0;
		}
		progressdialog.setValue(y);
		if(progressdialog.wasCanceled())
			return false;
	}

	// c), d), e), f)
	// c) 0 1   => 1 1
	//    1 1      1 1
	//
	// d) 1 0   => 1 1
	//    1 1      1 1
	//
	// e) 1 1   => 1 1
	//    0 1      1 1
	//
	// f) 1 1   => 1 1
	//    1 0      1 1
	for (int y=0; y<image->height()-1; y++)
	{
		for (int x=0; x<image->width(); x++)
		{
			// c)
			if(*(image->scanLine(y) + x)==0)
			{
				if(*(image->scanLine(y)+x+1)==255
					&& *(image->scanLine(y+1)+x)==255
					&& *(image->scanLine(y+1)+x+1)==255)
						*(image->scanLine(y)+x)=255;
			}
			else
			{
				// d)
				if(*(image->scanLine(y)+x+1)==0
					&& *(image->scanLine(y+1)+x)==255
					&& *(image->scanLine(y+1)+x+1)==255)
						*(image->scanLine(y)+x)=255;

				// e)
				else if(*(image->scanLine(y)+x+1)==255
					&& *(image->scanLine(y+1)+x)==0
					&& *(image->scanLine(y+1)+x+1)==255)
						*(image->scanLine(y)+x)=255;

				// f)
				else if(*(image->scanLine(y)+x+1)==255
					&& *(image->scanLine(y+1)+x)==255
					&& *(image->scanLine(y+1)+x+1)==0)
						*(image->scanLine(y)+x)=255;
			}
		}
		progressdialog.setValue(y);
		if(progressdialog.wasCanceled())
			return false;
	}
	return true;
}


