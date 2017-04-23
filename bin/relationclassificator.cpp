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
#include "freeling/omlet/svm.h"

using namespace std;
using namespace freeling;


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
      ERROR_CRASH(L"Error opening file "+configFile);

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

	featGen = new featGenerator(true, true);
}

//Extract features and classify them.
void relationclassificator::predict(freeling::document &doc) {
	list<wstring> predictions;
	list<string> examples;

	//for (paragraf)
		list<freeling::sentence> ls;
		//for (ls)
			//create events
			//make pairs
			list<pair(event, event)> pairs;
			for (auto pi : pairs)
			{
				double *pred = new double[classif->get_nlabels()];
				//create example to classify
				example exmp(classif->get_nlabels());
				//add features
				set<int> features = featGen->generateFeaturesSet(pi.first, pi.second, ls);
				for (auto f : features) exmp.add_feature(f);

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

				predictions.push_back(tag);
				examples.push_back(pi.first.id+"-"+pi.second.id);

			}
	
	//read and print the results
	for (int i = 0; i < examples.size(); ++i) {
		cout << examples[i]+" have "+predictions[i] << endl;
	}

}