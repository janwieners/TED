#include "dlgsettings.h"

dlgsettings::dlgsettings(bool histoexprgb, bool histoexp, int thr, bool setcut, bool setmedian,
						 bool setadaptivemedian, bool setkfill, bool seteroding,
						 int patternsizeX, int patternsizeY,
						 float kohonenalpha, int kohonenneighborhood,
						 int kohonenperiod, int kohonenmaxcycles)
{
	// Initialization
	histogramexpansionrgbon=histoexprgb;	
	histogramexpansionon=histoexp;
	threshmethod=thr;
	cuton=setcut;
	medianon=setmedian;
	adaptivemedianon=setadaptivemedian;
	kfillon=setkfill;
	erodingon=seteroding;
	patternsizex=patternsizeX;
	patternsizey=patternsizeY;
	kohonenalpha=kohonenalpha;
	kohonenneighborhood=kohonenneighborhood;
	kohonenperiod=kohonenperiod;
	kohonenmaxcycles=kohonenmaxcycles;

	// + Thresholding stuff
	thrgroupbox=new QGroupBox("Schwellwertalgorithmen");
	thrsimple=new QRadioButton("Einfacher Schwellwertalgorithmus");
	thrptile=new QRadioButton("P-Tile");
	thrlaplacian=new QRadioButton("Laplace Methode (Kantenbasierend)");
	thriterativeridler=new QRadioButton("Iterativ (Ridler 1978)");
	thrglhotsu=new QRadioButton("Graustufenhistogramm (Otsu 1978)");
	threntropypun=new QRadioButton("Entropiebasiert (Pun 1981)");
	threntropykapur=new QRadioButton("Entropiebasiert (Kapur 1985)");
	thrjohannsen=new QRadioButton("Johannsen");
	thrfuzzentropy=new QRadioButton("Fuzzy: Entropiebasiert");
	thrfuzzyager=new QRadioButton("Fuzzy: Yager");
	thrminimumerror=new QRadioButton("Minimum Error");
	thrrelaxation=new QRadioButton("Relaxation Methode");

	thrvbox=new QVBoxLayout;
	thrvbox->addWidget(thrsimple);
	thrvbox->addWidget(thrptile);
	thrvbox->addWidget(thrlaplacian);
	thrvbox->addWidget(thriterativeridler);
	thrvbox->addWidget(thrglhotsu);
	thrvbox->addWidget(threntropypun);
	thrvbox->addWidget(threntropykapur);
	thrvbox->addWidget(thrjohannsen);
	thrvbox->addWidget(thrfuzzentropy);
	thrvbox->addWidget(thrfuzzyager);
	thrvbox->addWidget(thrminimumerror);
	thrvbox->addWidget(thrrelaxation);

	thrgroupbox->setLayout(thrvbox);

	// Set Thresholding Method
	switch(threshmethod)
	{
	case 0:
		thrsimple->setChecked(true);
		break;
	case 1:
		thrptile->setChecked(true);
		break;
	case 2:
		thrlaplacian->setChecked(true);
		break;
	case 3:
		thriterativeridler->setChecked(true);
		break;
	case 4:
		thrglhotsu->setChecked(true);
		break;
	case 5:
		threntropypun->setChecked(true);
		break;
	case 6:
		threntropykapur->setChecked(true);
		break;
	case 7:
		thrjohannsen->setChecked(true);
		break;
	case 8:
		thrfuzzentropy->setChecked(true);
		break;
	case 9:
		thrfuzzyager->setChecked(true);
		break;
	case 10:
		thrminimumerror->setChecked(true);
		break;
	case 11:
		thrrelaxation->setChecked(true);
		break;
	default:
		thrglhotsu->setChecked(true);
		break;
	}
	// - Thresholding

	// + Preprocessing
	preprocessinggroup=new QGroupBox("Vorverarbeitung");
	qchistogramexpansionrgbon=new QCheckBox("Automatischer Histogrammausgleich auf RGB Bild");
	qchistogramexpansionon=new QCheckBox("Automatischer Histogrammausgleich");
	qccuton=new QCheckBox("Digitalisat automatisch zuschneiden");
	qcmedianon=new QCheckBox("Median Filter");
	qcadmedianon=new QCheckBox("Adaptiver Median Filter (langsam)");
	qckfillon=new QCheckBox("K-Fill Filter");
	qcerodingon=new QCheckBox("Erodierung");

	prebox=new QVBoxLayout;
	prebox->addWidget(qchistogramexpansionrgbon);
	prebox->addWidget(qchistogramexpansionon);
	prebox->addWidget(qccuton);
	prebox->addWidget(qcmedianon);
	prebox->addWidget(qcadmedianon);
	prebox->addWidget(qckfillon);
	prebox->addWidget(qcerodingon);

	// Set
	qchistogramexpansionrgbon->setChecked(histogramexpansionrgbon);
	qchistogramexpansionon->setChecked(histogramexpansionon);
	qccuton->setChecked(cuton);
	qcmedianon->setChecked(medianon);
	qcadmedianon->setChecked(adaptivemedianon);
	qckfillon->setChecked(kfillon);
	qcerodingon->setChecked(erodingon);

	preprocessinggroup->setLayout(prebox);

	// - Preprocessing

	// + Objectfinder: Pattern Size
	objectfindergroup=new QGroupBox("Objekt-Extraktion");
	labelpatternsizex=new QLabel("Mustergröße: X");
	labelpatternsizey=new QLabel("Mustergröße: Y");
	editpatsizex=new QLineEdit;
	editpatsizey=new QLineEdit;
	objectfinderbox=new QVBoxLayout;
	objectfinderbox->addWidget(labelpatternsizex);
	objectfinderbox->addWidget(editpatsizex);
	objectfinderbox->addWidget(labelpatternsizey);
	objectfinderbox->addWidget(editpatsizey);

	// Set
	editpatsizex->setText(QString::number(patternsizex));
	editpatsizey->setText(QString::number(patternsizey));

	objectfindergroup->setLayout(objectfinderbox);
	// - Objectfinder: Pattern Size

	// + Kohonen Map
	kohonengroup=new QGroupBox("Kohonenkarte");
	labelkohonenalpha=new QLabel("Alpha");
	labelkohonenneighborhood=new QLabel("Nachbarschaft");
	labelkohonenperiod=new QLabel("Periode");
	labelkohonenmaxcycles=new QLabel("Maximale Anzahl an Iterationen");
	editkohonenalpha=new QLineEdit;
	editkohonenneighborhood=new QLineEdit;
	editkohonenperiod=new QLineEdit;
	editkohonenmaxcycles=new QLineEdit;
	kohonenbox=new QVBoxLayout;
	kohonenbox->addWidget(labelkohonenalpha);
	kohonenbox->addWidget(editkohonenalpha);
	kohonenbox->addWidget(labelkohonenneighborhood);
	kohonenbox->addWidget(editkohonenneighborhood);
	kohonenbox->addWidget(labelkohonenperiod);
	kohonenbox->addWidget(editkohonenperiod);
	kohonenbox->addWidget(labelkohonenmaxcycles);
	kohonenbox->addWidget(editkohonenmaxcycles);

	// Set
	editkohonenalpha->setText(QString::number(kohonenalpha));
	editkohonenneighborhood->setText(QString::number(kohonenneighborhood));
	editkohonenperiod->setText(QString::number(kohonenperiod));
	editkohonenmaxcycles->setText(QString::number(kohonenmaxcycles));

	kohonengroup->setLayout(kohonenbox);
	// - Kohonen

	// Finalize
	QGridLayout *tmpBox=new QGridLayout;
	tmpBox->addWidget(thrgroupbox, 0, 0, 0, 1);
	tmpBox->addWidget(preprocessinggroup, 0, 1);
	tmpBox->addWidget(objectfindergroup, 1, 1);
	tmpBox->addWidget(kohonengroup, 2, 1);

	QPushButton *acceptbutton=new QPushButton("Einstellungen Speichern");
	connect(acceptbutton, SIGNAL(clicked()), this, SLOT(accept()));

	tmpBox->addWidget(acceptbutton, 3, 1);

	setLayout(tmpBox);
}

void dlgsettings::accept(void)
{
	if(thrsimple->isChecked()==true)
		threshmethod=0;
	else if(thrptile->isChecked()==true)
		threshmethod=1;
	else if(thrlaplacian->isChecked()==true)
		threshmethod=2;
	else if(thriterativeridler->isChecked()==true)
		threshmethod=3;
	else if(thrglhotsu->isChecked()==true)
		threshmethod=4;
	else if(threntropypun->isChecked()==true)
		threshmethod=5;
	else if(threntropykapur->isChecked()==true)
		threshmethod=6;
	else if(thrjohannsen->isChecked()==true)
		threshmethod=7;
	else if(thrfuzzentropy->isChecked()==true)
		threshmethod=8;
	else if(thrfuzzyager->isChecked()==true)
		threshmethod=9;
	else if(thrminimumerror->isChecked()==true)
		threshmethod=10;
	else if(thrrelaxation->isChecked()==true)
		threshmethod=11;

	histogramexpansionrgbon=qchistogramexpansionrgbon->isChecked();	
	histogramexpansionon=qchistogramexpansionon->isChecked();
	cuton=qccuton->isChecked();
	medianon=qcmedianon->isChecked();
	adaptivemedianon=qcadmedianon->isChecked();
	kfillon=qckfillon->isChecked();
	erodingon=qcerodingon->isChecked();
	
	QString tempstring="";
	tempstring=editpatsizex->text();
	patternsizex=tempstring.toInt();
	tempstring=editpatsizey->text();
	patternsizey=tempstring.toInt();

	tempstring=editkohonenalpha->text();
	kohonenalpha=tempstring.toFloat();

	tempstring=editkohonenneighborhood->text();
	kohonenneighborhood=tempstring.toInt();

	tempstring=editkohonenperiod->text();
	kohonenperiod=tempstring.toInt();

	tempstring=editkohonenmaxcycles->text();
	kohonenmaxcycles=tempstring.toInt();

	savesettings();	
	close();
}


	
