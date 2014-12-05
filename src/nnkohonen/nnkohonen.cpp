// pattern.cpp		V. Rao, H. Rao
// Kohonen map for pattern recognition
#include <vector>
#include <iterator>
#include <algorithm>

#include "layerk.h"
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDomDocument>

using namespace std;

const double dist_tol=0.001;

bool sortbywinindex(const QPoint &left, const QPoint &right) 
{
    return left.x() < right.x();
}

bool sortbyobjectid(const QPoint &left, const QPoint &right) 
{
    return right.x() < left.x();
}

int nnkohonen(QString workdir, QString imageid, int inputlayersize, int outputlayersize,
			  float alpha, int neighborhood_size, int period, int max_cycles, int countobjects)
{
	float avg_dist_per_cycle=0.0;
	float dist_last_cycle=0.0;
	float avg_dist_per_pattern=100.0; // for the latest cycle
	float dist_last_pattern=0.0; 
	float total_dist;
	unsigned startup;
	int patterns_per_cycle=0;

	int total_cycles, total_patterns;

	// Output stuff
	QString filestring="";
	QString patterndump="";
	QString outfilestring="";
	QFile outfile;

	// create a network object
	Kohonen_network knet;

	// the main loop
	// continue looping until the average distance is less than
	// the tolerance specified at the top of this file, or the maximum number of
	// cycles is exceeded; 
	// initialize counters
	total_cycles=0; // a cycle is once through all the input data
	total_patterns=0; // a pattern is one entry in the input data

	// get layer information
	knet.get_layer_info(inputlayersize, outputlayersize);

	// set up the network connections
	knet.set_up_network(neighborhood_size);

	// initialize the weights:
	// randomize weights for the Kohonen layer
	// note that the randomize function for the
	// Kohonen simulator generates
	// weights that are normalized to length = 1
	knet.randomize_weights();     

	// main loop
	startup=1;
	total_dist=0;

	// Save winindex and object
	std::vector <QPoint> kohonenhelper;

	// Process pre-calculated normalized patterns
	QDomDocument doc;
	QString indexfile=workdir;
	indexfile+="/";
	indexfile+=imageid;
	indexfile+="_index.xml";

	QFile file(indexfile);
	if (!file.open(QIODevice::ReadOnly))
		return 0;
	if (!doc.setContent(&file)) 
	{
		file.close();
		return 0;
	}
	file.close();

	QDomElement docElem=doc.documentElement();
	QDomNode node=docElem.firstChild();

	QProgressDialog progressdialog("Training Kohonen Network", "Cancel", 0, max_cycles);
	progressdialog.setWindowTitle("Training Kohonen Network");
	progressdialog.setWindowModality(Qt::WindowModal);
	progressdialog.show();
	progressdialog.setValue(0);

	// Process Normalized patterns in index.xml
	// Put patterns in RAM
	int *patternid=0;
	patternid=new int [countobjects];
	QString *pattern=0;
	pattern=new QString [countobjects];
	int counter=0;
	float **patternvector=0;
	patternvector=new float *[countobjects];
	
	// Extract Vectors / Patterns from XML
	while(!node.isNull())
	{
		QDomElement e=node.toElement();
		if(!e.isNull())
		{
			QDomAttr glyphid=e.attributeNode("id");
			patternid[counter]=(glyphid.value()).toInt();

			QDomElement subnode=node.firstChildElement("normlength");
			QString inputpattern=subnode.text();

			pattern[counter]=subnode.text();

			QStringList processpattern=pattern[counter].split(";");
			patternvector[counter]=new float[inputlayersize];
			for(int i=0; i<inputlayersize; i++)
				patternvector[counter][i]=(float)(processpattern[i]).toFloat();
			counter++;
		}
		// Get next training pattern from index.xml
		node=node.nextSibling();
	}

	while((avg_dist_per_pattern > dist_tol) && (total_cycles < max_cycles) || (startup==1))
	{
		// reset for each cycle
		startup=0;
		dist_last_cycle=0;
		patterns_per_cycle=0;
		kohonenhelper.clear();
		
		for(int i=0; i<countobjects; i++)
		{
			knet.get_next_vector(patternvector[i]);

			// now apply it to the Kohonen network
			knet.process_next_pattern();
			dist_last_pattern=knet.get_win_dist();

			kohonenhelper.push_back(QPoint(knet.get_win_index(), patternid[i]));

			total_patterns++;
			// gradually reduce the neighborhood size and the gain, alpha
			if (((total_cycles+1) % period) == 0)
			{
				if (neighborhood_size > 0)
					neighborhood_size --;
				knet.update_neigh_size(neighborhood_size);
				if (alpha>0.1)
					alpha -= (float)0.1;
			}
			patterns_per_cycle++;
			dist_last_cycle+=dist_last_pattern;
			knet.update_weights(alpha);
			dist_last_pattern=0;
		}
		avg_dist_per_pattern=dist_last_cycle/patterns_per_cycle;
		total_dist+=dist_last_cycle;
		total_cycles++;

		progressdialog.setValue(total_cycles);
	}

	// open writing file for writing - only save results of clustering
	outfilestring=workdir;
	outfilestring+="/";
	outfilestring+=imageid;
	outfilestring+="/";
	outfilestring+="/kohonen.dat";
	outfile.setFileName(outfilestring);
	if(!outfile.open(QIODevice::WriteOnly | QIODevice::Text))
		return 0;
	QTextStream outstream(&outfile);

	// Sort and save results of clustering
	std::sort(kohonenhelper.begin(), kohonenhelper.end(), sortbywinindex);
	//sortbyobjectid;
	for(std::vector<QPoint>::iterator helpiterator=kohonenhelper.begin();
		helpiterator!=kohonenhelper.end();
		helpiterator++)
	{
		outstream << (*helpiterator).x() << ";" << (*helpiterator).y() << "\n";
	}
	// close the output file
	outfile.close();

	avg_dist_per_cycle=total_dist/total_cycles;

	// Clean up
	delete []patternid;
	delete []pattern;
	delete []patternvector;
	patternid=0;
	pattern=0;
	patternvector=0;

	return 0;
}


