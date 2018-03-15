#ifndef LOADER_H
#define LOADER_H
#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <stdio.h>
#include <cstdlib>



#include <fstream>

using namespace std;

class Loader{
public:
  static Loader* myLoader;
  Loader();
  void load(ifstream&);
  static Loader* getLoader();
};

#endif /* LOADER_H */
