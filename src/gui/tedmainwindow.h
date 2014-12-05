#ifndef TED_H
#define TED_H

#include <QtGui>
#include <QDialog>
#include <QString>
#include <QMainWindow>
#include <QDomDocument>
#include <QProgressDialog>

#include "dlgsettings.h"
#include "dlgobjectinfo.h"
#include "../basics/basics.h"
#include "../filters/kfill.h"
#include "../filters/median.h"
#include "../erosion/patternfill.h"
#include "../erosion/nextneighbors.h"
#include "../thresholding/pun.h"
#include "../thresholding/otsu.h"
#include "../thresholding/ptile.h"
#include "../thresholding/fuzzy.h"
#include "../thresholding/kapur.h"
#include "../thresholding/johannsen.h"
#include "../thresholding/relax3.h"
#include "../thresholding/laplacian.h"
#include "../thresholding/minimumerror.h"
#include "../thresholding/movingaverages.h"
#include "../thresholding/iterativeselection.h"
#include "../thresholding/simplethresholding.h"
#include "../thresholding/greylevelhistograms.h"
#include "../edge_detection/grad1.h"
#include "../boxes/objectfinder.h"
#include "../nnkohonen/nnkohonen.h"

#include "../nnmlp/nnmlp.h"

// Override QLabel's mousePressEvent
class view : public QLabel
{
	Q_OBJECT

signals:
     void printcoordinate(QPoint curcoordinate);

private:
	void mousePressEvent(QMouseEvent *MouseEvent)
	{
		// Switch Mouse Events
		switch (MouseEvent->button())
		{
			// Left Mousebutton: Show specific object
			case Qt::LeftButton:
				curcoordinate=mapFromGlobal(MouseEvent->globalPos());
				emit printcoordinate(curcoordinate);
			break;
			default:
			break;
		}
	}
	QPoint curcoordinate;
};
// *************

class signaturebrowseitem : public QLabel
{
	Q_OBJECT

signals:
     void changeimage(QString curimage);

private:
	void mousePressEvent(QMouseEvent *MouseEvent)
	{
		// Switch Mouse Events
		switch (MouseEvent->button())
		{
			// Left Mousebutton: Show specific object
			case Qt::LeftButton:
				curimage=objectName();
				emit changeimage(curimage);
			break;
			default:
			break;
		}
	}
	QString curimage;
};

// Main Application
class TED : public QMainWindow
{
	Q_OBJECT

public:
	TED();
	~TED();

private slots:
	// Connect with class view
	void objectclicked(QPoint curobject)
	{
		if(ranclustering==true)
			getobjectat(curobject);
	}
	void saveprocessingstatus(QString status)
	{
		bool stop=false;
		QDomElement docElem=projectxml.documentElement();
		QDomNode node=docElem.firstChild();
		while(stop!=true)
		{
			if(node.isNull())
				break;

			QDomElement e=node.toElement();
			if(!e.isNull())
			{
				QDomAttr currentimageid=e.attributeNode("imageid");
				if(activeimageid==currentimageid.value())
				{
					e.attributeNode("status").setValue(status);
					stop=true;
				}
			}
			node=node.nextSibling();
		}
		// save XML
		QString rawxml=projectxml.toString();
		QString projectindexfile=workdirectory;
		projectindexfile+="/";
		projectindexfile+=signaturename;
		projectindexfile+=".ted";

		QFile file_projectindexfile(projectindexfile);
		file_projectindexfile.open(QIODevice::WriteOnly);
		QTextStream out_file_projectindexfile(&file_projectindexfile);
		out_file_projectindexfile << rawxml;
		file_projectindexfile.close();
	}
	void saveobjectcount(QString objects)
	{
		bool stop=false;
		QDomElement docElem=projectxml.documentElement();
		QDomNode node=docElem.firstChild();
		while(stop!=true)
		{
			if(node.isNull())
				break;

			QDomElement e=node.toElement();
			if(!e.isNull())
			{
				QDomAttr currentimageid=e.attributeNode("imageid");
				if(activeimageid==currentimageid.value())
				{
					e.attributeNode("objects").setValue(objects);
					stop=true;
				}
			}
			node=node.nextSibling();
		}
		// save XML
		QString rawxml=projectxml.toString();
		QString projectindexfile=workdirectory;
		projectindexfile+="/";
		projectindexfile+=signaturename;
		projectindexfile+=".ted";

		QFile file_projectindexfile(projectindexfile);
		file_projectindexfile.open(QIODevice::WriteOnly);
		QTextStream out_file_projectindexfile(&file_projectindexfile);
		out_file_projectindexfile << rawxml;
		file_projectindexfile.close();
	}
	void saveprocessedimage(QImage *saveimage, QString processingstep)
	{
		QString metadirectory=workdirectory;
		metadirectory+="/objectdb_";
		metadirectory+=signaturename;
		metadirectory+="/";
		metadirectory+=activeimageid;
		QDir metadir(metadirectory);
		if(!metadir.exists())
		{
			// Create Directory
			metadir.mkpath(metadirectory);
		}
		metadirectory+="/";
		metadirectory+="processed_";
		metadirectory+=processingstep;
		metadirectory+=".tif";
		QImage *tempimage=new QImage;
		tempimage=saveimage;
		tempimage->save(metadirectory, "tif", 100);
	}
	// Connect with Signaturebrowser
	void setactiveimage(QString curimage);
	void createSignature(void);
	void openexistingSignature(void);
	void updatebuttons(void);
	void loadImage(void);
	void aboutQt(void) { QMessageBox::aboutQt(0, "About Qt"); }
	void aboutTED(void) { QMessageBox::information(0, tr("TED::TED Enhances Digitization"),
		tr("TED::TED Enhances Digitization<br />Ein Beitrag zur OCR auf extrem alten Drucken<br /><br />Version 0.1 (08/2008)<br />Jan Gerrit Wieners")); }
	void preprocessinghelper(void);
	void objectdetecthelper(void);
	void nnkohonenhelper(void);
	void outputhelper(void);
	void settingshelper(void)
	{
		// Init Settings Dialogue
		dlgsettings=new class dlgsettings(expandhistogramrgb, expandhistogram, thresholdingmethod,
			setcut, setmedian, setadaptivemedian, setkfill, seteroding, patternsizeX, patternsizeY,
			kohonenalpha, kohonenneighborhood, kohonenperiod, kohonenmaxcycles);
		connect(dlgsettings,
		SIGNAL(printcursettings(bool, bool, int, bool, bool,
		bool, bool, bool, int, int, float, int, int, int)),
		this, SLOT(settingssave(bool, bool, int, bool, bool,
		bool, bool, bool, int, int, float, int, int, int)));
		dlgsettings->show();
	}
	void settingssave(bool histoexprgb, bool histoexp, int threshmethod, bool cuton, bool medianon, bool adaptivemedianon,
		bool kfillon, bool erodingon, int patternsizex, int patternsizey,
		float kohonena, int kohonenneigh, int kohonenp, int kohonenmax)
	{
		expandhistogramrgb=histoexprgb;
		expandhistogram=histoexp;
		thresholdingmethod=threshmethod;
		setcut=cuton;
		setmedian=medianon;
		setadaptivemedian=adaptivemedianon;
		setkfill=kfillon;
		seteroding=erodingon;
		patternsizeX=patternsizex;
		patternsizeY=patternsizey;
		kohonenalpha=kohonena;
		kohonenneighborhood=kohonenneigh;
		kohonenperiod=kohonenp;
		kohonenmaxcycles=kohonenmax;
	}
	void repaintmainview(void)
	{
		// Repaint View to visualize recognized objects
		view->setPixmap(QPixmap::fromImage(cpyimage));
		// Save recognized objects
		saveprocessedimage(&cpyimage, "3");
	}
	void setcurwinindex(int negwinindex)
	{
		minwinindex=negwinindex;
	}
	void saveobjectinfodlgdata();
	void performsignaturewideclustering(void);

private:
	// Dialogue Stuff
	// Override QLabel
    class view *view;
	class dlgsettings *dlgsettings;
	class signaturebrowseitem *signaturebrowseitem;
	QScrollArea *imageView;

	class mlpnet *mlpnet;

	QVBoxLayout *signaturebrowser;
	QScrollArea *signaturebrowserscroll;
	QWidget *dockWidgetContents;

	// Object information dialogue
	dlgobjectinfo* objectinfodlg;

	// Create a new Project: Name the signature
	QString signaturename;
	QString workdirectory;

	// Menues
	QMenu *fileMenu;
	QMenu *helpMenu;

	// Dock Widgets
	QDockWidget *outputconsole;
	QDockWidget *signatureimages;

	// Toolbars
	QToolBar *toolbar;
	QToolBar *processingbar;

	// Actions
	QAction *exitAction;
	QAction *selectAction;
	QAction *aboutQtAction;
	QAction *aboutTEDAction;
	QAction *settingsAction;
	QAction *loadimageAction;
	QAction *opensignatureAction;
	QAction *createsignatureAction;
	QAction *signaturewideclusteringAction;

	// Processing Toolbar Actions
	QAction *ocrAction;
	QAction *outputAction;
	QAction *nnkohonenAction;
	QAction *preprocessingAction;
	
	// Private Functions
	void createMenus(void);
	void createActions(void);
    void createStatusBar(void);
	void createProcessingbar(void);
	void createOutputConsole(void);
	void createsignatureimagesdock(void);

	// XML
	bool createdxml;
	QDomDocument doc;
	QDomDocument projectxml;
	void getobjectat(QPoint curobject);
	QString projectindexfile;

	// Image Stuff
	QImage image;
	QImage cpyimage;
	QString filename;
	QLabel *currentfile;
	QLabel *signaturenamelabel;
	QString activeimageid;

	// Output Console Widget
	QTextEdit *writeonconsole;

	int objectcount;
	void setobjectcount(int count) { objectcount=count; }
	int imageid;

	// Control Variables
	bool gotimage;
	bool gotsignature;
	bool ranclustering;
	bool ranpreprocessing;
	bool ranobjectrecognition;

	bool expandhistogramrgb;
	bool expandhistogram;
	int thresholdingmethod;
	bool setcut;
	bool setmedian;
	bool setadaptivemedian;
	bool seteroding;
	bool setkfill;
	int patternsizeX;
	int patternsizeY;
	float kohonenalpha;
	int kohonenneighborhood;
	int kohonenperiod;
	int kohonenmaxcycles;
	int minwinindex;

	QList <QLabel*> labelwidgetcontainer;
};

#endif // TED_H












