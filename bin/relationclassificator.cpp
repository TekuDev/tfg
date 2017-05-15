#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <map>
#include "freeling.h"
#include "freeling/morfo/traces.h"

#include "freeling/morfo/configfile.h"
#include "featGenerator.h"
#include "event.h"
#include "relationclassificator.h"
#include "freeling/omlet/svm.h"

using namespace std;
using namespace freeling;

//https://github.com/TALP-UPC/FreeLing/blob/master/data/en/nerc/nec/nec-ab-rich.dat

relationclassificator::relationclassificator(const std::wstring &configFile)
{
    wstring lexFile,rgfFile,modelFile;
    wstring clastype, classnames;
    wstring path=configFile.substr(0,configFile.find_last_of(L"/\\")+1);
     
    // read configuration file and store information   
    enum sections {LEXICON,RGF,CLASSIFIER,MODELFILE,CLASSES,NE_TAG};
    config_file cfg;
    cfg.add_section(L"Lexicon",LEXICON);
    cfg.add_section(L"Classifier",CLASSIFIER);
    cfg.add_section(L"ModelFile",MODELFILE);
    cfg.add_section(L"Classes",CLASSES);

    if (not cfg.open(configFile))
      wcout << L"Error opening file " << configFile << endl;

    wstring line;
    while (cfg.get_content_line(line)) {

      	wistringstream sin;
		sin.str(line);

		switch (cfg.get_section()) {
			case LEXICON: {
		    	// Reading lexicon file name
		        sin>>lexFile;
		        lexFile= util::absolute(lexFile,path); 
		        break;
		    }

		    case CLASSIFIER: {
		        // Reading classifier type
		        sin>>clastype;
		        break;
		    }

		    case MODELFILE: {
		        // Reading classifier model file name
		        sin>>modelFile;
		        modelFile= util::absolute(modelFile,path); 
		        break;
		    }

		    case CLASSES: {
		        // Reading class names and numbers: e.g. "0 NP00SP0 1 NP00G00 2 NP00O00 3 NP00V00"
		        classnames=line;
		        break;
		    }

		    default: break;
		}
	}
	cfg.close();

	if (clastype==L"SVM") {
      classif = new svm(modelFile,classnames);
	}



	ifstream in;
	in.open(util::wstring2string(lexFile));
	if (not in.is_open()) cout << "error al cargar el diccionario" << endl;
	else {
		string l1,l2, freq;
		while (in >> l1 >> l2 >> freq) {
			if(stoi(freq) > 5) dic[l2] = stoi(l1);
		}
		in.close();

		/*for (auto p : dic) {
			cout << p.first << ":" << p.second << endl;
		}*/
	}

}

//Destructor
relationclassificator::~relationclassificator() {}

//Extract features and classify them.
void relationclassificator::predict(const freeling::document &doc) {
	featGenerator featGen(true, true);

	vector<string> predictions;
	vector<string> examples;

	//for (paragraf)
	list<paragraph::const_iterator> sents;
	for (document::const_iterator p=doc.begin(); p!=doc.end(); ++p)
		for (paragraph::const_iterator s=p->begin(); s!=p->end(); ++s)
           sents.push_back(s);
	
	//create events
	featGen.createEvents(sents);

	//make pairs
	list<std::pair<event,event>> pairs = featGen.getPairs();
	for (auto pi : pairs)
	{
		//cout << pi.first.id << "-" << pi.second.id << endl;
		double *pred = new double[classif->get_nlabels()];
		//create example to classify
		example exmp(classif->get_nlabels());
		//add features
		string features = featGen.generateFeatures2String(pi.first, pi.second, sents);
		list<string> featuresList = relationclassificator::split(features, ' ');
		//for (auto s : featuresList) cout << s << " ";
		
		list<pair<int,int>> feautresCoded = featGen.codeFeatures(featuresList, dic);
		for (auto fc : feautresCoded) {
			exmp.add_feature(fc.first,fc.second);
		//	cout << fc.first << ":" << fc.second << " ";
		}

		//classify
		classif->classify(exmp,pred);


		double max=pred[0]; 
        wstring tag=classif->get_label(0);
		for (int j=1; j<classif->get_nlabels(); ++j) {
			if (pred[j]>max) {
				max=pred[j];
				tag=classif->get_label(j);
			}
		}

		predictions.push_back(util::wstring2string(tag));
		examples.push_back(pi.first.id+"-"+pi.second.id+" "+pi.first.word+" "+pi.second.word);
		

		featGen.resetCurrentFeatures();
	}
	
	//read and print the results
	for (int i = 0; i < examples.size(); ++i) {
		cout << examples[i] << " : " << predictions[i] << endl;
	}

}

//private:
list<string> relationclassificator::split(string s, char delim) {
	list<string> ssplited;
	
	string part = "";
	for (auto c : s) {
		if(c == delim) {
			ssplited.push_back(part);
			part = "";
		}
		else{
			part += c;
		}
	}
	if (part != "")	ssplited.push_back(part);

	return ssplited;
}