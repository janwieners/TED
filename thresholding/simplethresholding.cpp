#include <QImage>
#include <QProgressDialog>

// *****
// simplethresholding()
// Reduces image information to monochrome (brute-force threshold method)
// *****
bool simplethresholding(QImage *image, int threshold)
{
	QRgb *RGB;
	unsigned char oldVal, newVal;
	int width, height;
	width=image->width();
	height=image->height();

	QProgressDialog progressdialog("Converting Image Information to monochrome.", "Cancel", 0, height);
	progressdialog.setWindowTitle("Thresholding");
	progressdialog.setWindowModality(Qt::WindowModal);
	progressdialog.show();
	progressdialog.setValue(0);

	// Compare the value of each pixel of the input image with threshold
	for (int y=0; y<height; y++)
	{
		for (int x=0; x<width; x++)
		{
			RGB=(QRgb *)image->scanLine(y) + x;
			oldVal=qRed(*RGB);
			if(oldVal>=threshold) 
				newVal=255;
			else 
				newVal=0;
	        *RGB = qRgb(newVal,newVal,newVal);
	    }
		progressdialog.setValue(y);
		if(progressdialog.wasCanceled())
			return false;
	}
	return true;
}



