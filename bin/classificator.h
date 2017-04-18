#include <list>

#include "freeling/windll.h"
#include "freeling/morfo/language.h"

#include "freeling/omlet/classifier.h"

#include "freeling/morfo/configfile.h"
#include "featGenerator.h"
#include "freeling/omlet/svm.h"

using namespace std;
using namespace freeling;

class classificator
{
 private: 
    /// classifier
    const classifier* classif;

  public:
	/// Constructor
	classificator(const std::wstring &);
	/// Destructor
	~classificator();

    /// predict from string
    void predict(std::wstring &) const;
};
