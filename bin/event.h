#ifndef _EVENT_H
#define _EVENT_H

#include <iostream>
#include <string>
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

    event(const string &t) {
        type = util::string2wstring(t);
    }
    ~event() {};
};

#endif