#ifndef _FEATGEN_H
#define _FEATGEN_H

#include "event.h"
#include <map>
#include <vector>
#include <string>
#include <list>
#include <utility>

class featGenerator
{
public:
	list<event> events;
	map<string, int> dic;
	const string numericFeatures[3] = {"nWord","nSen","nVerb"};
	list< pair<string, string> > conexions;
	bool lexicalfeats, syntactfeats;
	ofstream out[2];
	string current_features;


	featGenerator();
	featGenerator(bool lexicalfeats, bool syntactfeats);
	~featGenerator();

	//Add elements to structures
	void createEvents(const list<paragraph::const_iterator> &ls);
	void addEvent(event e);
	void addFeatToDic(string feat);
	void addConexion(string ei, string ej);
	bool openFile(int i, string path);
	
	//Print structures to control
	void printEvents();
	void printConexions();

	//Principal functions, generate features and dicctionary
	void printDic(int numOfClasses);
	void printFeat(string feat);
	void printFeatsSet(set<string> feats);
	void generateFeatures(const list<paragraph::const_iterator> &ls);
	void generateLexicalFeats(event &ei, event &ej, const list<paragraph::const_iterator> &ls);
	void generateSyntacticalFeats(event &ei, event &ej, const list<paragraph::const_iterator> &ls);
	string generateFeatures2String(event &ei, event &ej, const list<paragraph::const_iterator> &ls);
	list<pair<int,int>> codeFeatures(list<string> &features, const map<string,int> &dic) const;

	//gets
	list<event> getEvents();
	list<std::pair<event,event>> getPairs();
	string getCurrentFeatures();

	//sets
	void setBooleans(bool lexicalfeats, bool syntactfeats);
	void resetCurrentFeatures();

private:
	string trataNumeric(string f);
	void printDateInfo(string dateInfo, string word);

	static bool sortFunc(pair<string,int> first, pair<string,int> second);
  	list<string> split(string s, char delim) const;
};

#endif