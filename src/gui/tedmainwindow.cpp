#include <QDir>
#include <QtGui>

#include "tedmainwindow.h"

// Benchmarking stuff
#include <time.h>

// Construct TED
TED::TED()
{
	// Initialize TED: Status Bar, Menus, Actions, etc.
	// Initialize Main View
	view=new class view;
	imageView=new QScrollArea(this);
	imageView->setBackgroundRole(QPalette::Dark);
	imageView->setWidget(view);
	setCentralWidget(imageView);

	// Initialize Object Information Dialogue
	objectinfodlg=new dlgobjectinfo;

	// Init settings dialogue
	dlgsettings=new class dlgsettings;

	createActions();
	createMenus();
	createProcessingbar();
	createStatusBar();
	createOutputConsole();
	createsignatureimagesdock();
	setWindowTitle(tr("TED :: TED Enhances Digitization"));

	// Set Default Values
	// Otsu Thresholding
	expandhistogramrgb=false;
	expandhistogram=true;
	thresholdingmethod=4;
	setcut=true;
	setmedian=true;
	setadaptivemedian=false;
	setkfill=true;
	seteroding=true;

	patternsizeX=15;
	patternsizeY=15;
	kohonenalpha=0.25;
	kohonenneighborhood=5;
	kohonenperiod=50;
	kohonenmaxcycles=50;
	minwinindex=-1;

	// Initialize Control Variables
	imageid=0;
	gotimage=false;
	createdxml=false;
	gotsignature=false;
	ranclustering=false;
	ranpreprocessing=false;
	ranobjectrecognition=false;
	updatebuttons();
}

TED::~TED()
{
}

// TED Actions
void TED::createActions()
{
	// Create a new Signature
	createsignatureAction=new QAction(tr("&Neue Signatur erstellen"), this);
	createsignatureAction->setShortcut(tr("Ctrl+O"));
	createsignatureAction->setIcon(QIcon("icons/book.png"));
	connect(createsignatureAction, SIGNAL(triggered()), this, SLOT(createSignature()));

	// Open existing Signature
	opensignatureAction=new QAction(tr("&Signatur oeffnen"), this);
	opensignatureAction->setShortcut(tr("Ctrl+O"));
	opensignatureAction->setIcon(QIcon("icons/book_go.png"));
	connect(opensignatureAction, SIGNAL(triggered()), this, SLOT(openexistingSignature()));

	// Load an Image
	loadimageAction=new QAction(tr("&Digitalisat hinzufuegen"), this);
	loadimageAction->setShortcut(tr("Ctrl+O"));
	loadimageAction->setIcon(QIcon("icons/picture_add.png"));
	connect(loadimageAction, SIGNAL(triggered()), this, SLOT(loadImage()));

	// Exit TED
	exitAction=new QAction(tr("&Beenden"), this); 
	exitAction->setShortcut(tr("Ctrl+Q")); 
	exitAction->setIcon(QIcon("icons/door_open.png"));
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

	// Displays general / legal information about TED
	aboutTEDAction=new QAction(tr("Ueber &TED"), this); 
	aboutTEDAction->setShortcut(tr("Ctrl+A")); 
	aboutTEDAction->setIcon(QIcon("icons/comment.png"));
	connect(aboutTEDAction, SIGNAL(triggered()), this, SLOT(aboutTED()));

	// Displays Information about Qt
	aboutQtAction=new QAction(tr("Ueber &Qt"), this); 
	aboutQtAction->setShortcut(tr("Ctrl+T"));
	aboutQtAction->setIcon(QIcon("icons/qticon.png"));
	connect(aboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQt()));

	// Processing Toolbar Action: Preprocess
	preprocessingAction=new QAction(tr("Digitalisat vorbereiten"), this);
	preprocessingAction->setIcon(QIcon("icons/cog.png"));
	connect(preprocessingAction, SIGNAL(triggered()), this, SLOT(preprocessinghelper()));

	// Processing Toolbar Action: Object Detection
	ocrAction=new QAction(tr("Glyphenisolierung"), this);
	ocrAction->setIcon(QIcon("icons/page_edit.png"));
	connect(ocrAction, SIGNAL(triggered()), this, SLOT(objectdetecthelper()));

	// Processing Toolbar Action: Object clustering
	nnkohonenAction=new QAction(tr("Glyphenzuordnung"), this);
	nnkohonenAction->setIcon(QIcon("icons/cup.png"));
	connect(nnkohonenAction, SIGNAL(triggered()), this, SLOT(nnkohonenhelper()));

	// Processing Toolbar Action: Perform Signature-wide OCR
	signaturewideclusteringAction=new QAction(tr("Signaturweite OCR"), this);
	signaturewideclusteringAction->setIcon(QIcon("icons/sitemap.png"));
	connect(signaturewideclusteringAction, SIGNAL(triggered()), this, SLOT(performsignaturewideclustering()));

	// Processing Toolbar Action: Outputnone
	outputAction=new QAction(tr("Ausgabe speichern"), this);
	outputAction->setIcon(QIcon("icons/layout_content.png"));
	connect(outputAction, SIGNAL(triggered()), this, SLOT(outputhelper()));

	// Image / object clicked
	connect(view, SIGNAL(printcoordinate(QPoint)), this, SLOT(objectclicked(QPoint)));

	// Object Dialogue Stuff
	connect(objectinfodlg, SIGNAL(accepted()), this, SLOT(saveobjectinfodlgdata()));
	connect(objectinfodlg, SIGNAL(rejected()), this, SLOT(saveobjectinfodlgdata()));
	connect(objectinfodlg, SIGNAL(repaintview()), this, SLOT(repaintmainview()));
	connect(objectinfodlg, SIGNAL(printcurnegativewinindex(int negindex)), this, SLOT(setcurwinindex(int negwinindex)));

	// Settings
	settingsAction=new QAction(tr("Einstellungen"), this);
	settingsAction->setIcon(QIcon("icons/wrench.png"));
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(settingshelper()));
}

// TED Menus
void TED::createMenus()
{
	fileMenu=menuBar()->addMenu(tr("&Datei"));
	fileMenu->addAction(createsignatureAction);
	fileMenu->addAction(opensignatureAction);
	fileMenu->addSeparator(); 
	fileMenu->addAction(exitAction);

	fileMenu=menuBar()->addMenu(tr("&Arbeitsschritte"));
	fileMenu->addAction(loadimageAction);
	fileMenu->addSeparator(); 
	fileMenu->addAction(preprocessingAction);
	fileMenu->addAction(ocrAction);
	fileMenu->addAction(nnkohonenAction);
	fileMenu->addSeparator(); 
	fileMenu->addAction(signaturewideclusteringAction);
	fileMenu->addSeparator(); 
	fileMenu->addAction(outputAction);

	fileMenu=menuBar()->addMenu(tr("&Einstellungen"));
	fileMenu->addAction(settingsAction);

	helpMenu=menuBar()->addMenu(tr("&Hilfe"));
	helpMenu->addAction(aboutTEDAction);
	helpMenu->addSeparator();
	helpMenu->addAction(aboutQtAction);
}

// Create Status Bar
void TED::createStatusBar()
{
	QStatusBar *statusBar;
	statusBar=new QStatusBar;

	// Add Signature Information
	QLabel *label=new QLabel(tr("<strong>Signatur:</strong> "));
	statusBar->addPermanentWidget(label);
	signaturenamelabel=new QLabel(" ");
	statusBar->addPermanentWidget(signaturenamelabel);

	// Add File Information
	QLabel *l=new QLabel("<strong>Bild:</strong> ");
	statusBar->addPermanentWidget(l);
	currentfile=new QLabel(" ");
	statusBar->addPermanentWidget(currentfile);
	setStatusBar(statusBar);
}

// TED Processing Bar: Holds predefined Actions
void TED::createProcessingbar()
{
	// Generate Processing Toolbar
	processingbar=addToolBar(tr("Digitalisat vorbereiten"));
	processingbar->setAllowedAreas(Qt::TopToolBarArea);
	processingbar->setMovable(false);
	processingbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	addToolBar(Qt::TopToolBarArea, processingbar);

	// Add Processing Toolbar Actions
	
	// #0 Create / open Signature
	processingbar->addAction(createsignatureAction);
	processingbar->addAction(opensignatureAction);

	processingbar->addSeparator();

	// #1 Add Image
	processingbar->addAction(loadimageAction);

	processingbar->addSeparator();

	// #2 Preprocess Image
	processingbar->addAction(preprocessingAction);

	// #3 Perform object recognition
	processingbar->addAction(ocrAction);

	// #4 Create Kohonen Network / Map
	processingbar->addAction(nnkohonenAction);

	processingbar->addSeparator();

	// #4 Perform Signature-wide clustering
	processingbar->addAction(signaturewideclusteringAction);

	processingbar->addSeparator();

	// #5 Create XHTML Output
	processingbar->addAction(outputAction);
}

// TED Output Console (show, what TED does)
void TED::createOutputConsole()
{
	outputconsole=new QDockWidget();
	addDockWidget(Qt::BottomDockWidgetArea, outputconsole);
	outputconsole->setWindowTitle("Ausgabe");
	outputconsole->setAllowedAreas(Qt::BottomDockWidgetArea);
	outputconsole->setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetMovable);
	outputconsole->setMinimumSize(35, 30);

	// Use QTextEdit for printing output
	writeonconsole=new QTextEdit();
	writeonconsole->setReadOnly(true);
	outputconsole->setWidget(writeonconsole);
	writeonconsole->clear();
}

void TED::createsignatureimagesdock()
{
	// Browse Signature Images
	signatureimages=new QDockWidget();
	addDockWidget(Qt::RightDockWidgetArea, signatureimages);
	signatureimages->setWindowTitle("Signaturbrowser");
	signatureimages->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
	signatureimages->setMinimumSize(175, 175);
	signatureimages->setFeatures(QDockWidget::NoDockWidgetFeatures | QDockWidget::DockWidgetMovable);

	signaturebrowserscroll=new QScrollArea();
	dockWidgetContents=new QWidget(signaturebrowserscroll);
	dockWidgetContents->setLayout(new QVBoxLayout());

	signaturebrowserscroll->setWidget(dockWidgetContents);
	signaturebrowserscroll->setWidgetResizable(true);
	signaturebrowserscroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	signaturebrowserscroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	signaturebrowserscroll->widget()->layout()->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

	QLayout* outerLayout=new QVBoxLayout();
	outerLayout->addWidget(signaturebrowserscroll);

	dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
	signatureimages->setWidget(signaturebrowserscroll);
}

void TED::createSignature(void)
{
	// New Project = New Signature
	bool ok;

	// View empty image
	QImage tempimage(1, 1, QImage::Format_RGB32);
	tempimage.fill(qRgb(255, 255, 255));
	QPixmap pixmap=pixmap.fromImage(tempimage, 0);
	view->setPixmap(pixmap);
	view->resize(view->sizeHint());

	// Start new Signature
	QString tempdescription=tr("Bitte geben Sie den Namen der neu zu erstellenden Signatur an.");
	tempdescription+="<br />";
	tempdescription+=tr("W‰hlen Sie anschlieﬂend das Projektverzeichnis.");
	signaturename=QInputDialog::getText(this, tr("Name der Signatur"),
		tempdescription, QLineEdit::Normal, tr("Signatur"), &ok);

	if (ok && !signaturename.isEmpty())
	{
		// Clean Up Signaturebrowser
		for (int i=0; i < labelwidgetcontainer.size(); ++i)
		{
			labelwidgetcontainer.at(i)->hide();
			signaturebrowserscroll->widget()->layout()->removeWidget(labelwidgetcontainer.at(i));
			//delete labelwidgetcontainer.at(i);
		}
		labelwidgetcontainer.clear();

		signaturenamelabel->setText(signaturename);

		// New Project = New Signature = Which working directory?
		do
		{
			// New Signature: Create Directory and Index File
			workdirectory=QFileDialog::getExistingDirectory(this, tr("Bitte waehlen Sie ein Projektverzeichnis"),
				"/home",
				QFileDialog::ShowDirsOnly);
		}while(workdirectory=="");

		// Create a new project file
		projectindexfile=workdirectory;
		projectindexfile+="/";
		projectindexfile+=signaturename;
		projectindexfile+=".ted";
		QFile file_projectindex(projectindexfile);
		file_projectindex.open(QIODevice::WriteOnly);
		QTextStream out_file_projectindex(&file_projectindex);
		out_file_projectindex << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		out_file_projectindex << "<project name=\"" << signaturename << "\"";
		out_file_projectindex << " workdirectory=\"" << workdirectory << "\"";
		QDateTime datenow=QDateTime::currentDateTime();
		QString curdate=datenow.toString();
		out_file_projectindex << " created=\"" << curdate << "\">\n";
		out_file_projectindex << "</project>";
		file_projectindex.close();

		file_projectindex.open(QIODevice::ReadOnly);
		// Load New Project File into RAM
		projectxml.setContent(&file_projectindex);
		file_projectindex.close();

		QLabel *tempsignaturename=new QLabel;
		QString helpertemp="<b>";
		helpertemp+=signaturename;
		helpertemp+="</b>";
		tempsignaturename->setText(helpertemp);
		signaturebrowserscroll->widget()->layout()->addWidget(tempsignaturename);

		gotsignature=true;
		updatebuttons();
	}
}

void TED::openexistingSignature(void)
{
	filename=QFileDialog::getOpenFileName(this,
		"÷ffnen", ".", "Projektdateien (*.ted)");
	if(filename.isEmpty() == true)
	{
		//QMessageBox::warning(this, "Achtung", "Sie haben keine Projektdatei ausgew‰hlt", "Weiter");
	}
	else
	{
		// Count Project Elements (there must be an easier way to do this...) :)
		QDomElement tmpdocElem=projectxml.documentElement();
		QDomNode tmpnode=tmpdocElem.firstChild();
		int countimgfiles=0;
		while((!tmpnode.isNull()))
		{
			QDomElement e=tmpnode.toElement();
			if(!e.isNull())
			{
				countimgfiles++;
			}
			tmpnode=tmpnode.nextSibling();
		}
		// Init progressDialog
		QProgressDialog progressdialog("Lade Projektdatei", "Abbrechen", 0, countimgfiles);
		progressdialog.setWindowTitle("Lade Projektdatei");
		progressdialog.setWindowModality(Qt::WindowModal);
		progressdialog.show();
		progressdialog.setValue(0);

		// Clean Up Signaturebrowser
		for (int i=0; i < labelwidgetcontainer.size(); ++i)
		{
			labelwidgetcontainer.at(i)->hide();
			signaturebrowserscroll->widget()->layout()->removeWidget(labelwidgetcontainer.at(i));
			//delete labelwidgetcontainer.at(i);
        }
		labelwidgetcontainer.clear();

		projectindexfile=filename;
		QFile file_projectindex(projectindexfile);
		file_projectindex.open(QIODevice::ReadOnly);
		
		// Load New Project File into RAM
		projectxml.setContent(&file_projectindex);
		file_projectindex.close();

		int counter=0;

		activeimageid="";
		QDomElement docElem=projectxml.documentElement();
		QDomAttr cursigname=docElem.attributeNode("name");
		signaturenamelabel->setText(cursigname.value());
		signaturename=cursigname.value();
		QDomAttr curworkdir=docElem.attributeNode("workdirectory");
		workdirectory=curworkdir.value();
		QDomNode node=docElem.firstChild();
		while((!node.isNull()))
		{
			progressdialog.setValue(counter);
			QDomElement e=node.toElement();
			if(!e.isNull())
			{
				
				QDomAttr currentimageid=e.attributeNode("imageid");
				QDomAttr filepath=e.attributeNode("src");
				QImage image;
				image.load(filepath.value());
				QPixmap pixmap=pixmap.fromImage(image, 0);
		
				QLabel *previewimage=new class signaturebrowseitem;
				previewimage->setObjectName(currentimageid.value());
				
				// Connect dynamically created class signaturebrowseitem with main app
				connect(previewimage, SIGNAL(changeimage(QString)), this, SLOT(setactiveimage(QString)));
				QImage tempimage=image.scaled(QSize(150, 125), Qt::KeepAspectRatio, Qt::SmoothTransformation);
				QPixmap temppixmap=temppixmap.fromImage(tempimage, 0);
				previewimage->setPixmap(temppixmap);
				labelwidgetcontainer.push_back(previewimage);
				signaturebrowserscroll->widget()->layout()->addWidget(previewimage);

				labelwidgetcontainer.push_back(previewimage);

				QLabel *previewname=new QLabel;
				QFileInfo fi(filepath.value());
				previewname->setText(fi.fileName());
				signaturebrowserscroll->widget()->layout()->addWidget(previewname);
				labelwidgetcontainer.push_back(previewname);

				activeimageid=currentimageid.value();
				counter++;
			}
			node=node.nextSibling();
		}
		if(activeimageid!="")
			setactiveimage(activeimageid);
		else
		{
			// View empty image
			QImage tempimage(1, 1, QImage::Format_RGB32);
			tempimage.fill(qRgb(255, 255, 255));
			QPixmap pixmap=pixmap.fromImage(tempimage, 0);
			view->setPixmap(pixmap);
			view->resize(view->sizeHint());
		}
		view->resize(view->sizeHint());
		gotsignature=true;
		updatebuttons();
	}
}

void TED::updatebuttons(void)
{
	loadimageAction->setEnabled(false);
	ocrAction->setEnabled(false);
	outputAction->setEnabled(false);
	nnkohonenAction->setEnabled(false);
	preprocessingAction->setEnabled(false);
	signaturewideclusteringAction->setEnabled(false);

	if(gotsignature==true)
	{
		loadimageAction->setEnabled(true);
		ocrAction->setEnabled(false);
		outputAction->setEnabled(false);
		nnkohonenAction->setEnabled(false);
		preprocessingAction->setEnabled(false);
		signaturewideclusteringAction->setEnabled(false);
	}

	if(gotimage==true)
	{
		loadimageAction->setEnabled(true);
		preprocessingAction->setEnabled(true);
		ocrAction->setEnabled(false);
		nnkohonenAction->setEnabled(false);
		outputAction->setEnabled(false);
		signaturewideclusteringAction->setEnabled(false);
	}

	if(ranpreprocessing==true)
	{
		loadimageAction->setEnabled(true);
		preprocessingAction->setEnabled(false);
		ocrAction->setEnabled(true);
		nnkohonenAction->setEnabled(false);
		outputAction->setEnabled(false);
		signaturewideclusteringAction->setEnabled(false);
	}

	if(ranobjectrecognition==true)
	{
		loadimageAction->setEnabled(true);
		preprocessingAction->setEnabled(false);
		ocrAction->setEnabled(false);
		nnkohonenAction->setEnabled(true);
		outputAction->setEnabled(false);
		signaturewideclusteringAction->setEnabled(false);
	}

	if(ranclustering==true)
	{
		loadimageAction->setEnabled(true);
		preprocessingAction->setEnabled(false);
		ocrAction->setEnabled(false);
		nnkohonenAction->setEnabled(false);
		outputAction->setEnabled(true);
		signaturewideclusteringAction->setEnabled(true);
	}
}

// Load an image on TED's Canvas
void TED::loadImage(void)
{
	QPixmap pixmap;
		
	filename=QFileDialog::getOpenFileName(this,
		"Oeffnen", ".", "Bilddateien (*.png *.xpm *.jpg *.tif *.tiff)");
	if(filename.isEmpty() == true)
	{
		//QMessageBox::warning(this,"Warning", "You didn't choose an image", "Proceed");
	}

	if (image.load(filename))
	{
		pixmap=pixmap.fromImage(image, 0);

		imageid=1;
		// Set unique image id
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
				if(QString::number(imageid)!=currentimageid.value())
				{
					stop=true;
				}
				else
					imageid++;
			}
			else stop=true;
			node=node.nextSibling();
		}
		activeimageid=QString::number(imageid);
		
		QLabel *previewimage=new class signaturebrowseitem;
		previewimage->setObjectName(QString::number(imageid));
		// Connect dynamically created class signaturebrowseitem with main app
		connect(previewimage, SIGNAL(changeimage(QString)), this, SLOT(setactiveimage(QString)));
		QImage tempimage=image.scaled(QSize(150, 125), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QPixmap temppixmap=temppixmap.fromImage(tempimage, 0);
		previewimage->setPixmap(temppixmap);
		labelwidgetcontainer.push_back(previewimage);
		signaturebrowserscroll->widget()->layout()->addWidget(previewimage);

		QLabel *previewname=new QLabel;
		QFileInfo fi(filename);
		previewname->setText(fi.fileName());
		labelwidgetcontainer.push_back(previewname);
		signaturebrowserscroll->widget()->layout()->addWidget(previewname);

		view->setPixmap(pixmap);
		view->resize(view->sizeHint());
		view->setLineWidth(4);
		currentfile->setText(filename);

		writeonconsole->append(tr("<strong>Bild geoeffnet: </strong>"));
		writeonconsole->append(filename);

		// Set control variables
		gotimage=true;
		createdxml=false;
		gotsignature=true;
		ranclustering=false;
		ranpreprocessing=false;
		ranobjectrecognition=false;
		updatebuttons();

		// Copy Image and convert it to RGB
		image=image.convertToFormat(QImage::Format_RGB32);
		cpyimage=image;

		// Save new image to project file
		docElem=projectxml.documentElement();
		QDomElement elem=projectxml.createElement("img");
		elem.setAttribute("src", filename);
		elem.setAttribute("imageid", QString::number(imageid));
		elem.setAttribute("status", "0");
		elem.setAttribute("objects", "0");
		docElem.appendChild(elem);

		QString rawxml=projectxml.toString();
		QFile file_projectindexfile(projectindexfile);
		file_projectindexfile.open(QIODevice::WriteOnly);
		QTextStream out_file_projectindexfile(&file_projectindexfile);
		out_file_projectindexfile << rawxml;
		file_projectindexfile.close();
	   }
	else if(!filename.isEmpty())
	{
		writeonconsole->append(tr("Warning! Image seems to be corrupted."));
		QMessageBox::warning(this,"Warning",
	                               "This Image seems to be corrupted",
	                               "Proceed");
	}
}

void TED::preprocessinghelper(void)
{
	clock_t start, end;
    double cpu_time_used;
	QString ergebnis="";

	QImage originalimage=image;
	// Tell the Console what TED's doing
	writeonconsole->append(tr("<br /><strong>Starte Vorverarbeitung</strong>"));

	// Perform Histogram Expansion on each RGB Channel
	if(expandhistogramrgb==true)
	{
		writeonconsole->append(tr("Histogrammausgleich (RGB)..."));
		histogramexpansionrgb(&image);
		writeonconsole->append(tr("Fertig."));
	}

	// Superficial Layout Analysis
	//QImage ornamentimage=image;
	//isolatedecoration(&image, &ornamentimage);
	//nextneighbours3x3(&ornamentimage, 3);
	
	// 1.) Convert Image Information to Grayscales
	writeonconsole->append(tr("Reduziere Farbinformation auf Graustufen..."));
	start=clock();
	convert2grayscale(&image);
	end=clock();
	cpu_time_used=((double) (end - start)) / CLOCKS_PER_SEC;
	ergebnis=QString::number(cpu_time_used);
	//QMessageBox::warning(this, "Grayscales", ergebnis, "Proceed");
	writeonconsole->append(tr("Fertig."));

	// Cut Image
	QImage copyimage=image;
	if(setcut==true)
	{
		writeonconsole->append(tr("Zuschneiden des Digitalisates..."));
		simplethresholding(&image, 155);
		image=cutrelevant(&image, &copyimage);
		view->setPixmap(QPixmap::fromImage(image));
		view->resize(view->sizeHint());
		writeonconsole->append(tr("Fertig."));
	}

	// Histogramexpansion
	if(expandhistogram==true)
	{
		//histogramexpansionsimple(&image);
		histogramexpansion(&image);
	}

	// 2.) Suppress noise by filtering median (3x3 matrix)
	if(setmedian==true)
	{
		writeonconsole->append(tr("Median Filter (3x3)..."));
		filtermedian3x3(&image);
		writeonconsole->append(tr("Fertig."));
	}

	// 2.a) Variation of median filter: ACWM; Adaptive Center Weighted Median Filter
	// ACWM: very efficient suppressing of (many, many) salt-and-pepper noise

	if(setadaptivemedian==true)
	{
		writeonconsole->append(tr("Adaptiver Median Filter..."));
		adaptivemedian(&image, 7);
		writeonconsole->append(tr("Fertig."));
	}

	// 3.) Thresholding / Binarization: Reduce Image Information to Monochrome
	writeonconsole->append(tr("<strong>Schwellwertverfahren</strong>"));

	// Set Thresholding Method
	switch(thresholdingmethod)
	{
	case 0:
		//3.a) Simple Threshold Method
		writeonconsole->append(tr("Globaler Schwellwert..."));
		simplethresholding(&image, 155);
		writeonconsole->append(tr("Fertig."));
		break;
	case 1:
		// 3.b) P-Tile Algorithm (based on histogram)
		writeonconsole->append(tr("P-Tile Methode..."));
		thr_percent(&image, 5);
		writeonconsole->append(tr("Fertig."));
		break;
	case 2:
		// 3.c) Using Edge Pixels: Laplacian Method
		writeonconsole->append(tr(" Eckpixel (Laplace)..."));
		thr_lap(&image);
		writeonconsole->append(tr("Fertig."));
		break;
	case 3:
		// 3.d) Iterative selection (Ridler 1978)
		writeonconsole->append(tr(" Iterativ (Ridler)..."));
		thr_is(&image);
		writeonconsole->append(tr("Fertig."));
		break;
	case 4:
		// 3.e) Using Grey Level Histograms (Otsu 1978)
		writeonconsole->append(tr(" Otsu..."));
		thr_glh(&image);
		writeonconsole->append(tr("Fertig."));
		break;
	case 5:
		// 3.f) Entropy Based Thresholding (Pun 1981)
		writeonconsole->append(tr(" Entropiebasiert: Pun..."));
		thr_pun(&image);
		writeonconsole->append(tr("Fertig."));
		break;
	case 6:
		// 3.g) Entropy Based: Kapur (Kapur 1985)
		writeonconsole->append(tr(" Entropiebasiert: Kapur..."));
		thr_kapur(&image);
		writeonconsole->append(tr("Fertig."));
		break;
	case 7:
		// 3.h) Johannsen Algorithm
		writeonconsole->append(tr(" Johannsen Algorithmus..."));
		thr_joh(&image);
		writeonconsole->append(tr("Fertig."));
		break;
	case 8:
		// 3.i) Fuzzy: Entropy
		writeonconsole->append(tr(" Fuzzy: Entropie..."));
		thr_fuzz_entropy(&image);
		writeonconsole->append(tr("Fertig."));
		break;
	case 9:
		// 3.j) Fuzzy: Yager
		writeonconsole->append(tr(" Fuzzy: Yager..."));
		thr_fuzz_yager(&image);
		writeonconsole->append(tr("Fertig."));
		break;
	case 10:
		// 3.k) Minimum Error Thresholding
		writeonconsole->append(tr(" Minimum Error..."));
		thr_me(&image);
		writeonconsole->append(tr("Fertig."));
		break;
	case 11:
		// 3.l) Relaxation Method Thresholding
		writeonconsole->append(tr(" Relaxation Methode (3)..."));
		thr_relax(&image);
		writeonconsole->append(tr("Fertig."));
		break;
	default:
		break;
	}

	// 3. Reduce Noise
	if(setkfill==true)
	{
		writeonconsole->append(tr("Entferne Salz-/Pfeffer Rauschen: kfilter..."));		
		k_fill(&image, 3);
		writeonconsole->append(tr("Fertig"));
	}

	if(seteroding==true)
	{
		// 4. Erode uninteresting Pixels
		 //Set Pixels with less than 2 neighbors white (not really needed, if thresholding algorithm
		 //produces good output...
		writeonconsole->append(tr("Erodierung 3x3 #1..."));
		nextneighbours3x3(&image, 1);
		writeonconsole->append(tr("Fertig"));

		writeonconsole->append(tr("Erodierung 3x3 #2..."));
		nextneighbours3x3(&image, 2);
		writeonconsole->append(tr("Fertig"));
	}
	cpyimage=image;
	view->setPixmap(QPixmap::fromImage(image));
	view->resize(view->sizeHint());

	ranpreprocessing=true;
	updatebuttons();
	saveprocessingstatus("1");
	saveprocessedimage(&image, "1");
}

void TED::objectdetecthelper(void)
{
	writeonconsole->append(tr("Kantenerkennung..."));
	grad1(&image);
	writeonconsole->append(tr("Fertig"));

	writeonconsole->append(tr("Generiere Bounding Boxes..."));

	writeonconsole->append(tr("Luke Pathwalker..."));
	int found=findobjects(&image, &cpyimage, workdirectory, signaturename,
		QString::number(imageid), patternsizeX, patternsizeY);
	setobjectcount(found);
	QString foundobjects=QString::number(found);
	writeonconsole->append(tr("Fertig"));

	writeonconsole->append(foundobjects);
	writeonconsole->append(tr("Objekte gefunden"));

	view->setPixmap(QPixmap::fromImage(cpyimage));

	if(foundobjects>0)
		ranobjectrecognition=true;
	updatebuttons();
	saveprocessingstatus("2");
	saveobjectcount(foundobjects);
	saveprocessedimage(&cpyimage, "2");
	saveprocessedimage(&cpyimage, "3");
}

void TED::nnkohonenhelper(void)
{
	writeonconsole->append(tr("Entfalte Kohonenkarte..."));
	QString workdir=workdirectory;
	workdir+="/objectdb_";
	workdir+=signaturename;
	workdir+="/";

	// -----------------------------------------
	//	Read in initial values for alpha, and the
	//  neighborhood size. 
	//  Both of these parameters are decreased with
	//  time. The number of cycles to execute before
	//  decreasing the value of these parameters is
	//	called the period. Read in a value for the
	//	period.
	// -----------------------------------------
	// alpha (0.01-1.0)
	// neighborhood size (integer between 0 and 50)

	// period, which is the number of cycles after which the values
	// for alpha the neighborhood size are decremented
	
	// Read in the maximum number of cycles
	// each pass through the input data file is a cycle
	// Good results for Patternsize 25x25px: nnkohonen(workdir, 625, 1250, 0.5, 50, 50, 5);
	// Perform fast, sketchily clustering:
	nnkohonen(workdir, QString::number(imageid), (patternsizeX*patternsizeY),
		(patternsizeX*patternsizeY), kohonenalpha, kohonenneighborhood, kohonenperiod,
		kohonenmaxcycles, objectcount);

	// Write Winner Indices into index.xml
	QString workdirxml=workdirectory;
	workdirxml+="/objectdb_";
	workdirxml+=signaturename;
	workdirxml+="/";
	workdirxml+=QString::number(imageid);
	workdirxml+="_index.xml";

	QFile xmlfile(workdirxml);
	if (!xmlfile.open(QIODevice::ReadOnly))
		return;
	if (!doc.setContent(&xmlfile)) 
	{
		xmlfile.close();
		return;
	}
	xmlfile.close();
	createdxml=true;

	QString trainingfile=workdir;
	trainingfile+="/";
	trainingfile+=QString::number(imageid);;
	trainingfile+="/";
	trainingfile+="/kohonen.dat";

	QFile file(trainingfile);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	QTextStream in(&file);
	while(!in.atEnd())
	{
		QStringList fileid=(in.readLine()).split(";");

		QDomElement docElem=doc.documentElement();
		QDomNode node=docElem.firstChild();
		while(!node.isNull())
		{
			QDomElement e=node.toElement();
			if(!e.isNull())
			{
				// Traverse DomDocument to find current id
				QDomAttr glyphid=e.attributeNode("id");
				QString id=glyphid.value();
				if(id==fileid[1])
				{
					e.attributeNode("winindex").setValue(fileid[0]);
				}
			}
			node=node.nextSibling();
		}
	}
	file.close();

	// save XML
	QString rawxml=doc.toString();

	QFile file_recognized_objects(workdirxml);
	file_recognized_objects.open(QIODevice::WriteOnly);
	QTextStream out_file_recognized_objects(&file_recognized_objects);
	out_file_recognized_objects << rawxml;
	file_recognized_objects.close();

	writeonconsole->append(tr("Fertig"));
	ranclustering=true;
	updatebuttons();
	saveprocessingstatus("3");
	saveprocessedimage(&cpyimage, "3");
}

void TED::saveobjectinfodlgdata()
{
}

void TED::getobjectat(QPoint curobject)
{
	// Extract Information about chosen object from index.xml an show it in objectinfodlg
	objectinfodlg=new dlgobjectinfo;
	connect(objectinfodlg, SIGNAL(repaintview()), this, SLOT(repaintmainview()));
	
	// Open Index XML (only, if not already in RAM)
	if(createdxml==false)
	{
		QString workdir=workdirectory;
		workdir+="/objectdb_";
		workdir+=signaturename;
		workdir+="/";
		workdir+=QString::number(imageid);
		workdir+="_index.xml";

		QFile file(workdir);
		if (!file.open(QIODevice::ReadOnly))
			return;
		if (!doc.setContent(&file)) 
		{
			file.close();
			return;
		}
		file.close();
	}
	createdxml=true;

	int id=0;
	int minx, maxx, miny, maxy;
	
	QDomElement docElem=doc.documentElement();
	QDomNode node=docElem.firstChild();

	QString foundids="";
	int objectcounter=0;

	while(!node.isNull())
	{
		QDomElement e=node.toElement();
		if(!e.isNull())
		{
			QDomAttr glyphid=e.attributeNode("id");
			id=(glyphid.value()).toInt();

			QDomElement subnode=node.firstChildElement("boundingbox");
			QDomAttr a=subnode.attributeNode("minx");
			minx=(a.value()).toInt();
			a=subnode.attributeNode("maxx");
			maxx=(a.value()).toInt();
			a=subnode.attributeNode("miny");
			miny=(a.value()).toInt();
			a=subnode.attributeNode("maxy");
			maxy=(a.value()).toInt();

			// Compare click position with current object
			if(minx<=curobject.x() && maxx>=curobject.x()
				&& miny<=curobject.y() && maxy>=curobject.y())
			{
				// Object found
				foundids+=glyphid.value();
				objectcounter++;
			}
		}
		node=node.nextSibling();
	}

	if(objectcounter==1)
	{
		objectinfodlg->setobject(foundids);
		QString workdir=workdirectory;
		workdir+="/objectdb_";
		workdir+=signaturename;
		workdir+="/";
		objectinfodlg->setworkdir(workdir);
		objectinfodlg->setimgid(QString::number(imageid));
		objectinfodlg->setranclustering(ranclustering);
		objectinfodlg->setindexxml(&doc);
		objectinfodlg->setworkimage(&cpyimage);
		objectinfodlg->setcurnegativewinindex(minwinindex);
		connect(objectinfodlg, SIGNAL(printcurnegativewinindex(int)), this, SLOT(setcurwinindex(int)));
		objectinfodlg->initdlg();
	}
	view->setPixmap(QPixmap::fromImage(cpyimage));
}

void TED::outputhelper(void)
{
	// The brute-force way: xhtml with css (absolute aligned glyphs)
	QString metadirectory=workdirectory;
	metadirectory+="/objectdb_";
	metadirectory+=signaturename;
	metadirectory+="/";
	metadirectory+="output";
	QDir metadir(metadirectory);
	if(!metadir.exists())
	{
		// Create Directory
		metadir.mkpath(metadirectory);
	}
	metadirectory+="/";
	metadirectory+=activeimageid;
	QString outxhtml=metadirectory;
	outxhtml+=".htm";
	QFile file_output(outxhtml);
	file_output.open(QIODevice::WriteOnly);
	QTextStream out_file_output(&file_output);
	out_file_output << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"";
	out_file_output << "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">";
	out_file_output << "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"de\" lang=\"de\">\n";
	out_file_output << "<head>\n<title>output</title>\n";
	out_file_output << "<link rel=\"stylesheet\" href=\"" << activeimageid << ".css\" ";
    out_file_output << "type=\"text/css\" />\n</head>\n";
	out_file_output << "<body>";

	QString outcss=metadirectory;
	outcss+=".css";
	QFile file_outputcss(outcss);
	file_outputcss.open(QIODevice::WriteOnly);
	QTextStream out_file_outputcss(&file_outputcss);

	QDomElement docElem=doc.documentElement();
	QDomNode node=docElem.firstChild();

	while(!node.isNull())
	{
		QDomElement e=node.toElement();
		if(!e.isNull())
		{
			QDomElement subnode=node.firstChildElement("recognitiondata");
			if(subnode.text()!="none")
			{
				QDomElement newelement=node.toElement();
				QDomAttr glyphid=e.attributeNode("id");
				QString id=glyphid.value();

				QDomElement positionsub=node.firstChildElement("boundingbox");
				QDomAttr posleft=positionsub.attributeNode("minx");
				QString positionleft=posleft.value();
				QDomAttr posright=positionsub.attributeNode("maxy");
				QString positionright=posright.value();

				out_file_outputcss << "#object_" << id << "\n" << "{\n" << "position: absolute;\n";
				out_file_outputcss << "left: " << positionleft << "px;\n";
				out_file_outputcss << "top: " << positionright << "px;\n";
				out_file_outputcss << "}\n\n";

				// ***

				out_file_output << "<div id=\"object_" << id << "\">";
				out_file_output << subnode.text() << "</div>";
			}
		}
		node=node.nextSibling();
	}
	out_file_output << "</body></html>";
	file_output.close();
	file_outputcss.close();
}

void TED::setactiveimage(QString curimage)
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
			if(curimage==currentimageid.value())
			{
				QPixmap pixmap;
				QDomAttr filepath;
				QDomAttr curimageid=e.attributeNode("imageid");
				imageid=(curimageid.value()).toInt();
				activeimageid=curimageid.value();
				QDomAttr curstatus=e.attributeNode("status");
				QString curstat=curstatus.value();
				if(curstat!="0")
				{
					QString loadimage=workdirectory;
					loadimage+="/objectdb_";
					loadimage+=signaturename;
					loadimage+="/";
					loadimage+=curimage;
					loadimage+="/";
					loadimage+="processed_";
					loadimage+=curstatus.value();
					loadimage+=".tif";
					image.load(loadimage);
					pixmap=pixmap.fromImage(image, 0);
					cpyimage=image;

					ranpreprocessing=true;
					if(curstat=="1")
					{
						createdxml=false;
						ranclustering=false;
						ranobjectrecognition=false;
					}
					else if(curstat=="2")
					{
						ranclustering=false;
						ranobjectrecognition=true;

						// Load xml
						QString workdir=workdirectory;
						workdir+="/objectdb_";
						workdir+=signaturename;
						workdir+="/";
						workdir+=curimage;
						workdir+="_index.xml";

						QFile file(workdir);
						if (!file.open(QIODevice::ReadOnly))
							return;
						if (!doc.setContent(&file)) 
						{
							file.close();
							return;
						}
						file.close();
						createdxml=true;

						QDomAttr curobjectcount=e.attributeNode("objects");
						objectcount=(curobjectcount.value()).toInt();
					}
					else if(curstat=="3")
					{
						ranclustering=true;
						ranobjectrecognition=true;

						// Load xml
						QString workdir=workdirectory;
						workdir+="/objectdb_";
						workdir+=signaturename;
						workdir+="/";
						workdir+=curimage;
						workdir+="_index.xml";

						QFile file(workdir);
						if (!file.open(QIODevice::ReadOnly))
							return;
						if (!doc.setContent(&file)) 
						{
							file.close();
							return;
						}
						file.close();
						createdxml=true;

						QDomAttr curobjectcount=e.attributeNode("objects");
						objectcount=(curobjectcount.value()).toInt();
					}
					else
					{
						createdxml=false;
						ranclustering=false;
						ranpreprocessing=false;
						ranobjectrecognition=false;
					}
				}
				else
				{
					filepath=e.attributeNode("src");
					image.load(filepath.value());
					pixmap=pixmap.fromImage(image, 0);

					createdxml=false;
					ranclustering=false;
					ranpreprocessing=false;
					ranobjectrecognition=false;
				}
				view->setPixmap(pixmap);
				view->resize(view->sizeHint());
				view->setLineWidth(4);
				currentfile->setText(filename);

				// Check processing status
				gotimage=true;
				gotsignature=true;
				updatebuttons();

				QFileInfo fi(filepath.value());
				currentfile->setText(fi.fileName());

				stop=true;
			}
		}
		node=node.nextSibling();
	}
}

void TED::performsignaturewideclustering(void)
{
	// Prepare signature data
	QString globalclusterfile=workdirectory;
	globalclusterfile+="/objectdb_";
	globalclusterfile+=signaturename;
	globalclusterfile+="/mlptrain.xml";
	QFile file_globalcluster(globalclusterfile);
	file_globalcluster.open(QIODevice::WriteOnly);
	QTextStream out_file_globalcluster(&file_globalcluster);
	out_file_globalcluster << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	out_file_globalcluster << "<mlptraindata>\n";

	QString mlpcheckfile=workdirectory;
	mlpcheckfile+="/objectdb_";
	mlpcheckfile+=signaturename;
	mlpcheckfile+="/mlpcheck.xml";
	QFile file_mlpcheckfile(mlpcheckfile);
	file_mlpcheckfile.open(QIODevice::WriteOnly);
	QTextStream out_file_mlpcheckfile(&file_mlpcheckfile);
	out_file_mlpcheckfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	out_file_mlpcheckfile << "<mlpcheckdata>\n";

	QDomElement docElem=projectxml.documentElement();
	QDomAttr cursigname=docElem.attributeNode("name");
	QString cursignaturename=cursigname.value();
	QDomNode node=docElem.firstChild();
	int countassigned=0;
	int countunassigned=0;
	QString bpnettrain="";

	while(!node.isNull())
	{
		QDomElement e=node.toElement();
		if(!e.isNull())
		{
			QDomAttr curstatus=e.attributeNode("status");
			QString curstat=curstatus.value();
			// Process images with status==3
			if(curstat=="3")
			{
				QDomAttr currentimageid=e.attributeNode("imageid");
				QString currentimage=currentimageid.value();

				QString workdir=workdirectory;
				workdir+="/objectdb_";
				workdir+=cursignaturename;
				workdir+="/";
				workdir+=currentimage;
				workdir+="_index.xml";

				QFile file(workdir);
				if (!file.open(QIODevice::ReadOnly))
					return;

				QDomDocument tempdoc;
				if(!tempdoc.setContent(&file)) 
				{
					file.close();
					return;
				}
				else
				{
					// Let's go
					QDomElement docElem=tempdoc.documentElement();
					QDomNode node=docElem.firstChild();
					while(!node.isNull())
					{
						QDomElement e=node.toElement();
						if(!e.isNull())
						{
							QDomElement subnode=node.firstChildElement("recognitiondata");
							if(subnode.text()!="none")
							{
								QDomAttr glyphid=e.attributeNode("id");
								QString objectid=glyphid.value();
								out_file_globalcluster << "<assigned docid=\"" << currentimage << "\" ";
								out_file_globalcluster << "objectid=\"" << objectid << "\" ";
								out_file_globalcluster << "recdata=\"" << subnode.text() << "\" />\n";
								countassigned++;
							}
							else
							{
								// Write patterns
								QDomAttr glyphid=e.attributeNode("id");
								QString objectid=glyphid.value();
								out_file_mlpcheckfile << "<unassigned docid=\"" << currentimage << "\" ";
								out_file_mlpcheckfile << "objectid=\"" << objectid << "\" ";
								out_file_mlpcheckfile << "recdata=\"" << subnode.text() << "\" />\n";
								countunassigned++;
							}
						}
						node=node.nextSibling();
					}
				}
				file.close();
			}
		}
		node=node.nextSibling();
	}
	out_file_globalcluster << bpnettrain;
	out_file_globalcluster << "</mlptraindata>\n";
	out_file_mlpcheckfile << "</mlpcheckdata>\n";
	file_globalcluster.close();
	file_mlpcheckfile.close();

	// Start MLP Networking
	QString curdir=workdirectory;
	curdir+="/objectdb_";
	curdir+=signaturename;
	curdir+="/";

	mlpnet=new class mlpnet;
	mlpnet->setworkdirectory(curdir);
	mlpnet->settrainingdata(globalclusterfile);
	mlpnet->setcheckdata(mlpcheckfile);
	mlpnet->prepareglyphs();
	mlpnet->train();
	// Ready

	QDomDocument tmpxml;

	// Save recognized glyphs in global project file
	QString mlprec=workdirectory;
	mlprec+="/objectdb_";
	mlprec+=signaturename;
	mlprec+="/";
	mlprec+="mlp_recognized.xml";

	QString curimage="";
	QString curdocid=0;
	QImage tmpimage;
	QFile mlprecfile(mlprec);
	if (!mlprecfile.open(QIODevice::ReadOnly))
		return;
	QDomDocument mlprecxml;
	if(!mlprecxml.setContent(&mlprecfile)) 
	{
		mlprecfile.close();
		return;
	}
	else
	{
		QDomElement docElem=mlprecxml.documentElement();
		QDomNode node=docElem.firstChild();
		while(!node.isNull())
		{
			QDomElement e=node.toElement();
			if(!e.isNull())
			{
				QDomAttr recdata=e.attributeNode("recdata");
				QDomAttr objectid=e.attributeNode("objectid");
				QDomAttr docid=e.attributeNode("docid");
				if(recdata.value()!="none")
				{
					QString test=docid.value();
					if(docid.value()!=curdocid)
					{
						// Save if !=0
						if(curdocid!=0)
						{
							QString rawxml=tmpxml.toString();
							QString filestring=workdirectory;
							filestring+="/objectdb_";
							filestring+=signaturename;
							filestring+="/";
							filestring+=curdocid;
							filestring+="_index.xml";

							QFile file_recognized_objects(filestring);
							file_recognized_objects.open(QIODevice::WriteOnly);
							QTextStream out_file_recognized_objects(&file_recognized_objects);
							out_file_recognized_objects << rawxml;
							file_recognized_objects.close();

							tmpimage.save(curimage, "tif", 100);
						}
						curdocid=docid.value();
						QString tmpfile=workdirectory;
						tmpfile+="/objectdb_";
						tmpfile+=signaturename;
						tmpfile+="/";
						tmpfile+=curdocid;
						curimage=tmpfile;
						tmpfile+="_index.xml";

						curimage+="/";
						curimage+="processed_3.tif";
						tmpimage.load(curimage);

						QFile tmpxmlfile(tmpfile);
						if(!tmpxmlfile.open(QIODevice::ReadOnly))
							return;

						if(!tmpxml.setContent(&tmpxmlfile)) 
						{
							tmpxmlfile.close();
							return;
						}
					}
					QDomElement tmpxmldocElem=tmpxml.documentElement();
					QDomNode tmpxmlnode=tmpxmldocElem.firstChild();
					bool stop=false;
					while(stop!=true)
					{
						if(tmpxmlnode.isNull())
							break;

						QDomElement tmpxmle=tmpxmlnode.toElement();
						if(!tmpxmle.isNull())
						{
							// Traverse DomDocument to find current id
							QDomAttr glyphid=tmpxmle.attributeNode("id");
							QString id=glyphid.value();
							if(id==objectid.value())
							{
								QDomElement subnode=tmpxmlnode.firstChildElement("recognitiondata");
								QDomNode n=subnode.firstChild();
								n.setNodeValue(recdata.value());

								// Colour Glyph
								QDomElement clrsubnode=tmpxmlnode.firstChildElement("boundingbox");
								QDomAttr a=clrsubnode.attributeNode("minx");
								int minx=(a.value()).toInt();
								a=clrsubnode.attributeNode("maxx");
								int maxx=(a.value()).toInt();
								a=clrsubnode.attributeNode("miny");
								int miny=(a.value()).toInt();
								a=clrsubnode.attributeNode("maxy");
								int maxy=(a.value()).toInt();

								for(int count=minx; count<=maxx; count++)
								{
									tmpimage.setPixel(count, miny, qRgb(0, 255, 255));
								}
								for(int count=minx; count<=maxx; count++)
								{
									tmpimage.setPixel(count, maxy, qRgb(0, 255, 255));
								}
								for(int count=miny; count<=maxy; count++)
								{
									tmpimage.setPixel(minx, count, qRgb(0, 255, 255));							
								}
								for(int count=miny; count<=maxy; count++)
								{
									tmpimage.setPixel(maxx, count, qRgb(0, 255, 255));
								}
								stop=true;
							}
						}
						tmpxmlnode=tmpxmlnode.nextSibling();
					}
				}
			}
			node=node.nextSibling();
		}
		QString rawxml=tmpxml.toString();
		QString filestring=workdirectory;
		filestring+="/objectdb_";
		filestring+=signaturename;
		filestring+="/";
		filestring+=curdocid;
		filestring+="_index.xml";

		// Save Image
		tmpimage.save(curimage, "tif", 100);

		QFile file_recognized_objects(filestring);
		file_recognized_objects.open(QIODevice::WriteOnly);
		QTextStream out_file_recognized_objects(&file_recognized_objects);
		out_file_recognized_objects << rawxml;
		file_recognized_objects.close();
	}
	mlprecfile.close();
	setactiveimage(curdocid);
}

