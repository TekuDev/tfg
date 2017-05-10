#ifndef _RELCLASS_H
#define _RELCLASS_H

#include <list>
#include "freeling/windll.h"
#include "freeling/morfo/language.h"
#include "freeling/omlet/classifier.h"
#include "freeling/morfo/configfile.h"
#include "featGenerator.h"
#include "freeling/omlet/svm.h"

using namespace std;
using namespace freeling;

class relationclassificator
{
 private: 
    /// classifier
    classifier *classif;

    ///dictionary
    map<string,int> dic;

  public:
	/// Constructor
	relationclassificator(const std::wstring &);
	/// Destructor
	~relationclassificator();

    /// predict from string
    void predict(const freeling::document &doc);

  private:
  	list<string> split(string s, char delim);
};


#endif