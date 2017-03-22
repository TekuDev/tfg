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
    event() {};
    ~event() {};
};

//global
list<event> events;
map<string, int> dic;
set<string> clases;
string numericFeatures[] = {"nWord","nSen","nVerb"};
list< pair<string, string> > conexions;
ofstream out[3];
bool lexicalfeats, syntactfeats, createDiccs;


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
    
    if(createDiccs) {
	    f = trataNumeric(f);
	    if(dic.find(f) != dic.end()) dic[f] += 1;
	    else dic[f] = 1;
	}
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
    int i = 0;
    for (auto c : clases) {
        out[2] << i << " " << c << endl;
        ++i;
    }

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
	int notfound = 0;
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
        if (found == false) {
        	++notfound;
        	
        }
    }
    cout << "not found: " << notfound << endl;
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

void generateLexicalFeats(event &ei, event &ej, list<sentence> &ls) {
    set<string> repetableFeats;
    int nWord = 0;
    int nSen = 0;
    int nVerb = 0;
    int ssen,spos,fsen,fpos;

    //out[0] << "word1:"<<ei.id<<endl;;
    //out[0] << "word2:"<<ej.id<<endl;


    const freeling::word *fword, *sword;

    if(ei.start_offs < ej.start_offs) {
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
    //--w;
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

void generateFeatures(pugi::xml_document &doc, wstring txt, tokenizer &tk, splitter &sp, maco &morfo, hmm_tagger &tagger, splitter::session_id &sid, senses &sen, ukb &wsd, dep_treeler &parser) {
int descard = 0;
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

    addFreelingInfoToEvents(ls);

    event et, ed;

	for(pugi::xml_node node: links.children("TLINK"))
    {
    	//Inmediate features from xml
    	string clas = node.attribute("relType").value();
    	if(clas == "BEGINS") clas = "IS_INCLUDED";
    	else if(clas == "BEGUN_BY") clas = "INCLUDES";
    	else if(clas == "IAFTER") clas = "AFTER";
    	else if(clas == "IBEFORE") clas = "BEFORE";

    	string event = node.attribute("eventInstanceID").value();
    	string rt = node.attribute("relatedToTime").value();
    	bool related2time = (rt.empty()) ? false : true;
    	
    	string related, ei, ej;
        if (event.empty()) {
            event = node.attribute("timeID").value();
            ei = event; 
            pugi::xml_node t1;
            if(event =="t0") t1 = date.find_child_by_attribute("TIMEX3", "tid", event.c_str());
            else t1 = text.find_child_by_attribute("TIMEX3", "tid", event.c_str());
            event = t1.child_value();
            /*string f = "w1="+event; printFeat(f);*/
        }

        else {
            pugi::xml_node e1 = links.find_child_by_attribute("MAKEINSTANCE", "eiid", event.c_str());
            event = e1.attribute("eventID").value();
            ei = event;
            pugi::xml_node t2 = text.find_child_by_attribute("EVENT", "eid", event.c_str());
            event = t2.child_value();
            /*string f = "w1="+event; printFeat(f);*/
        }

    	if (related2time) {
            ej = rt;
            pugi::xml_node t;
            if(rt =="t0") t = date.find_child_by_attribute("TIMEX3", "tid", rt.c_str());
    		else t = text.find_child_by_attribute("TIMEX3", "tid", rt.c_str());
    		related = t.child_value();
            /*string f = "w2="+related; printFeat(f);*/
    	}
    	else {
    		related = node.attribute("relatedToEventInstance").value();
    		pugi::xml_node e2 = links.find_child_by_attribute("MAKEINSTANCE", "eiid", related.c_str());
    		related = e2.attribute("eventID").value();
            ej = related;
    		pugi::xml_node t3 = text.find_child_by_attribute("EVENT", "eid", related.c_str());
    		related = t3.child_value();
            /*string f = "w2="+related; printFeat(f);*/
		}

        pair<string, string> p = make_pair(ei,ej);
        conexions.push_back(p);

        et = findEventInEvents(ei);
    	ed = findEventInEvents(ej);
    	
    	if(et.sen != -1 and et.pos != -1 and ed.sen != -1 and ed.pos != -1) {
    		printClass(clas);
	    	//Features from Freeling:
	        if(lexicalfeats) {
	            generateLexicalFeats(et,ed,ls);
	        }
	        if(syntactfeats) {
	            generateSyntacticalFeats(et, ed, ls);
	        }
    	}
        else ++descard;
        out[0] << endl;
    }
    //printConexions();

    //Create the 'none' features:
    for (auto ei : events) {
        for (auto ej : events) {
            string event = ei.id;
            string related = ej.id;
            
            if (ei.sen != -1 and ei.pos != -1 and ej.sen != -1 and ej.pos != -1) {
            	if (event != related and not findConexion(event,related)
            		and ei.id[0] != 't' 
            		and (((ei.sen - ej.sen) == 1 or (ei.sen - ej.sen) == 0)
            		    or ej.id == "t0")){

	                printClass("NONE");

	                //Features from Freeling:
	                if(lexicalfeats) {
	                    generateLexicalFeats(ei,ej,ls);
	                }
	                if(syntactfeats) {
	                    generateSyntacticalFeats(ei, ej, ls);
	                }

	                out[0] << endl;
            	}
            }
            else ++descard;
        }
    }
    out[0] << endl;
    cout << "discard: " << descard << endl;
}

int main(int nArgs, char* args[]) {
    if(nArgs != 4 and nArgs != 5) {cout << "Debes introducir el nombre del fichero [Features], el tipo de features generados [ALL, OL(only lexical), OS (Only Syntactical)] y el nombre del diccionario y diccionario de clases o default para no generarlo" << endl; exit(0);}
    string arg3=args[3];
    if(nArgs == 4 and arg3 != "default") {cout << "Debes introducir el valor 'default' para no generar diccionarios" << endl; exit(0);}
    
    string featuresName, featTypes;
    featuresName = args[1]; featTypes = args[2];

    if (featTypes == "ALL") {lexicalfeats = true; syntactfeats=true;}
    else if (featTypes == "OL") {lexicalfeats = true; syntactfeats=false;}
    else if (featTypes == "OS") {lexicalfeats = false; syntactfeats=true;}
    else {cout << "el tipo de features tiene que ser ALL or OL or OS" << endl; exit(0);}

    out[0].open("../inputs/features/"+featuresName+".txt");
    createDiccs = false;

    if(arg3 != "default") {
    	createDiccs = true;
    	string diccName, diccOfClassName; diccName=args[3]; diccOfClassName=args[4];
		out[1].open("../inputs/dicctionaries/"+diccName+".txt");
		out[2].open("../inputs/diccOfClasses/"+diccOfClassName+".txt");    	
    }

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
            cout << "Number of events = " << events.size() << endl;
    	}
    	else out[0] << "Error parsing the input file" << endl;

        events.clear();
        conexions.clear();
        if(createDiccs) cout << "Dic size: " << dic.size() << endl;
    }
    sp.close_session(sid);

    if(createDiccs) printDic();
}