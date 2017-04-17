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

const classifier* classif;


int main(int argc, char const *argv[])
{
	wstring necFile = argv[1];
    wstring lexFile,rgfFile,modelFile;
    wstring clastype, classnames;
    wstring path=necFile.substr(0,necFile.find_last_of(L"/\\")+1);
     
    // read configuration file and store information   
    enum sections {LEXICON,RGF,CLASSIFIER,MODELFILE,CLASSES,NE_TAG};
    config_file cfg;
    cfg.add_section(L"Lexicon",LEXICON);
    cfg.add_section(L"RGF",RGF);
    cfg.add_section(L"Classifier",CLASSIFIER);
    cfg.add_section(L"ModelFile",MODELFILE);
    cfg.add_section(L"Classes",CLASSES);
    cfg.add_section(L"NE_Tag",NE_TAG);

    if (not cfg.open(necFile))
      ERROR_CRASH(L"Error opening file "+necFile);

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

		    case RGF: {
		        // Reading RGF file name
		        sin>>rgfFile;
		        rgfFile= util::absolute(rgfFile,path); 
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

		    case NE_TAG: {
		        // tag that identifies detected NEs to be classified
		        NPtag=line;
		        break;
		    }

		    default: break;
		}
	}
	cfg.close();

	if (clastype==L"SVM") {
      classif = new svm(modelFile,classnames);
	}

	//Extract features and classify them.
https://github.com/TALP-UPC/FreeLing/blob/master/src/libfreeling/nec.cc

	/*Use featGen to generate features from the text and then predict from each pair*/
}