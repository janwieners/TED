// ------------------------------------------------------
// ------------------------------------------------------
// TED :: TED Enhances Digitization 
// (c) 2008 Jan Gerrit Wieners _ jan.wieners@uni-koeln.de
// ------------------------------------------------------
// ------------------------------------------------------

#include <QtGui/QApplication>
#include "gui/tedmainwindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	TED TEDwindow;

	TEDwindow.setGeometry(100, 100, 1024, 768);
	TEDwindow.showMaximized();

	app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
	return app.exec();
}


