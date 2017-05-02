#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <map>
#include "../pugixml-1.7/src/pugixml.hpp"
#include "freeling.h"
#include "freeling/morfo/traces.h"
using namespace std;
using namespace freeling;

class event {
  public:
    wstring type;
    string id;
    string word;
    const freeling::word *w;
    int pos;
    int sen;

    event(const string &t) : type(util::string2wstring(t)) {}
    event() {};
    ~event() {};
};

//global
list<event> events;
map<string, int> dic;
//set<string> clases;
string numericFeatures[] = {"nWord","nSen","nVerb"};
ofstream out[1];
bool lexicalfeats, syntactfeats;


bool sortFunc(pair<string,int> first, pair<string,int> second) { return (first.second > second.second); }

string trataNumeric(string f) {
    string tag = f.substr(0,f.find('='));
    for (string n : numericFeatures) {
        if(tag == n) return tag;
    }
    return f;
}

void printFeat(string f) {
    std::replace(f.begin(),f.end(),' ','_');
    out[0] << f << " ";
}

void printFeatsSet(set<string> feats) {
    for(auto f : feats) {
        printFeat(f);
    }
}

void printEvents() {	
    cout << "Print events" << endl;
	for(auto e: events) {
        wstring type = e.type;
        string id = e.id;
        string word = e.word;
        cout << word << " id: " << id << endl;

        const freeling::word w = *e.w;
        int pos = e.pos;
        int sen = e.sen;
        cout << "pos: " << pos << " en la frase " << sen << endl;
        wcout << "form: " << w.get_form() << endl;
    }
}

void printDateInfo(string dateInfo, string word) {
    dateInfo = dateInfo.substr(1,(dateInfo.size()-2));
    vector<string> infos; string subdate = "";
    for (char c : dateInfo) {
        if(c == ':') {infos.push_back(subdate); subdate = "";}
        else subdate += c;
    }
    infos.push_back(subdate);

    if(infos[0] != "??") printFeat(word+"dayOfWeek="+infos[0]);
    if(infos[3] != "??") printFeat(word+"time="+infos[2]+infos[3]);

    vector<string> d; subdate = "";
    for (char c : infos[1]) {
        if(c == '/') {d.push_back(subdate); subdate = "";}
        else subdate += c;
    }
    d.push_back(subdate);

    if(d[0] != "??") printFeat(word+"dayOfMonth="+d[0]);
    if(d[1] != "??") printFeat(word+"Month="+d[1]);
    if(d[2] != "??") printFeat(word+"Year="+d[2]);
}


//extract Events from Freeling info.
void extractEvents(list<sentence> &ls) {
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

                events.push_back(e);

                ++i;
            }
            ++w;
        }
        ++sen;
        ++s;
    }
}

event findEventInEvents(string e) {
    for(auto &ev: events) {
        if (ev.id == e) {return ev;}
    }
}

void generateSyntacticalFeats(event &ei, event &ej, list<sentence> &ls) {
    set<string> repetableFeats;
    list<freeling::sentence>::const_iterator s;
    freeling::dep_tree::const_iterator p;
    int spanS, spanF;
    bool found = false;

    word fword = *ei.w;
    word sword = *ej.w;

    //select the sentence of the word
    s=ls.begin();
    for (int i = 0; i < ei.sen; ++i) ++s;

    //select the word in the dep_tree
    p=s->get_dep_tree().begin();
    while  (p!=s->get_dep_tree().end()) {
        word w = p->get_word();
        if(ei.pos == w.get_position()) {found = true; break;}
        ++p;
    }
    /*found = true;
    dep_tree::const_iterator pi = p->get_node_by_pos(ei.pos);
    */

    //If found the word print the features
    if(found) {
        if (not fword.get_senses().empty()) {
            string sens = util::wstring2string(fword.get_senses().begin()->first);
            string feat = "w1Sense_"+sens; printFeat(feat);
        }

        string tag = util::wstring2string(fword.get_tag());
        if(tag[0]=='V') {
            for (freeling::dep_tree::const_sibling_iterator ch = p.sibling_begin(); ch!=p.sibling_end(); ch++) {
                string label = util::wstring2string(ch->get_label());
                if (label=="SBJ" or label=="OBJ" or (label[0]=='A' and label[1]=='M')) {
                    string lemma = util::wstring2string(ch->get_word().get_lemma());
                    string feat = "w1"+label+"_"+lemma; //printFeat(feat);
                    repetableFeats.insert(feat);
                }

                if (&ch->get_word() == ej.w) {
                    string form = util::wstring2string(ch->get_word().get_form());
                    string feat = "role12_"+label+"_"+form; printFeat(feat);
                }
            } 
        }
    }

    found = false;

    //select the sentence of the word
    s=ls.begin();
    for (int i = 0; i < ej.sen; ++i) ++s;

    //select the word in the dep_tree
    p=s->get_dep_tree().begin();
    while  (p!=s->get_dep_tree().end()) {
        word w = p->get_word();
        spanS = w.get_span_start();
        spanF = w.get_span_finish();
        if(ej.pos == w.get_position()) {found = true; break;}
        ++p;
    }
    /*found = true;
    dep_tree::const_iterator pi = p->get_node_by_pos(ej.pos);
    */

    //If found the word print the features
    if(found) {
        if (not sword.get_senses().empty()) {
            string sens = util::wstring2string(sword.get_senses().begin()->first);
            string feat = "w2Sense_"+sens; printFeat(feat);
        }

        string tag = util::wstring2string(sword.get_tag());
        if(tag[0]=='V') {
            for (freeling::dep_tree::const_sibling_iterator ch = p.sibling_begin(); ch!=p.sibling_end(); ch++) {
                string label = util::wstring2string(ch->get_label());
                if (label=="SBJ" or label=="OBJ" or (label[0]=='A' and label[1]=='M')) {
                    string lemma = util::wstring2string(ch->get_word().get_lemma());
                    string feat = "w2"+label+"_"+lemma; //printFeat(feat);
                    repetableFeats.insert(feat);
                }

                if (&ch->get_word() == ei.w) {
                    string form = util::wstring2string(ch->get_word().get_form());
                    string feat = "role21_"+label+"_"+form; printFeat(feat);
                }
            }
        }
    }
    printFeatsSet(repetableFeats);
}

void generateLexicalFeats(event &ei, event &ej, list<sentence> &ls) {
    set<string> repetableFeats;
    int nWord = 0;
    int nSen = 0;
    int nVerb = 0;
    int ssen,spos,fsen,fpos;

    //out[0] << "word1:"<<ei.id<<endl;;
    //out[0] << "word2:"<<ej.id<<endl;


    const freeling::word *fword, *sword;

    if(ei.pos <= ej.pos) {
        fword = ei.w; fsen = ei.sen; fpos = ei.pos;
        sword = ej.w; ssen = ej.sen; spos = ej.pos;
    }
    else {
        fword = ej.w; fsen = ej.sen; fpos = ej.pos;
        sword = ei.w; ssen = ei.sen; spos = ei.pos;
    }

    //out[0] << "pos1:"<<fpos<<endl;
    //out[0] << "pos2:"<<spos<<endl;
    //out[0] << "sen1:"<<fsen<<endl;
    //out[0] << "sen2:"<<ssen<<endl;

    //first word
    std::list<sentence>::iterator sen = ls.begin();
    for(int i = 0; i < fsen; ++i) ++sen;
    sentence::const_iterator w = sen->begin();
    for(int i = 0; i < fpos; ++i) ++w;
    if(w != sen->begin()) {
        --w;
        string f1before = "w1Before="+util::wstring2string(w->get_form());
        printFeat(f1before);
        ++w;
    }
    ++w;
    if(w != sen->end()) {
        string f1after = "w1After="+util::wstring2string(w->get_form());
        printFeat(f1after);
    }
    --w;
    bool fin = false;
    int senAct = fsen; int wordAct = fpos;

    //second word
    while(not fin and sen != ls.end()) {
        while(not fin and w != sen->end()) {
        	//out[0] << "posACT:"<<wordAct<<endl;
    		//out[0] << "senACT:"<<senAct<<endl;
        	
        	//out[0] << util::wstring2string(w->get_form())<<endl;
            string form = "wordInMiddle="+util::wstring2string(w->get_form());
            string lemma = "WIMLemma="+util::wstring2string(w->get_lemma());
            repetableFeats.insert(form);
            repetableFeats.insert(lemma);
            
            string tag = util::wstring2string(w->get_tag());
            if(tag[0]=='V') ++nVerb;

            if(senAct == ssen and wordAct == spos) {
                fin = true;
                if (w != sen->begin()) {
                    --w; 
                    string f2before = "w2Before="+util::wstring2string(w->get_form());
                    printFeat(f2before);
                    ++w;
                }
                
                ++w;
                if (w != sen->end()) {
                    string f2after = "w2After="+util::wstring2string(w->get_form());
                    printFeat(f2after);
                }
                --w;
            }
            //out[0] << "nWord:"<<nWord<<endl;
    		//out[0] << "word:"<<util::wstring2string(w->get_form())<<endl;

            ++nWord;
            ++w;
            ++wordAct;
        }
        ++senAct;
        ++sen;
        ++nSen;
        w = sen->begin();
        wordAct=0;
    }
    //sen->get_words().size();


    //print Forms and Lemmas
    string f1lemma = "w1Lemma="+util::wstring2string(fword->get_lemma());
    string f2lemma = "w2lemma="+util::wstring2string(sword->get_lemma());
    string f1form = "w1form="+util::wstring2string(fword->get_form());
    string f2form = "w2form="+util::wstring2string(sword->get_form());
    printFeat(f1form); printFeat(f2form); printFeat(f1lemma); printFeat(f2lemma);

    printFeatsSet(repetableFeats);

    //If some word is a DATE, print extra information
    if (util::wstring2string(fword->get_tag()) == "W") {
        string dateInfo = util::wstring2string(fword->get_lemma());
        printDateInfo(dateInfo, "w1");
    }
    if (util::wstring2string(sword->get_tag()) == "W") {
        string dateInfo = util::wstring2string(sword->get_lemma());
        printDateInfo(dateInfo, "w2");
    }

    //Print the distances
    string f8 = "nWord="+to_string(nWord);
    string f9 = "nVerb="+to_string(nVerb);
    string f10 = "nSen="+to_string(nSen);
    printFeat(f8); printFeat(f9); printFeat(f10);
}

void generateFeatures(wstring txt) {
    //Pasamos el texto por el Freeling
    list<word> lw;
    list<sentence> ls;

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
                             true, //  MultiwordsDetection,
                             true, //  NERecognition,
                             false, //  QuantitiesDetection,
                             true);  //  ProbabilityAssignment
    hmm_tagger tagger(path+L"tagger.dat", true, FORCE_TAGGER);
    senses sen(path+L"senses.dat");
    ukb wsd(path+L"ukb.dat");
    dep_treeler parser(path+L"dep_treeler/dependences.dat");

    lw=tk.tokenize(txt);
    ls=sp.split(sid, lw, true);
    morfo.analyze(ls);
    tagger.analyze(ls);
    sen.analyze(ls);
    wsd.analyze(ls);
    parser.analyze(ls);

    //Add events to events::list<event>
    extractEvents(ls);

    printEvents();

    //Create features:
    int i = 0;
    for (auto ei : events) {
    	int j = 0;
        for (auto ej : events) {
            string event = ei.id;
            string related = ej.id;
                        
        	if (event != related and ei.id[0] != 't' and i < j and ((ei.sen - ej.sen) == 1 or (ei.sen - ej.sen) == 0)){

                //Features from Freeling:
                if(lexicalfeats) {
                    generateLexicalFeats(ei,ej,ls);
                }
                if(syntactfeats) {
                    generateSyntacticalFeats(ei, ej, ls);
                }

                out[0] << endl;
        	}
        ++j;
        }
    ++i;
    }
    out[0] << endl;

    sp.close_session(sid);
}

int main(int nArgs, char* args[]) {
    if(nArgs != 3) {cout << "Debes introducir el nombre del fichero [Features], el tipo de features generados [ALL, OL(only lexical), OS (Only Syntactical)]" << endl; exit(0);}
    
    string featuresName, featTypes;
    featuresName = args[1]; featTypes = args[2];

    if (featTypes == "ALL") {lexicalfeats = true; syntactfeats=true;}
    else if (featTypes == "OL") {lexicalfeats = true; syntactfeats=false;}
    else if (featTypes == "OS") {lexicalfeats = false; syntactfeats=true;}
    else {cout << "el tipo de features tiene que ser ALL or OL or OS" << endl; exit(0);}

    out[0].open("tfg/inputs/features2/"+featuresName+".txt");
    if(not out[0].is_open()) {cout << "error al abrir el documento features" << endl; exit(0);}

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
	generateFeatures(wtext);
    cout << "Features generados" << endl;
    cout << "Number of events = " << events.size() << endl;
    events.clear();
}