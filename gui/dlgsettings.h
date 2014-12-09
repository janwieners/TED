#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QLabel>
#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QTextStream>
#include <QScrollArea>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QDomDocument>
#include <QRadioButton>

class dlgsettings : public QWidget
{
	Q_OBJECT

signals:
	void printcursettings(bool histogramexpansionrgbon, bool histogramexpansionon, int threshmethod,
		bool cuton, bool medianon, bool adaptivemedianon, bool kfillon,
		bool erodingon, int patternsizex, int patternsizey,
			float kohonena, int kohonenneigh, int kohonenp, int kohonenmax);

public:
	dlgsettings()
	{
	}
	dlgsettings(bool histoexprgb, bool histoexp, int thr, bool setcut, bool setmedian,
		bool setadaptivemedian, bool setkfill,
		bool seteroding, int patternsizeX, int patternsizeY,
		float kohonenalpha, int kohonenneighborhood,
		int kohonenperiod, int kohonenmaxcycles);
	void savesettings(void)
	{
		emit printcursettings(histogramexpansionrgbon, histogramexpansionon, threshmethod,
			cuton, medianon, adaptivemedianon, kfillon,
			erodingon, patternsizex, patternsizey, kohonenalpha, kohonenneighborhood,
			kohonenperiod, kohonenmaxcycles);
	}

private slots:
	void accept(void);

private:
	// Control Variables
	bool histogramexpansionrgbon;
	bool histogramexpansionon;
	int threshmethod;
	bool cuton;
	bool medianon;
	bool adaptivemedianon;
	bool kfillon;
	bool erodingon;
	int patternsizex;
	int patternsizey;
	float kohonenalpha;
	int kohonenneighborhood;
	int kohonenperiod;
	int kohonenmaxcycles;

	// Thresholding stuff
	QGroupBox *thrgroupbox;
	QVBoxLayout *thrvbox;
	QRadioButton *thrsimple;
	QRadioButton *thrptile;
	QRadioButton *thrlaplacian;
	QRadioButton *thriterativeridler;
	QRadioButton *thrglhotsu;
	QRadioButton *threntropypun;
	QRadioButton *threntropykapur;
	QRadioButton *thrjohannsen;
	QRadioButton *thrfuzzentropy;
	QRadioButton *thrfuzzyager;
	QRadioButton *thrminimumerror;
	QRadioButton *thrrelaxation;

	// Preprocessing
	QGroupBox *preprocessinggroup;
	QVBoxLayout *prebox;
	QCheckBox *qchistogramexpansionrgbon;
	QCheckBox *qchistogramexpansionon;
	QCheckBox *qccuton;
	QCheckBox *qcmedianon;
	QCheckBox *qcadmedianon;
	QCheckBox *qckfillon;
	QCheckBox *qcerodingon;

	// Object finder stuff
	QGroupBox *objectfindergroup;
	QVBoxLayout *objectfinderbox;
	QLabel *labelpatternsizex;
	QLabel *labelpatternsizey;

	// Kohonen Stuff
	QGroupBox *kohonengroup;
	QVBoxLayout *kohonenbox;
	QLabel *labelkohonenalpha;
	QLabel *labelkohonenneighborhood;
	QLabel *labelkohonenperiod;
	QLabel *labelkohonenmaxcycles;
	QLineEdit *editpatsizex;
	QLineEdit *editpatsizey;
	QLineEdit *editkohonenalpha;
	QLineEdit *editkohonenneighborhood;
	QLineEdit *editkohonenperiod;
	QLineEdit *editkohonenmaxcycles;
};
#endif





