#ifndef DLGOBJECTINFO_H
#define DLGOBJECTINFO_H

#include <QFile>
#include <QLabel>
#include <QWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QTextStream>
#include <QScrollArea>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QDomDocument>

class dlgobjectinfo : public QWidget
{
	Q_OBJECT

signals:
	void repaintview(void);
	// Set running winindex
	void printcurnegativewinindex(int winvalue);

public:
	dlgobjectinfo();
	void setobject(QString object)
	{
		objecttoshow=object;
	}
	void setworkdir(QString workdir)
	{
		workdirectory=workdir;
	}
	void setimgid(QString imgid)
	{
		imageid=imgid;
	}
	void setranclustering(bool preclustering)
	{
		ranclustering=preclustering;
	}
	void setindexxml(QDomDocument *indexdoc)
	{
		indexxmldocument=indexdoc;
	}
	void setworkimage(QImage *workimage)
	{
		fullimage=workimage;
	}
	void setcurnegativewinindex(int winindex)
	{
		curnegativewinindex=winindex;
	}
	void inkobject(QString objectid, int colour);
	void initdlg(void);
private slots:
	void accept(void);
	void reject(void);
	// Overload Qts Close Event
	void closeEvent()
	{
		//QMessageBox::warning(this, "ObjectDLG", "Close Event", "Proceed");
	}
private:
	QImage *fullimage;

	QGroupBox *groupinformation;
	QGroupBox *grouprecognition;
	QGroupBox *grouprawpattern;

	QGridLayout *stylegrid;
	QGridLayout *patterngrid;
	QGridLayout *informationgrid;
	QGridLayout *recognitiongrid;

	QLabel *infoimagelabel;
	QLabel *objectidlabel;
	QLabel *winnerindexlabel;
	QLabel *patternstringlabel;
	QLabel *assignedlabel;
	QLabel *assigneddesclabel;
	QLabel *recognitiondesclabel;
	QLabel *recognitiondescwronglabel;

	QLineEdit *editrecognized;

	QString winindex;
	QString objecttoshow;
	QString workdirectory;
	QString patternstring;

	QImage image;
	QString imageid;

	QDomDocument *indexxmldocument;

	QScrollArea *scrollArea;
	QWidget* innerWidget;

	bool ranclustering;
	int returnvaluecolour;
	int curnegativewinindex;
	QList <QLabel*> labelwidgetcontainer;
	QList <QCheckBox*> checkboxwidgetcontainer;
};
#endif











