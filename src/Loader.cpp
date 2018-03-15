#include "Memory.h"
#include "Loader.h"
#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include "SymbolTable.h"
#include "SegmentTable.h"
#include <list>


using namespace std;
Loader* Loader::myLoader = nullptr;

Loader::Loader()
{
  
}

void Loader::load(ifstream& i)
{
  std::string str;

  i.seekg(0, std::ios::end);   
  str.reserve(i.tellg());
  i.seekg(0, std::ios::beg);

  str.assign((std::istreambuf_iterator<char>(i)),
             std::istreambuf_iterator<char>());
  SymbolTable::read(str);
  SegmentTable::read(str);
  SegmentTable::check();
  SegmentTable::load();
  
}

Loader* Loader::getLoader()
{
  if (myLoader==nullptr)
  {
    myLoader = new Loader();
  }
  return myLoader;
}
