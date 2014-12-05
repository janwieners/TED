#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <QTextStream>

// Libann MLP Stuff
#include "ann.h"
#include "fm.h"
#include "mlp.h"
#include "nnmlp.h"
#include "glyph.h"
#include <map>

double 
log2(double x)
{
  return log10(x)/log10(2.0);
}

using namespace std;

void mlpnet::prepareglyphs(void)
{
	// Create ASCII Glyphs based on input patterns
	// Step 1) Prepare Training Glyphs
	QString mlptraindirectory="";
	QString charglyph="";
	QDomElement docElem=trainingxml.documentElement();
	QDomNode node=docElem.firstChild();
	while(!node.isNull())
	{
		QDomElement e=node.toElement();
		if(!e.isNull())
		{
			QDomAttr curdocid=e.attributeNode("docid");
			QDomAttr curobjectid=e.attributeNode("objectid");

			// Open Image files; write pattern into RAM
			QString tempimgstring=workdirectory;
			tempimgstring+=curdocid.value();
			tempimgstring+="/";
			mlptraindirectory=tempimgstring;
			mlptraindirectory+="mlptrain";
			tempimgstring+=curobjectid.value();
			tempimgstring+=".tif";
			QRgb *RGB;
			QImage tempimage;
			tempimage.load(tempimgstring);

			charglyph="";
			for (int y=0; y < tempimage.height(); y++)
			{
				for (int x=0; x < tempimage.width(); x++)
				{
					RGB=(QRgb *)tempimage.scanLine(y) + x;
					if(qRed(*RGB)>155)
					{
						charglyph+=".";
					}
					else
					{
						charglyph+="#";
					}
				}
				charglyph+="\n";
			}
			// If not created: create TrainingDir
			QDir dir(mlptraindirectory);
			if(!dir.exists())
			{
				// Create Directory
				dir.mkpath(mlptraindirectory);
			}
			// Save tif as ascii
			QString curmlptrainglyph=mlptraindirectory;
			curmlptrainglyph+="/";
			curmlptrainglyph+=curobjectid.value();
			curmlptrainglyph+=".char";			
			QFile file_mlptrainfile(curmlptrainglyph);
			file_mlptrainfile.open(QIODevice::WriteOnly);
			QTextStream out_file_mlptrainfile(&file_mlptrainfile);
			out_file_mlptrainfile << charglyph;
			file_mlptrainfile.close();
			// ***
		}
		node=node.nextSibling();
	}

	// Step 2) Prepare Glyphs to recognize
	QString mlptoclassifydirectory="";
	docElem=checkxml.documentElement();
	node=docElem.firstChild();
	while(!node.isNull())
	{
		QDomElement e=node.toElement();
		if(!e.isNull())
		{
			QDomAttr curdocid=e.attributeNode("docid");
			QDomAttr curobjectid=e.attributeNode("objectid");

			// Open Image files; write pattern into RAM
			QString tempimgstring=workdirectory;
			tempimgstring+=curdocid.value();
			tempimgstring+="/";
			mlptoclassifydirectory=tempimgstring;
			mlptoclassifydirectory+="mlptoclassify";
			tempimgstring+=curobjectid.value();
			tempimgstring+=".tif";
			QRgb *RGB;
			QImage tempimage;
			tempimage.load(tempimgstring);
			QImage testimg=tempimage;
			charglyph="";
			for (int y=0; y < tempimage.height(); y++)
			{
				for (int x=0; x < tempimage.width(); x++)
				{
					RGB=(QRgb *)tempimage.scanLine(y) + x;
					if(qRed(*RGB)>155)
					{
						charglyph+=".";
					}
					else
					{
						charglyph+="#";
					}
				}
				charglyph+="\n";
			}
			// If not created: create toClassifyDir
			QDir dir(mlptoclassifydirectory);
			if(!dir.exists())
			{
				// Create Directory
				dir.mkpath(mlptoclassifydirectory);
			}
			// Save tif as ascii
			QString curmlptoclassifyglyph=mlptoclassifydirectory;
			curmlptoclassifyglyph+="/";
			curmlptoclassifyglyph+=curobjectid.value();
			curmlptoclassifyglyph+=".char";			
			QFile file_mlptoclassifyfile(curmlptoclassifyglyph);
			file_mlptoclassifyfile.open(QIODevice::WriteOnly);
			QTextStream out_file_mlptoclassifyfile(&file_mlptoclassifyfile);
			out_file_mlptoclassifyfile << charglyph;
			file_mlptoclassifyfile.close();
			// ***
		}
		node=node.nextSibling();
	}
}

void mlpnet::train(void)
{
	srand(time(0));

	// Populate the feature map from the files
	ann::FeatureMap fm;
	
	// Train ASCII-Glyphs
	QString mlptraindirectory="";
	std::string *charglyph=0;
	int glyphcounter=0;
	int similiarglyphcounter=0;

	QDomElement docElem=trainingxml.documentElement();
	// Determine number of training glyphs
	QDomNodeList countnodes=docElem.childNodes();
	charglyph=new std::string [countnodes.count()];
	QString glyphname="";

	// Var. I: Only train unique glyphs
	QDomNode node=docElem.firstChild();	
	while(!node.isNull())
	{
		QDomElement e=node.toElement();
		if(!e.isNull())
		{
			QDomAttr curdocid=e.attributeNode("docid");
			QDomAttr curobjectid=e.attributeNode("objectid");
			QDomAttr curclassname=e.attributeNode("recdata");

			QString mlptrainglyph=workdirectory;
			mlptrainglyph+=curdocid.value();
			mlptrainglyph+="/";
			mlptrainglyph+="mlptrain";
			mlptrainglyph+="/";
			mlptrainglyph+=curobjectid.value();
			mlptrainglyph+=".char";

			glyphname=curclassname.value();

			// Count similiar glyphs
			similiarglyphcounter=0;
			for(int i=0; i<=glyphcounter; i++)
			{
				if(glyphname.toStdString()==charglyph[i])
					similiarglyphcounter++;
			}
			charglyph[glyphcounter]=curclassname.value().toStdString();
			const Glyph g(mlptrainglyph.toStdString());

			if(similiarglyphcounter==0)
			{
				fm.addFeature((glyphname).toStdString(), g);
				glyphcounter++;
			}
		}
		node=node.nextSibling();
	}
	// -Var. I

	// Var II: train every glyph
	//QDomNode node=docElem.firstChild();	
	//while(!node.isNull())
	//{
	//	QDomElement e=node.toElement();
	//	if(!e.isNull())
	//	{
	//		QDomAttr curdocid=e.attributeNode("docid");
	//		QDomAttr curobjectid=e.attributeNode("objectid");
	//		QDomAttr curclassname=e.attributeNode("recdata");

	//		QString mlptrainglyph=workdirectory;
	//		mlptrainglyph+=curdocid.value();
	//		mlptrainglyph+="/";
	//		mlptrainglyph+="mlptrain";
	//		mlptrainglyph+="/";
	//		mlptrainglyph+=curobjectid.value();
	//		mlptrainglyph+=".char";

	//		glyphname=curclassname.value();

	//		// Count similiar glyphs
	//		similiarglyphcounter=0;
	//		for(int i=0; i<=glyphcounter; i++)
	//		{
	//			if(glyphname.toStdString()==charglyph[i])
	//				similiarglyphcounter++;
	//		}
	//		glyphname+="_";
	//		glyphname+=QString::number(similiarglyphcounter);

	//		charglyph[glyphcounter]=curclassname.value().toStdString();

	//		const Glyph g(mlptrainglyph.toStdString());
	//		fm.addFeature((glyphname).toStdString(), g);

	//		glyphcounter++;
	//	}
	//	node=node.nextSibling();
	//}
	// - Var. II

	int classes=fm.featureSize();
	double x=log2(glyphcounter);
	int outputsize=((x-(int)x) > 0) ? (int) x+1: (int) x;
	if(outputsize==1) outputsize=2;
	//Mlp(int inputs, size of input layer
	//int outputs, size of output layer
	//float alpha, learning rate
	//float momentum, learning momentum
	//float t=0.5, tolerance of training
	//float k=1, slope of sigmoid function
	//int hSize=0 size of hidden layer (0 --> inputs/2)
	//);
	// BEST hidden layer size == classes /32
	ann::Mlp net(classes, outputsize, 1, 0, 0.5, 1, classes/32);
	net.train(fm);

	int congcnt=0;
	bool stop;
	QByteArray cmpfile1, cmpfile2;

	// Recognize Glyph
	docElem=checkxml.documentElement();
	node=docElem.firstChild();
	while(!node.isNull())
	{
		QDomElement e=node.toElement();
		if(!e.isNull())
		{
			stop=false;

			QDomAttr curdocid=e.attributeNode("docid");
			QDomAttr curobjectid=e.attributeNode("objectid");
			QDomAttr curclassname=e.attributeNode("recdata");

			QString mlptrainglyph=workdirectory;
			mlptrainglyph+=curdocid.value();
			mlptrainglyph+="/";
			mlptrainglyph+="mlptoclassify";
			mlptrainglyph+="/";
			mlptrainglyph+=curobjectid.value();
			mlptrainglyph+=".char";

			const Glyph g(mlptrainglyph.toStdString());
			std::string test=net.recall(g);

			QString recognizedas(test.c_str());

			// Now check the pixel-wise congruency of searched pattern and recognized pattern
			QDomElement traindocElem=trainingxml.documentElement();
			QDomNode trainnode=traindocElem.firstChild();
			while(stop!=true)
			{
				if(trainnode.isNull())
					break;

				QDomElement traine=trainnode.toElement();
				if(!traine.isNull())
				{
					QDomAttr traincurdocid=traine.attributeNode("docid");
					QDomAttr traincurobjectid=traine.attributeNode("objectid");
					QDomAttr traincurclassname=traine.attributeNode("recdata");

					congcnt=0;
					QString tempstring=traincurclassname.value();

					if(traincurclassname.value()==recognizedas)
					{
						// Open training glyph
						QString trainmlptrainglyph=workdirectory;
						trainmlptrainglyph+=traincurdocid.value();
						trainmlptrainglyph+="/";
						trainmlptrainglyph+="mlptrain";
						trainmlptrainglyph+="/";
						trainmlptrainglyph+=traincurobjectid.value();
						trainmlptrainglyph+=".char";

						// compare training glyph with recognized glyph
						QFile file_trainglyph(trainmlptrainglyph);
						if(file_trainglyph.open(QFile::ReadOnly))
							cmpfile1=file_trainglyph.readAll();
						QFile file_recglyph(mlptrainglyph);
						if(file_recglyph.open(QFile::ReadOnly))
							cmpfile2=file_recglyph.readAll();

						for(int i=0; i<=cmpfile1.size(); i++)
						{
							if(cmpfile1[i]==cmpfile2[i])
								congcnt++;
						}
						// Save recognition results to trainfile (process later...)
						if(congcnt<(cmpfile1.size()-(cmpfile1.size()/4)))
							curclassname.setNodeValue("none");
						else
							curclassname.setNodeValue(recognizedas);

						stop=true;
					}
				}
				trainnode=trainnode.nextSibling();
			}
		}
		node=node.nextSibling();
	}
	// Save recognition results --> XML
	QString rawxml=checkxml.toString();
	QString filestring=workdirectory;
	filestring+="mlp_recognized.xml";

	QFile file_recognized_objects(filestring);
	file_recognized_objects.open(QIODevice::WriteOnly);
	QTextStream out_file_recognized_objects(&file_recognized_objects);
	out_file_recognized_objects << rawxml;
	file_recognized_objects.close();
}

void mlpnet::recognize(void)
{
}
