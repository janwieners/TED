#include <QDir>
#include <QFile>
#include <QImage>
#include <QString>
#include <QMessageBox>
#include <QDomDocument>
#include <QProgressDialog>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

class mlpnet
{
public:
	mlpnet()
	{
	}
	~mlpnet();
	void setworkdirectory(QString workdir)
	{
		workdirectory=workdir;
	}
	void settrainingdata(QString trainingfile)
	{
		QFile file_trainingfile(trainingfile);
		file_trainingfile.open(QIODevice::ReadOnly);
		trainingxml.setContent(&file_trainingfile);
		file_trainingfile.close();
	}
	void setcheckdata(QString checkfile)
	{
		QFile file_checkfile(checkfile);
		file_checkfile.open(QIODevice::ReadOnly);
		checkxml.setContent(&file_checkfile);
		file_checkfile.close();
	}
	void train(void);
	void recognize(void);
	void prepareglyphs(void);

private:
	QDomDocument trainingxml;
	QDomDocument checkxml;
	QString workdirectory;
};


