#include <QImage>
#include <QProgressDialog>

#include "nextneighbors.h"

bool nextneighbours3x3(QImage *image, unsigned char maxneighbours)
{
	unsigned int width, baseline, neighbourcount=0;

	QProgressDialog progressdialog("Eroding (Next Neighbors 3x3)", "Cancel", 0, image->height());
	progressdialog.setWindowTitle("Eroding (Next Neighbors 3x3)");
	progressdialog.setWindowModality(Qt::WindowModal);
	progressdialog.show();
	progressdialog.setValue(0);
	
	// Navigate in Pixmap
	// Pattern:
	// - - -
	// - . -
	// - - -
	int Xoffset[9] = { -1, 0, 1, -1, 0, 1, -1, 0, 1};
	int Yoffset[9] = { -1, -1, -1, 0, 0, 0, 1, 1, 1};

	width=image->width();
	QRgb *RGB;
	QRgb *RGBtmp;
	for (int y=1; y<image->height()-1; y++)
		for (int x=0; x<image->width(); x++)
		{
			baseline=y;
			RGB=(QRgb *)image->scanLine(y)+x;
			// Check, if center Pixel's got black neighbor(s)
			if(qBlue(*RGB)==0)
			{
				for (int i=0; i<9; i++)
				{
					RGBtmp=(QRgb *)image->scanLine(baseline+Yoffset[i])+x+Xoffset[i];
					if(qBlue(*RGBtmp)==0)
						neighbourcount++;
				}
				if(neighbourcount<=maxneighbours)
					image->setPixel(x, y, qRgb(255, 255, 255));
			}
			neighbourcount=0;

			progressdialog.setValue(y);
			if(progressdialog.wasCanceled())
			return false;
		}
	return true;
}


