#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <map>
#include "../pugixml-1.7/src/pugixml.hpp"
#include "freeling.h"
#include "freeling/morfo/traces.h"
#include "event.h"
#include "featGenerator.h"
using namespace std;
using namespace freeling;


//extract Events from Freeling info.
void extractEvents(list<sentence> &ls, featGenerator *featGen) {
    int sen = 0;
    int i = 0;

    list<freeling::sentence>::const_iterator s=ls.begin();
    while(s != ls.end()) {
        sentence::const_iterator w = s->begin();
        while(w != s->end()) {
            string tag = util::wstring2string(w->get_tag());
            if(tag[0] == 'V' || tag == "W") {
                event *ev = new event(tag);
                event e = *ev;

                e.w = &(*w);
                e.pos = (*w).get_position();
                e.sen = sen;
                e.id = "e"+to_string(i);
                e.word = util::wstring2string(w->get_form());

                featGen->addEvent(e);
                ++i;
            }
            ++w;
        }
        ++sen;
        ++s;
    }
}



int main(int nArgs, char* args[]) {
    if(nArgs != 3) {cout << "Debes introducir el nombre del fichero [Features] y el tipo de features generados [ALL, OL(only lexical), OS (Only Syntactical)]" << endl; exit(0);}
    
    string featuresName, featTypes;
    featuresName = args[1]; featTypes = args[2];

    bool lexicalfeats, syntactfeats;
    if (featTypes == "ALL") {lexicalfeats = true; syntactfeats=true;}
    else if (featTypes == "OL") {lexicalfeats = true; syntactfeats=false;}
    else if (featTypes == "OS") {lexicalfeats = false; syntactfeats=true;}
    else {cout << "el tipo de features tiene que ser ALL or OL or OS" << endl; exit(0);}

    featGenerator *featGen = new featGenerator(lexicalfeats,syntactfeats);

    string pathFeat = "../inputs/features2/"+featuresName+".txt";
    if(not featGen->openFile(0,pathFeat)) {cout << "error al abrir el documento features" << endl; exit(0);}

    string text = "";
    string word;
    cin >> word;
    text += word;
    while(cin >> word)
        text = text+" "+word;
    cout << text << endl;
    wstring wtext(text.begin(), text.end());
    wcout << wtext << endl;

    cout << "Generando features" << endl;

    //Pasamos el texto por el Freeling
    util::init_locale(L"default");
    wstring ipath=L"/usr/local";
    wstring path=ipath+L"/share/freeling/en/";

    tokenizer tk(path+L"tokenizer.dat"); 
    splitter sp(path+L"splitter.dat");
    splitter::session_id sid=sp.open_session();
    maco_options opt(L"en");

    opt.UserMapFile=L"";
    opt.LocutionsFile=path+L"locucions.dat"; opt.AffixFile=path+L"afixos.dat";
    opt.ProbabilityFile=path+L"probabilitats.dat"; opt.DictionaryFile=path+L"dicc.src";
    opt.NPdataFile=path+L"np.dat"; opt.PunctuationFile=path+L"../common/punct.dat"; 

    maco morfo(opt);
    morfo.set_active_options (false,// UserMap
                             true, // NumbersDetection,
                             true, //  PunctuationDetection,
                             true, //  DatesDetection,
                             true, //  DictionarySearch,
                             true, //  AffixAnalysis,
                             false, //  CompoundAnalysis,
                             true, //  RetokContractions,
                             true, //  MultiwsDetection,
                             true, //  NERecognition,
                             false, //  QuantitiesDetection,
                             true);  //  ProbabilityAssignment
    hmm_tagger tagger(path+L"tagger.dat", true, FORCE_TAGGER);
    senses sen(path+L"senses.dat");
    ukb wsd(path+L"ukb.dat");
    dep_treeler parser(path+L"dep_treeler/dependences.dat");

    list<freeling::word> lw; 
    list<sentence> ls;

    lw=tk.tokenize(wtext);
    ls=sp.split(sid, lw, true);
    morfo.analyze(ls);
    tagger.analyze(ls);
    sen.analyze(ls);
    wsd.analyze(ls);
    parser.analyze(ls);

//https://github.com/TALP-UPC/FreeLing/blob/master/src/main/simple_examples/coreferences.cc

    //Create list of iterators
    list<paragraph::const_iterator> sents;
    //for (paragraph::const_iterator s=ls.begin(); s!=ls.end(); ++s)
    paragraph::const_iterator s=(&ls)->begin();
    sents.push_back(s);

    //Add events to events::list<event>
    featGen->createEvents(sents);
    //featGen->printEvents();

    //Create features:
    featGen->generateFeatures(ls);

    sp.close_session(sid);

    cout << "Features generados" << endl;
    cout << "Number of events = " << featGen->getEvents().size() << endl;
}