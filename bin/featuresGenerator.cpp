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
    int start_offs;
    int end_offs;
    string id;
    string word;
    const freeling::word *w;
    int pos;
    int sen;

    event(const string &t) : type(util::string2wstring(t)) {}
    ~event() {};
};

//global
list<event> events;
map<string, int> dic;
set<string> clases;
string numericFeatures[] = {"nWord","nSen","nVerb"};
list< pair<string, string> > conexions;
ofstream out[2];
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
    f = trataNumeric(f);
    if(dic.find(f) != dic.end()) dic[f] += 1;
    else dic[f] = 1;
}

void printClass(string f) {
    out[0] << f << " ";
    clases.insert(f);
}

void printFeatsSet(set<string> feats) {
    for(auto f : feats) {
        printFeat(f);
    }
}

void printConexions() {
    out[0] << "Conexions: " << endl;
    for (auto c : conexions) {
        out[0] << c.first << " - " << c.second << endl;
    }
}

void printEvents(list<sentence> &ls) {	
    cout << "Print events" << endl;
	for(auto e: events) {
        wstring type = e.type;
        int start_offs = e.start_offs;
        int end_offs = e.end_offs;
        string id = e.id;
        string word = e.word;
        cout << word << " starts in: " << start_offs << " ends: " << end_offs << " id: " << id << endl;

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

void printDic() {
    int i = 15;
    /*for (auto c : clases) {
        out[2] << i << " " << c << endl;
        ++i;
    }*/

    vector< pair<string,int> > vecAux(dic.begin(), dic.end());
    sort(vecAux.begin(), vecAux.end(), &sortFunc);
    for(auto p : vecAux) {out[1] << i << " " << p.first << " " << p.second << endl; ++i;}
}


//Clear XML events and creat a structre with the XML information.
void extractXML(pugi::xml_node &xml, wstring &text, int &i) {

	pugi::xml_node node = xml.first_child();
    while (node.type() != pugi::node_null) {
        switch (node.type()) {
            case pugi::node_element: {
                string name = node.name();
                event *ev = new event(name);
                event e = *ev;
                e.start_offs = i;
                e.pos = -1; e.sen = -1;
                extractXML(node,text,i);
                e.end_offs = i;
                //if the next node is another event concat space for correct clean text.
                if(node.next_sibling().type()==pugi::node_element) {text += L" "; ++i;}
                string id = node.attribute("eid").value();
                e.id = (id.empty()) ? node.attribute("tid").value() : id;
                e.word = node.child_value();
                if(name == "TIMEX3" or name == "EVENT") events.push_back(e);
                break;
            }
            case pugi::node_pcdata: {
                wstring t = util::string2wstring(node.value());
                text += t;
                i += t.length();
                break;
            }
            default: {
                wcout << L"Unexpected node " << util::string2wstring(node.name());
                exit(1);
                break;
            }
        }
        node = node.next_sibling();
    }
}

void addFreelingInfoToEvents(list<sentence> &ls) {
    for(auto &ei : events) {
        int sen = 0;
        bool found = false;
        int spanS, spanF;
        
        list<freeling::sentence>::const_iterator s=ls.begin();
        while(not found and s != ls.end()) {
            sentence::const_iterator w = s->begin();
            while(not found and w != s->end()) {
                spanS = w->get_span_start();
                spanF = w->get_span_finish();
                if((ei.start_offs == spanS) or (ei.start_offs >= spanS and ei.end_offs <= spanF)) {
                    found = true;
                    ei.w = &(*w);
                    ei.pos = (*w).get_position();
                    ei.sen = sen;
                }
                ++w;
            }
            ++s;
            ++sen;
        }
    }
    //printEvents(ls);
}

event findEventInEvents(string e) {
    for(auto &ev: events) {
        if (ev.id == e) {return ev;}
    }
}

bool findConexion(string ei, string ej) {
    for(auto p : conexions){
        if ((p.first == ei and p.second == ej) or (p.second == ei and p.first == ej)) return true;
    }
    return false;
}

void dependences(string e1, string e2, list<sentence> &ls) {
    set<string> repetableFeats;
    list<freeling::sentence>::const_iterator s;
    freeling::dep_tree::const_iterator p;
    int spanS, spanF;
    bool found = false;
    
    event ei = findEventInEvents(e1);
    event ej = findEventInEvents(e2);

    if(ei.sen != -1 and ei.pos != -1 and ej.sen != -1 and ej.pos != -1) {

        word fword = *ei.w;
        word sword = *ej.w;

        //select the sentence of the word
        s=ls.begin();
        for (int i = 0; i < ei.sen; ++i) ++s;

        //select the word in the dep_tree
        p=s->get_dep_tree().begin();
        while  (p!=s->get_dep_tree().end()) {
            word w = p->get_word();
            spanS = w.get_span_start();
            spanF = w.get_span_finish();
            if((ei.start_offs == spanS) or (ei.start_offs >= spanS and ei.end_offs <= spanF)) {found = true; break;}
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
            if((ej.start_offs == spanS) or (ej.start_offs >= spanS and ej.end_offs <= spanF)) {found = true; break;}
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
}

void distance(string e1, string e2, list<sentence> &ls) {
    set<string> repetableFeats;
    int nWord = 0;
    int nSen = 0;
    int nVerb = 0;
    int ssen,spos,fsen,fpos;
    event ei = findEventInEvents(e1);
    event ej = findEventInEvents(e2);

    const freeling::word *fword, *sword;

    if(ei.start_offs < ej.start_offs) {
        fword = ei.w; fsen = ei.sen; fpos = ei.pos;
        sword = ej.w; ssen = ej.sen; spos = ej.pos;
    }
    else {
        fword = ej.w; fsen = ej.sen; fpos = ej.pos;
        sword = ei.w; ssen = ei.sen; spos = ei.pos;
    }

    if (fsen != -1 and fpos != -1 and ssen != -1 and spos != -1) {

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
                string form = "wordInMiddle="+util::wstring2string(w->get_form());
                string lemma = "WIMLemma="+util::wstring2string(w->get_lemma());
                repetableFeats.insert(form);
                repetableFeats.insert(lemma);

                ++nWord;
                
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
                ++w;
                ++wordAct;
            }
            ++senAct;
            ++sen;
            ++nSen;
            w = sen->begin();
        }
        //sen->get_words().size();


        //print Lemmas
        string f1lemma = "w1Lemma="+util::wstring2string(fword->get_lemma());
        string f2lemma = "w2lemma="+util::wstring2string(sword->get_lemma());
        printFeat(f1lemma); printFeat(f2lemma);

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
}

void generateFeatures(pugi::xml_document &doc, wstring txt, tokenizer &tk, splitter &sp, maco &morfo, hmm_tagger &tagger, splitter::session_id &sid, senses &sen, ukb &wsd, dep_treeler &parser) {

	pugi::xml_node links = doc.child("TimeML");
	pugi::xml_node text = links.child("TEXT");
    pugi::xml_node date = links.child("DCT");

	//Pasamos el texto por el Freeling
    list<word> lw;
    list<sentence> ls;

    lw=tk.tokenize(txt);
    ls=sp.split(sid, lw, true);
    morfo.analyze(ls);
    tagger.analyze(ls);
    sen.analyze(ls);
    wsd.analyze(ls);
    parser.analyze(ls);

    //TODO: Fixed
    addFreelingInfoToEvents(ls);

	for(pugi::xml_node node: links.children("TLINK"))
    {
    	//Inmediate features from xml
    	string clas = node.attribute("relType").value();
        printClass(clas);

    	string event = node.attribute("eventInstanceID").value();
    	string rt = node.attribute("relatedToTime").value();
    	bool related2time = (rt.empty()) ? false : true;
    	
    	string related, posw1, posw2, tensew1, tensew2, ei, ej;
        int dist;
        if (event.empty()) {
            event = node.attribute("timeID").value();
            ei = event; 
            pugi::xml_node t1;
            if(event =="t0") t1 = date.find_child_by_attribute("TIMEX3", "tid", event.c_str());
            else t1 = text.find_child_by_attribute("TIMEX3", "tid", event.c_str());
            posw1 = t1.attribute("type").value();
            event = t1.child_value();
            string f,f2; f = "w1="+event; f2 = "posw1="+posw1;
            printFeat(f); printFeat(f2);
        }

        else {
            pugi::xml_node e1 = links.find_child_by_attribute("MAKEINSTANCE", "eiid", event.c_str());
            posw1 = e1.attribute("pos").value();
            tensew1 = e1.attribute("tense").value();
            event = e1.attribute("eventID").value();
            ei = event;
            pugi::xml_node t2 = text.find_child_by_attribute("EVENT", "eid", event.c_str());
            event = t2.child_value();
            string f,f2,f3;
            f = "w1="+event; f2 = "posw1="+posw1; f3 = "tensew1="+tensew1;
            printFeat(f); printFeat(f2); printFeat(f3);
        }

    	if (related2time) {
            ej = rt;
            pugi::xml_node t;
            if(rt =="t0") t = date.find_child_by_attribute("TIMEX3", "tid", rt.c_str());
    		else t = text.find_child_by_attribute("TIMEX3", "tid", rt.c_str());
    		posw2 = t.attribute("type").value();
    		related = t.child_value();
            string f,f2; f = "w2="+related; f2 = "posw2="+posw2;
            printFeat(f); printFeat(f2);
    	}
    	else {
    		related = node.attribute("relatedToEventInstance").value();
    		pugi::xml_node e2 = links.find_child_by_attribute("MAKEINSTANCE", "eiid", related.c_str());
    		posw2 = e2.attribute("pos").value();
    		tensew2 = e2.attribute("tense").value();
    		related = e2.attribute("eventID").value();
            ej = related;
    		pugi::xml_node t3 = text.find_child_by_attribute("EVENT", "eid", related.c_str());
    		related = t3.child_value();
            string f,f2,f3;
            f = "w2="+related; f2 = "posw2="+posw2; f3 = "tensew2="+tensew2;
            printFeat(f); printFeat(f2); printFeat(f3);
		}

        pair<string, string> p = make_pair(ei,ej);
        conexions.push_back(p);

    	//Features from Freeling:
        if(lexicalfeats) {
            distance(ei,ej,ls);
        }
        if(syntactfeats) {
            dependences(ei, ej, ls);
        }
        out[0] << endl;
    }

    //Create the 'none' features:
    for (auto ei : events) {
        for (auto ej : events) {
            string event = ei.id;
            string related = ej.id;
            
            if (event != related and not findConexion(event,related)) {
                string posw1, posw2, tensew1, tensew2, f1;

                f1 = "NONE";
                printClass(f1);

                if(ei.type == L"TIMEX3") {
                    pugi::xml_node t1;
                    if(event =="t0") t1 = date.find_child_by_attribute("TIMEX3", "tid", event.c_str());
                    else t1 = text.find_child_by_attribute("TIMEX3", "tid", event.c_str());
                    posw1 = t1.attribute("type").value();
                    event = t1.child_value();
                    string f,f2;
                    f = "w1="+event; f2 = "posw1="+posw1;
                    printFeat(f); printFeat(f2);
                }
                else {
                    pugi::xml_node e1 = links.find_child_by_attribute("MAKEINSTANCE", "eventID", event.c_str());
                    posw1 = e1.attribute("pos").value();
                    tensew1 = e1.attribute("tense").value();
                    pugi::xml_node t2 = text.find_child_by_attribute("EVENT", "eid", event.c_str());
                    event = t2.child_value();
                    string f,f2,f3;
                    f = "w1="+event; f2 = "posw1="+posw1; f3 = "tensew1="+tensew1;
                    printFeat(f); printFeat(f2); printFeat(f3);
                }

                if(ej.type == L"TIMEX3") {
                    pugi::xml_node t;
                    if(related =="t0") t = date.find_child_by_attribute("TIMEX3", "tid", related.c_str());
                    else t = text.find_child_by_attribute("TIMEX3", "tid", related.c_str());
                    posw2 = t.attribute("type").value();
                    related = t.child_value();
                    string f,f2;
                    f = "w2="+related; f2 = "posw2="+posw2;
                    printFeat(f); printFeat(f2);
                }
                else {
                    pugi::xml_node e2 = links.find_child_by_attribute("MAKEINSTANCE", "eventID", related.c_str());
                    posw2 = e2.attribute("pos").value();
                    tensew2 = e2.attribute("tense").value();
                    pugi::xml_node t3 = text.find_child_by_attribute("EVENT", "eid", related.c_str());
                    related = t3.child_value();
                    string f,f2,f3;
                    f = "w2="+related; f2 = "posw2="+posw2; f3 = "tensew2="+tensew2;
                    printFeat(f); printFeat(f2); printFeat(f3);
                }

                //Features from Freeling:
                if(lexicalfeats) {
                    distance(ei.id,ej.id,ls);
                }
                if(syntactfeats) {
                    dependences(ei.id, ej.id, ls);
                }

                out[0] << endl;
            }
        }
    }
    out[0] << endl;

}

int main(int nArgs, char* args[]) {
    if(nArgs != 4) {cout << "Debes introducir el nombre de los 2 ficheros de output [Features], [Dicctionary] y los features generados [ALL, OL(only lexical), OS (Only Syntactical)]" << endl; exit(0);}
    
    string featuresName, dicctionaryName, featTypes;
    featuresName = args[1]; dicctionaryName = args[2]; featTypes = args[3];

    if (featTypes == "ALL") {lexicalfeats = true; syntactfeats=true;}
    else if (featTypes == "OL") {lexicalfeats = true; syntactfeats=false;}
    else if (featTypes == "OS") {lexicalfeats = false; syntactfeats=true;}
    else {cout << "el tipo de features tiene que ser ALL or OL or OS" << endl; exit(0);}

    out[0].open("../inputs/features/"+featuresName+".txt");
    out[1].open("../inputs/dicctionaries/"+dicctionaryName+".txt");

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

	string pathFile;
    while(cin >> pathFile)
    {
    	pugi::xml_document doc;
    	pugi::xml_parse_result result = doc.load_file(pathFile.c_str());

    	if(result) {
            pugi::xml_node node = doc.child("TimeML");

            int i = 0;
    		wstring text = L"";
            cout << "Extrayendo XML: " << pathFile << endl;
            extractXML(node,text,i);

            cout << "Generando features" << endl;
    		generateFeatures(doc, text, tk, sp, morfo, tagger, sid, sen, wsd, parser);
            cout << "Features generados" << endl;
    	}
    	else out[0] << "Error parsing the input file" << endl;

        events.clear();
        conexions.clear();
        cout << "Dic size: " << dic.size() << endl;
    }
    sp.close_session(sid);

    printDic();
}