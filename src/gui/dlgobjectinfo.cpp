#include "dlgobjectinfo.h"

#include <QMessageBox>

dlgobjectinfo::dlgobjectinfo()
{
	// Set up Object Info DLG
	// Pre-Initialize Variables
	winindex="";
	workdirectory="";
	objecttoshow="";
	patternstring="";
	ranclustering=false;
	// ************************
	
	// Set Window behavior
	setWindowModality(Qt::ApplicationModal);

	scrollArea=new QScrollArea();
	innerWidget=new QWidget(scrollArea);
	innerWidget->setLayout(new QVBoxLayout());
	scrollArea->setWidget(innerWidget);
	scrollArea->setWidgetResizable(true);
	
	// set up outer layout containing QScrollArea
	QLayout* outerLayout=new QVBoxLayout();
	outerLayout->addWidget(scrollArea);

	// Set up groups
	groupinformation=new QGroupBox("Objekt Information");
	grouprecognition=new QGroupBox("Objekt Recognition Information");
	grouprawpattern=new QGroupBox("Verwendetes Erkennungsmuster");

	// Set up grids
	stylegrid=new QGridLayout;
	informationgrid=new QGridLayout;
	recognitiongrid=new QGridLayout;
	patterngrid=new QGridLayout;

	// Set up labels
	infoimagelabel=new QLabel("Vorschaubild");
	objectidlabel=new QLabel("Objekt-ID: ");
	winnerindexlabel=new QLabel("Winner Index (Kohonen): ");
	assignedlabel=new QLabel("Dieses Objekt wurde erkannt als:");
	assigneddesclabel=new QLabel("Bitte aendern, sofern falsch zugeordnet.");
	recognitiondesclabel=new QLabel("Diese Glyphe ist folgenden Objekten aehnlich:");
	recognitiondescwronglabel=new QLabel("Bitte waehlen Sie <b>falsch</b> zugeordnete Objekte aus.");
	patternstringlabel=new QLabel("Patternstring");
	patternstringlabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	// Set up interactive stuff
	editrecognized=new QLineEdit;
	editrecognized->setFocus();

	// Now let's go
	informationgrid->addWidget(infoimagelabel, 0, 0);
	informationgrid->addWidget(objectidlabel, 0, 1);
	informationgrid->addWidget(winnerindexlabel, 2, 1);
	groupinformation->setLayout(informationgrid);

	recognitiongrid->addWidget(assignedlabel, 0, 0);
	recognitiongrid->addWidget(editrecognized, 1, 0);
	recognitiongrid->addWidget(assigneddesclabel, 2, 0);
	grouprecognition->setLayout(recognitiongrid);

	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setMaximumSize(QSize(150, 50));

	patterngrid->addWidget(patternstringlabel, 0, 0);
	grouprawpattern->setLayout(patterngrid);

	QPushButton *button01=new QPushButton("Speichern");
	QPushButton *button02=new QPushButton("Abbrechen");

	button02->setDefault(true);
	stylegrid->addWidget(groupinformation, 0, 0, 1, 0);
	stylegrid->addWidget(grouprecognition, 1, 0, 1, 2);
	stylegrid->addWidget(recognitiondesclabel, 3, 0);
	stylegrid->addWidget(recognitiondescwronglabel, 5, 0);
	stylegrid->addWidget(button01, 6, 0);
	stylegrid->addWidget(button02, 6, 1);
	//stylegrid->addWidget(grouprawpattern, 0, 1);
	setLayout(stylegrid);

	// Define Actions	
	connect(button01, SIGNAL(clicked()), this, SLOT(accept()));
	connect(button02, SIGNAL(clicked()), this, SLOT(reject()));
}

void dlgobjectinfo::accept(void)
{
	// Save wrong recognized objects
	for(int i=0; i<checkboxwidgetcontainer.size(); ++i)
	{
		if(checkboxwidgetcontainer.at(i)->checkState()==Qt::Checked)
		{
			QString wrongid=checkboxwidgetcontainer.at(i)->text();
			QDomElement docElem=indexxmldocument->documentElement();
			QDomNode node=docElem.firstChild();
			bool stop=false;
			while(stop!=true)
			{
				if(node.isNull())
					break;

				QDomElement e=node.toElement();
				if(!e.isNull())
				{
					// Traverse DomDocument to find current id
					QDomAttr glyphid=e.attributeNode("id");
					QString id=glyphid.value();
					if(id==wrongid)
					{
						e.attributeNode("winindex").setValue(QString::number(curnegativewinindex));
						// don't cluster wrong recognized objects
						curnegativewinindex--;
						// Ink object
						inkobject(wrongid, 2);
						stop=true;
					}
				}
				node=node.nextSibling();
			}
		}
	}

	// Save content of Input Field
	QString textedittext=editrecognized->text();
	if(textedittext!="")
	{
		// Assign Text from QTextEdit to current Object (objecttoshow)
		QDomElement docElem=indexxmldocument->documentElement();
		QDomNode node=docElem.firstChild();
		while((!node.isNull()))
		{
			QDomElement e=node.toElement();
			if(!e.isNull())
			{
				// Traverse DomDocument to find current id
				QDomAttr glyphid=e.attributeNode("winindex");
				QString id=glyphid.value();
				if(id==winindex)
				{
					QDomElement subnode=node.firstChildElement("recognitiondata");
					QDomNode n=subnode.firstChild();
					n.setNodeValue(textedittext);
				}
			}
			node=node.nextSibling();
		}
	}

	// Paint correctly recognized objects
	if(textedittext!="none")
	{
		QDomElement docElem=indexxmldocument->documentElement();
		QDomNode node=docElem.firstChild();
		while(!node.isNull())
		{
			QDomElement e=node.toElement();
			if(!e.isNull())
			{
				QDomAttr winidattr=e.attributeNode("winindex");
				QString winid=winidattr.value();
				if(winid==winindex)
				{
					QDomAttr glyphid=e.attributeNode("id");
					inkobject(glyphid.value(), 1);
				}
			}
			node=node.nextSibling();
		}
	}
	// save XML
	QString rawxml=indexxmldocument->toString();
	QString filestring=workdirectory;
	filestring+=imageid;
	filestring+="_";
	filestring+="index.xml";

	QFile file_recognized_objects(filestring);
	file_recognized_objects.open(QIODevice::WriteOnly);
	QTextStream out_file_recognized_objects(&file_recognized_objects);
	out_file_recognized_objects << rawxml;
	file_recognized_objects.close();

	// Refresh mainview
	emit repaintview();
	emit printcurnegativewinindex(curnegativewinindex);
	close();
}

void dlgobjectinfo::reject(void)
{
	close();
	//QMessageBox::warning(this, "ObjectDLG", "Reject", "Proceed");
}

void dlgobjectinfo::initdlg(void)
{
	// Initialize
	QString windowtitle="Bearbeite Glyphe ";
	windowtitle+=objecttoshow;
	setWindowTitle(windowtitle);
	QString objectidhelper="Objekt ID: ";
	objectidhelper+=objecttoshow;
	objectidlabel->setText(objectidhelper);

	// Reset similiargridlayout
	scrollArea=new QScrollArea(this);
	innerWidget=new QWidget(scrollArea);
	innerWidget->setLayout(new QVBoxLayout());
	scrollArea->setWidget(innerWidget);
	scrollArea->setWidgetResizable(true);
	
	// set up outer layout containing QScrollArea
	QLayout* outerLayout = new QVBoxLayout();
	outerLayout->addWidget(scrollArea);
	stylegrid->addWidget(scrollArea, 4, 0, 1, 2);

	// Set up pattern view
	patternstringlabel->setText("");

	// Set up image view
	QString showimage=workdirectory;
	showimage+=imageid;
	showimage+="/";
	showimage+=objecttoshow;
	showimage+=".tif";
	QPixmap pixmap;
	image.load(showimage);
	pixmap=pixmap.fromImage(image, 0);
	infoimagelabel->setPixmap(pixmap);

	if(ranclustering==false)
	{
		recognitiondescwronglabel->setText("");
		recognitiondesclabel->setText("Keine Clusterinformationen verfuegbar.\nBitte starten Sie die Objektzuordnung ueber den Button \"Glyphenzuordnung\".");
	}
	else
	{
		// Pick objects with similiar winner index
		QString winindexhelper="Winner Index: ";
		
		 //Search WinIndex
		QDomElement docElem=indexxmldocument->documentElement();
		QDomNode node=docElem.firstChild();
		bool stop=false;
		while(stop!=true)
		{
			if(node.isNull())
				break;

			QDomElement e=node.toElement();
			if(!e.isNull())
			{
				// Traverse DomDocument to find current id
				QDomAttr glyphid=e.attributeNode("id");
				QString id=glyphid.value();
				if(id==objecttoshow)
				{
					winindex=e.attributeNode("winindex").value();
					stop=true;
				}
			}
			node=node.nextSibling();
		}
		winindexhelper+=winindex;
		winnerindexlabel->setText(winindexhelper);
		// End Searching winindex

		// Now search for similiar objects based on winindex
		QStringList helperstringlist;
		docElem=indexxmldocument->documentElement();
		node=docElem.firstChild();
		stop=false;
		while(!node.isNull())
		{
			QDomElement e=node.toElement();
			if(!e.isNull())
			{
				// Traverse DomDocument to find current id
				QDomAttr glyphid=e.attributeNode("winindex");
				QString id=glyphid.value();

				QDomAttr curglyphid=e.attributeNode("id");
				QString curid=curglyphid.value();

				if(id==winindex && curid!=objecttoshow)
					helperstringlist << e.attributeNode("id").value();
			}
			node=node.nextSibling();
		}
		// End Searching similiar winindices

		QString helperstring="Diese Glyphe ist aehnlich zu:\n";
		if(helperstringlist.size()==0)
			helperstring+="<none>";
		else
		{
			for(int i=0; i<helperstringlist.size(); i++)
			{
				// Dynamically create labels and checkboxes
				QString showimage=workdirectory;
				showimage+=imageid;
				showimage+="/";
				showimage+=helperstringlist.at(i).toLocal8Bit().constData();
				showimage+=".tif";

				QPixmap pixmap;
				QImage tempimage;
				tempimage.load(showimage);
				pixmap=pixmap.fromImage(tempimage, 0);

				QCheckBox *checkbox=new QCheckBox(helperstringlist.at(i).toLocal8Bit(), this);
				checkbox->setIcon(QIcon(showimage));
				checkboxwidgetcontainer.append(checkbox);
				scrollArea->widget()->layout()->addWidget(checkboxwidgetcontainer.at(i)); 
			}
		}
		recognitiondescwronglabel->setText("Bitte waehlen Sie <u><b>falsch</b></u> zugeordnete Objekte aus.");
	}

	// Show recognition information in QTextEdit
	QDomElement docElem=indexxmldocument->documentElement();
	QDomNode node=docElem.firstChild();
	bool stop=false;
	while(stop!=true)
	{
		if(node.isNull())
			break;

		QDomElement e=node.toElement();
		if(!e.isNull())
		{
			// Traverse DomDocument to find current id
			QDomAttr glyphid=e.attributeNode("id");
			QString id=glyphid.value();
			if(id==objecttoshow)
			{
				QDomElement subnode=node.firstChildElement("recognitiondata");
				editrecognized->setText(subnode.text());
				stop=true;
			}
		}
		node=node.nextSibling();
	}

	// Show Dialogue (modal)
	show();
}

void dlgobjectinfo::inkobject(QString objectid, int colour)
{
	QDomElement docElem=indexxmldocument->documentElement();
	QDomNode node=docElem.firstChild();
	
	bool stop=false;
	while(stop!=true)
	{
		if(node.isNull())
			break;

		QDomElement e=node.toElement();
		if(!e.isNull())
		{
			// Traverse DomDocument to find current id
			QDomAttr glyphid=e.attributeNode("id");
			QString id=glyphid.value();
			if(id==objectid)
			{
				int minx, maxx, miny, maxy;
				QDomElement subnode=node.firstChildElement("boundingbox");
				QDomAttr a=subnode.attributeNode("minx");
				minx=(a.value()).toInt();
				a=subnode.attributeNode("maxx");
				maxx=(a.value()).toInt();
				a=subnode.attributeNode("miny");
				miny=(a.value()).toInt();
				a=subnode.attributeNode("maxy");
				maxy=(a.value()).toInt();

				// colour object bounding box
				switch(colour)
				{
					case 0:
						// Object RED
						for(int count=minx; count<=maxx; count++)
						{
							fullimage->setPixel(count, miny, qRgb(255, 0, 0));
						}
						for(int count=minx; count<=maxx; count++)
						{
							fullimage->setPixel(count, maxy, qRgb(255, 0, 0));
						}
						for(int count=miny; count<=maxy; count++)
						{
							fullimage->setPixel(minx, count, qRgb(255, 0, 0));							
						}
						for(int count=miny; count<=maxy; count++)
						{
							fullimage->setPixel(maxx, count, qRgb(255, 0, 0));
						}
						break;
					case 1:
						// Object GREEN
						for(int count=minx; count<=maxx; count++)
						{
							fullimage->setPixel(count, miny, qRgb(0, 255, 0));
						}
						for(int count=minx; count<=maxx; count++)
						{
							fullimage->setPixel(count, maxy, qRgb(0, 255, 0));
						}
						for(int count=miny; count<=maxy; count++)
						{
							fullimage->setPixel(minx, count, qRgb(0, 255, 0));							
						}
						for(int count=miny; count<=maxy; count++)
						{
							fullimage->setPixel(maxx, count, qRgb(0, 255, 0));
						}
						break;
					case 2:
						// Object BLUE
						for(int count=minx; count<=maxx; count++)
						{
							fullimage->setPixel(count, miny, qRgb(0, 0, 255));
						}
						for(int count=minx; count<=maxx; count++)
						{
							fullimage->setPixel(count, maxy, qRgb(0, 0, 255));
						}
						for(int count=miny; count<=maxy; count++)
						{
							fullimage->setPixel(minx, count, qRgb(0, 0, 255));							
						}
						for(int count=miny; count<=maxy; count++)
						{
							fullimage->setPixel(maxx, count, qRgb(0, 0, 255));
						}
						break;
				}
				stop=true;
			}
		}
		node=node.nextSibling();
	}
}


