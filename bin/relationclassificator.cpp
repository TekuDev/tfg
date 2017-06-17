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
#include "freeling/morfo/semgraph.h"

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
void relationclassificator::predict(freeling::document &doc) const {
	featGenerator featGen(true, true);

	freeling::semgraph::semantic_graph &semg = doc.get_semantic_graph();

	//for (paragraf)
	list<paragraph::const_iterator> sents;
	for (document::const_iterator p=doc.begin(); p!=doc.end(); ++p)
		for (paragraph::const_iterator s=p->begin(); s!=p->end(); ++s)
           sents.push_back(s);
	
	//create events
	featGen.createEvents(sents);

	//make pairs
	list<std::pair<event,event>> pairs = featGen.getPairs();
	int id = 0;
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

		if(tag != L"NONE") {

			wstring rtid = L"RT"+util::string2wstring(std::to_string(id));
			wstring classrel = tag;
			wstring id1 = util::string2wstring(pi.first.id)+L" "+util::string2wstring(pi.first.word);
			wstring id2 = util::string2wstring(pi.second.id)+L" "+util::string2wstring(pi.second.word);
			wstring t1 = id1[0] == 'e' ? L"EVENT" : L"TIMEEXPRESION";
			wstring t2 = id2[0] == 'e' ? L"EVENT" : L"TIMEEXPRESION";

			addRelTemp2SemGraph(semg, rtid, pi.first, pi.second, t1, t2, classrel);

			++id;
		}

		delete pred;
		featGen.resetCurrentFeatures();
	}

	vector<freeling::semgraph::SG_relation_tmp> relations = semg.get_relations();
	for (auto r : relations) {
		wcout << L"RelTemp Id: " << r.get_id();
		wcout << L" W1: " << r.get_w1();
		wcout << L" Type1: " << r.get_t1();
		wcout << L" W2: " << r.get_w2();
		wcout << L" Type2: " << r.get_t2();
		wcout << L" Relation: " << r.get_rel() << endl; 
	}
}

//private:
list<string> relationclassificator::split(string s, char delim) const {
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

void relationclassificator::addRelTemp2SemGraph(freeling::semgraph::semantic_graph &semg, wstring &relid, const event &e1, const event &e2, wstring &t1, wstring &t2, wstring &rel) const {
	wstring id1,id2;

	if(t1 == L"EVENT") {
		id1 = getFrameId(semg, e1);
	}
	else {
		id1 = getEntityId(semg, e1);
	}

	if(t2 == L"EVENT") {
		id2 = getFrameId(semg, e2);
	}
	else {
		id2 = getEntityId(semg, e2);
	}

	freeling::semgraph::SG_relation_tmp rt(relid, rel, id1, id2, t1, t2);
	semg.add_relTemp(rt);
}

wstring relationclassificator::getFrameId(freeling::semgraph::semantic_graph &semg, const event &e) const {
	vector<freeling::semgraph::SG_frame> frames = semg.get_frames();

	wstring id;

	for (auto f : frames) {
		if(f.get_sentence_id() == util::string2wstring(e.sen+1) and f.get_token_id() == util::string2wstring(e.pos+1)) return f.get_id();
	}

	freeling::semgraph::SG_frame fnew(e.w->get_lemma(), e.w->get_senses().begin()->first, util::string2wstring(e.pos+1), util::string2wstring(e.sen));
	semg.add_frame(fnew);

	return fnew.get_id();
}

wstring relationclassificator::getEntityId(freeling::semgraph::semantic_graph &semg, const event &e) const {
	vector<freeling::semgraph::SG_entity> entities = semg.get_entities();

	wstring id;

	for (auto ent : entities) {
		vector<freeling::semgraph::SG_mention> mentions = ent.get_mentions();

		for (auto m : mentions) {
			if(m.get_sentence_id() == util::string2wstring(e.sen+1) and m.get_id() == util::string2wstring(e.pos+1)) return ent.get_id();
		}
	}

	freeling::semgraph::SG_entity enew(e.w->get_lemma(), L"TIMEEXPRESION", freeling::semgraph::entityType::WORD, e.w->get_senses().begin()->first);
	list<wstring> words; words.push_back(util::string2wstring(e.word));
	freeling::semgraph::SG_mention mnew(util::string2wstring(e.pos+1), util::string2wstring(e.sen+1), words);

	//enew.add_mention(mnew);
	semg.add_entity(enew);

	return enew.get_id();

}