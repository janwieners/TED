#include <vector>
#include <iterator>
#include <algorithm>

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QImage>
#include <QPoint>
#include <QSize>
#include <QProgressDialog>
#include <QVariant>
#include <QMessageBox>

#include <math.h>

#include "objectfinder.h"
#include "../basics/quicksort.h"

bool sortcoordinatesbyX(const QPoint &left, const QPoint &right) 
{
    return left.x() < right.x();
}

bool sortcoordinatesbyY(const QPoint &left, const QPoint &right) 
{
    return left.y() < right.y();
}

int findobjects(QImage *image, QImage *cpyimage, QString workdirectory,
				QString signaturename, QString imageid, int patternsizeX, int patternsizeY)
{
	int width, height;
	width=image->width();
	height=image->height();

	QProgressDialog progressdialog("Suche nach Objekten", "Abbrechen", 0, height);
	progressdialog.setWindowTitle("Suche nach Objekten");
	progressdialog.setWindowModality(Qt::WindowModal);
	progressdialog.show();
	progressdialog.setValue(0);
	
	bool inobject=false;
	int objectcount=0;

	std::vector <QPoint> coordinates;
	std::vector<std::vector <QPoint> > objectcontainer;

	QPoint pathposition;
	QRgb *RGB;
	int pxcount=0;
	int destination=0;
	int tempx, tempy;

	int includeobjecttempx=0;
	int incminy=0;
	int incmaxy=0;

	// Start Path finding
	for (int y=1; y<height-1; y++)
		for (int x=0; x<width; x++)
		{
			// Initialize
			pxcount=0;
			destination=0;			
			inobject=false;
			coordinates.clear();

			// Check for black pixel
			RGB=(QRgb *)image->scanLine(y)+x;
			if(qRed(*RGB)==0)			
			{
				// This could be a new object
				inobject=true;
				tempx=x;
				tempy=y;
				do
				{
					pathposition=movepathfinder(image, tempx, tempy, &destination);
					if(pathposition.x()==-1)
					{
						// Pathfinder couldn't find next pixel
						// Assertion: One-Way paths are noise
						coordinates.clear();
						inobject=false;
					}
					else
					{
						tempx=pathposition.x();
						tempy=pathposition.y();
						pxcount++;
						coordinates.push_back(QPoint(tempx, tempy));
						if(tempx==x && tempy==y)
						{
							// Ready: Object successfully "recognized"
							
							// sort coordinates
							std::sort(coordinates.begin(), coordinates.end(), sortcoordinatesbyX);

							// Save recognized object
							objectcontainer.push_back(coordinates);
							objectcount++;

							// Object saved; now save inner information of object
							std::vector <QPoint> coordhelper;
							coordhelper.clear();
							for(std::vector<QPoint>::iterator helpiterator=coordinates.begin();
								helpiterator!=coordinates.end();
								helpiterator++)
							{
								// if so, delete inner object
								if((*helpiterator).x()!=includeobjecttempx)
								{
									for(int i=incminy; i<=incmaxy; i++)
									{
										//coordhelper.push_back(QPoint(includeobjecttempx, i));
										image->setPixel(includeobjecttempx, i, qRgb(255, 255, 255));
									}
									incminy=incmaxy=(*helpiterator).y();
								}
								else
								{
									if((*helpiterator).y() < incminy) incminy=(*helpiterator).y();
									if((*helpiterator).y() > incmaxy) incmaxy=(*helpiterator).y();
								}
								includeobjecttempx=(*helpiterator).x();
							}

							// Close current object preparation
							inobject=false;
						}
					}
					// Catch endless loops
					if(pxcount==10000) inobject=false;					
				}
				while(inobject);
			}
			progressdialog.setValue(y);
		}
	// End Path finding

	// --------------------------------

	// Prepare and Process Data
	QPoint testpoint;
	QPoint testpoint2;
	int curx, cury;
	int objectcounter=1;

	// Create Directory for Glyph Database
	QString workdir=workdirectory;
	workdir+="/objectdb_";
	workdir+=signaturename;
	
	QDir dir(workdir);

	if(!dir.exists())
	{
		// Create Directory
		dir.mkpath(workdir);
	}
	QString filestring=workdir;
	filestring+="/";
	filestring+=imageid;
	// Create Directory for individual glyph informations
	QDir metadir(filestring);
	if(!metadir.exists())
	{
		// Create Directory
		metadir.mkpath(filestring);
	}
	workdir+="/";
	workdir+=imageid;
	workdir+="_index.xml";
	QFile file_recognized_objects(workdir);
	file_recognized_objects.open(QIODevice::WriteOnly);
	QTextStream out_file_recognized_objects(&file_recognized_objects);
	out_file_recognized_objects << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	out_file_recognized_objects << "<objects>\n";

	QString metadirectory=workdirectory;
	metadirectory+="/objectdb_";
	metadirectory+=signaturename;
	metadirectory+="/";
	metadirectory+=imageid;

	// Stuff for boundary box;
	bool firstrun=true;
	int minx, miny, maxx, maxy;
	float objectsize=0;

	QString objectmetadatastring="";

	QProgressDialog objectprogressdialog("Processing detected Objects", "Cancel", 0, objectcontainer.size());
	objectprogressdialog.setWindowTitle("Processing detected Objects");
	objectprogressdialog.setWindowModality(Qt::WindowModal);
	objectprogressdialog.show();
	objectprogressdialog.setValue(0);

	for(std::vector<std::vector <QPoint> >::iterator testiterator=objectcontainer.begin();
		testiterator!=objectcontainer.end();
		testiterator++)
	{
		firstrun=true;
		objectsize=0;

		//// Write found objects to File
		out_file_recognized_objects << "<glyphobject id=\"" << objectcounter << "\" winindex=\"\">\n";
		out_file_recognized_objects << "<recognitiondata>none</recognitiondata>\n";
		//out_file_recognized_objects << "<recognitiondata>none</recognitiondata>\n<rawdata>\n";

		for(std::vector<QPoint>::iterator testiterator2=testiterator->begin();
			testiterator2!=testiterator->end();
			testiterator2++)
		{
			testpoint2=*testiterator2;
			curx=testpoint2.x();
			cury=testpoint2.y();

			RGB=(QRgb *)image->scanLine(cury)+curx;
			*RGB=qRgb(255, 0, 0);

			//out_file_recognized_objects << curx << " " << cury << "\n";

			if(firstrun==true)
			{
				minx=maxx=curx;
				miny=maxy=cury;
				tempx=curx;
				tempy=cury;
				firstrun=false;
			}	
			else
			{
				// Check for object specific and -characteristic minima's / maxima's
				if(testpoint2.x() < minx) minx=testpoint2.x();
				if(testpoint2.x() > maxx) maxx=testpoint2.x();
				if(testpoint2.y() < miny) miny=testpoint2.y();
				if(testpoint2.y() > maxy) maxy=testpoint2.y();
			}
		}
		// Redraw Glyph
		QImage tempimage(maxx-minx+2, maxy-miny+2, QImage::Format_RGB32);
		// Initialize Image
		tempimage.fill(qRgb(255, 255, 255));

		includeobjecttempx=0;
		incminy=0;
		incmaxy=0;
		for(std::vector<QPoint>::iterator testiterator2=testiterator->begin();
			testiterator2!=testiterator->end();
			testiterator2++)
		{
			testpoint2=*testiterator2;
			tempimage.setPixel(testpoint2.x()-minx, testpoint2.y()-miny, qRgb(0, 0, 0));

			if(testpoint2.x()!=includeobjecttempx)
			{
				for(int i=incminy; i<=incmaxy; i++)
				{
					RGB=(QRgb *)cpyimage->scanLine(i)+includeobjecttempx;
					tempimage.setPixel(includeobjecttempx-minx, i-miny, qRgb(qRed(*RGB), qGreen(*RGB), qBlue(*RGB)));
				}
				incminy=incmaxy=testpoint2.y();
			}
			else
			{
				if(testpoint2.y() < incminy) incminy=testpoint2.y();
				if(testpoint2.y() > incmaxy) incmaxy=testpoint2.y();
			}
			includeobjecttempx=testpoint2.x();
		}
		// Scale output
		tempimage=tempimage.scaled(QSize(patternsizeX, patternsizeY), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

		// Create training data for neural network
		int oldVal;

		QString objectstring=metadirectory;
		objectstring+="/";
		objectstring+=QString::number(objectcounter);
		objectstring+=".tif";
		tempimage.save(objectstring, "tif", 100);

		//Mark objects with boundaries
		for(int count=minx; count<=maxx; count++)
		{
			RGB=(QRgb *)cpyimage->scanLine(miny)+count;
			*RGB=qRgb(255, 0, 0);
		}
		for(int count=minx; count<=maxx; count++)
		{
			RGB=(QRgb *)cpyimage->scanLine(maxy)+count;
			*RGB=qRgb(255, 0, 0);
		}
		for(int count=miny; count<=maxy; count++)
		{
			RGB=(QRgb *)cpyimage->scanLine(count)+minx;
			*RGB=qRgb(255, 0, 0);
		}
		for(int count=miny; count<=maxy; count++)
		{
			RGB=(QRgb *)cpyimage->scanLine(count)+maxx;
			*RGB=qRgb(255, 0, 0);
		}

		//out_file_recognized_objects << "</rawdata>\n";

		// Calculate Normlength of input pattern
		float normlength=0.0;

		for(int y=0; y<patternsizeY; y++)
		{
			for(int x=0; x<patternsizeX; x++)
			{
				RGB=(QRgb *)tempimage.scanLine(y)+x;
				oldVal=qRed(*RGB);
				normlength+=(float)(oldVal*oldVal);
			}
		}
		normlength=1/(float)sqrt((double)normlength);
		out_file_recognized_objects << "<normlength value=\"" << normlength << "\">\n";
		
		// Calculate Normlength for every pixel in input pattern
		double curnormlength;
		for(int y=0; y<patternsizeY; y++)
		{
			for(int x=0; x<patternsizeX; x++)
			{
				RGB=(QRgb *)tempimage.scanLine(y)+x;
				oldVal=qRed(*RGB);

				curnormlength=(double)oldVal*normlength;
				QVariant var1(curnormlength);

				out_file_recognized_objects << var1.toString() << ";";
			}
			out_file_recognized_objects << "\n";
		}
		out_file_recognized_objects << "</normlength>\n";

		out_file_recognized_objects << "<boundingbox minx=\"" << minx << "\" ";
		out_file_recognized_objects << "maxx=\"" << maxx << "\" ";
		out_file_recognized_objects << "miny=\"" << miny << "\" ";
		out_file_recognized_objects << "maxy=\"" << maxy << "\" />";
		out_file_recognized_objects << "\n</glyphobject>\n";

		objectcounter++;
		objectprogressdialog.setValue(objectcounter);
	}
	out_file_recognized_objects << "</objects>";
	file_recognized_objects.close();

	return objectcount;
}

QPoint movepathfinder(QImage *image, int x, int y, int *destination)
{
	// movepathfinder(): walk the countour of objects
	QRgb *RGB;

	// Check, if next pixel is east of center pixel
	RGB=(QRgb *)image->scanLine(y)+x+1;
	if(qRed(*RGB)==0 && *destination!=5)
	{
		// If there's a pixel north of the actual px, walk north
		RGB=(QRgb *)image->scanLine(y-1)+x;
		if(qRed(*RGB)==0)
		{
			*destination=7;
			*RGB=qRgb(255, 255, 255);
			return QPoint(x, y-1);
		}
		// Exception: If there's a black pixel northeast from here: go there
		RGB=(QRgb *)image->scanLine(y-1)+x+1;
		if(qRed(*RGB)==0 && *destination!=4 && *destination!=5)
		{
			*destination=8;
			*RGB=qRgb(255, 255, 255);
			return QPoint(x+1, y-1);
		}

		RGB=(QRgb *)image->scanLine(y)+x+1;
		*destination=1;
		*RGB=qRgb(255, 255, 255);
		return QPoint(x+1, y);
	}

	// Check, if next pixel is south of center pixel
	RGB=(QRgb *)image->scanLine(y+1)+x;
	if(qRed(*RGB)==0 && *destination!=7)
	{
		// If walking from west: check north and northeast
		if(*destination==1)
		{
			// north
			RGB=(QRgb *)image->scanLine(y-1)+x;
			if(qRed(*RGB)==0)
			{
				*destination=7;
				*RGB=qRgb(255, 255, 255);
				return QPoint(x, y-1);
			}
			// northeast
			RGB=(QRgb *)image->scanLine(y-1)+x+1;
			if(qRed(*RGB)==0 && *destination!=4 && *destination!=5)
			{
				*destination=8;
				*RGB=qRgb(255, 255, 255);
				return QPoint(x+1, y-1);
			}
		}
		// If walking to northeast: first check north and northeast
		if(*destination==8)
		{
			// north
			RGB=(QRgb *)image->scanLine(y-1)+x;
			if(qRed(*RGB)==0)
			{
				*destination=7;
				*RGB=qRgb(255, 255, 255);
				return QPoint(x, y-1);
			}
			// northeast
			RGB=(QRgb *)image->scanLine(y-1)+x+1;
			if(qRed(*RGB)==0 && *destination!=4 && *destination!=5)
			{
				*destination=8;
				*RGB=qRgb(255, 255, 255);
				return QPoint(x+1, y-1);
			}
		}
		*destination=3;
		RGB=(QRgb *)image->scanLine(y+1)+x;
		*RGB=qRgb(255, 255, 255);
		return QPoint(x, y+1);
	}

	// Check, if next pixel is west of center pixel
	RGB=(QRgb *)image->scanLine(y)+x-1;
	if(qRed(*RGB)==0 && *destination!=1)
	{
		*destination=5;
		*RGB=qRgb(255, 255, 255);
		return QPoint(x-1, y);
	}

	// Check, if next pixel is north of center pixel
	RGB=(QRgb *)image->scanLine(y-1)+x;
	if(qRed(*RGB)==0 && *destination!=3)
	{
		// If walking from east: check south and southwest
		if(*destination==5)
		{
			// south
			RGB=(QRgb *)image->scanLine(y+1)+x;
			if(qRed(*RGB)==0)
			{
				*destination=3;
				*RGB=qRgb(255, 255, 255);
				return QPoint(x, y+1);
			}
			// southwest
			RGB=(QRgb *)image->scanLine(y+1)+x-1;
			if(qRed(*RGB)==0)
			{
				*destination=4;
				*RGB=qRgb(255, 255, 255);
				return QPoint(x-1, y+1);
			}
		}
		*destination=7;
		RGB=(QRgb *)image->scanLine(y-1)+x;
		*RGB=qRgb(255, 255, 255);
		return QPoint(x, y-1);
	}

	// Check, if next pixel is southeast of center pixel
	RGB=(QRgb *)image->scanLine(y+1)+x+1;
	if(qRed(*RGB)==0 && *destination!=6 && *destination!=5)
	{
		// Exception: If there's a black pixel northeast from here: go there
		RGB=(QRgb *)image->scanLine(y-1)+x+1;
		if(qRed(*RGB)==0 && *destination!=4 && *destination!=5)
		{
			*destination=8;
			*RGB=qRgb(255, 255, 255);
			return QPoint(x+1, y-1);
		}

		RGB=(QRgb *)image->scanLine(y+1)+x+1;
		*destination=2;
		*RGB=qRgb(255, 255, 255);
		return QPoint(x+1, y+1);
	}

	// Check, if next pixel is southwest of center pixel
	RGB=(QRgb *)image->scanLine(y+1)+x-1;
	if(qRed(*RGB)==0 && *destination!=8 && *destination!=1)
	{
		*destination=4;
		*RGB=qRgb(255, 255, 255);
		return QPoint(x-1, y+1);
	}

	// Check, if next pixel is northwest of center pixel
	RGB=(QRgb *)image->scanLine(y-1)+x-1;
	// Don't go northwest if destination==southeast (2)
	// or destination==east (1)
	if(qRed(*RGB)==0 && *destination!=2 && *destination!=1)
	{
		*destination=6;
		*RGB=qRgb(255, 255, 255);
		return QPoint(x-1, y-1);
	}

	// Check, if next pixel is northeast of center pixel
	RGB=(QRgb *)image->scanLine(y-1)+x+1;
	if(qRed(*RGB)==0 && *destination!=4 && *destination!=5)
	{
		*destination=8;
		*RGB=qRgb(255, 255, 255);
		return QPoint(x+1, y-1);
	}

	// EXCEPTIONS
	// Check, if next pixel is southwest of center pixel
	// Without restriction to walk southwest from destination west
	RGB=(QRgb *)image->scanLine(y+1)+x-1;
	if(qRed(*RGB)==0 && *destination!=8)
	{
		*destination=4;
		*RGB=qRgb(255, 255, 255);
		return QPoint(x-1, y+1);
	}
	// Check, if next pixel is southeast of center pixel
	RGB=(QRgb *)image->scanLine(y+1)+x+1;
	if(qRed(*RGB)==0 && *destination!=6)
	{
		*destination=2;
		*RGB=qRgb(255, 255, 255);
		return QPoint(x+1, y+1);
	}
	// Check, if next pixel is northwest of center pixel
	RGB=(QRgb *)image->scanLine(y-1)+x-1;
	if(qRed(*RGB)==0 && *destination!=2)
	{
		*destination=6;
		*RGB=qRgb(255, 255, 255);
		return QPoint(x-1, y-1);
	}
	// Check, if next pixel is northeast of center pixel
	RGB=(QRgb *)image->scanLine(y-1)+x+1;
	if(qRed(*RGB)==0 && *destination!=4)
	{
		*destination=8;
		*RGB=qRgb(255, 255, 255);
		return QPoint(x+1, y-1);
	}
	// Pathfinder recognizes one-way street
	return QPoint(-1, -1);
}


