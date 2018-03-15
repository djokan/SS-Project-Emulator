#include "SymbolTable.h"
#include <iostream>
#include <sstream>
#include "Utilities.h"


SymbolTable* SymbolTable::mySymbolTable = nullptr;

using namespace std;

void SymbolTable::read(string& str)
{
  int start = str.find("#TabelaSimbola");
  start = str.find("\n",start)+1;
  int end = str.find("#rel")-1;

  stringstream iss(str.substr(start,end-start));
  string line,word;

  int i=0;
  Symbol* s;
  while(getline(iss,line))
  {
    stringstream ln(line);
    ln >> word;
    s= new Symbol;
    if (word=="SEG")
    {
      s->isSegment = true;
      ln >> s->number >> s->name >> s->segment >> hex >> s->value >> hex >> s->length >> word;
      if (s->segment==0)
      {
        Utilities::error("Global Symbols are not allowed in emulator!");
      }
      s->global = false;
      s->reloc = false;
      if (word.find("G")!=string::npos)
      {
        s->global = true;
      }
      if (word.find("R")!=string::npos)
      {
        s->reloc = true;
      }
    }
    else
    {
      s->isSegment = false;
      ln >> s->number >> s->name >> s->segment >> hex >> s->value >> word;
      if (s->segment==0)
      {
        Utilities::error("Global Symbols are not allowed in emulator!");
      }
      s->global = false;
      s->reloc = false;
      if (word.find("G")!=string::npos)
      {
        
        s->global = true;
      }
    }
    getSymbolTable()->symbols.push_back(s);
  }
}

SymbolTable::SymbolTable()
{
  
}

SymbolTable* SymbolTable::getSymbolTable()
{
  if (mySymbolTable==nullptr)
    mySymbolTable = new SymbolTable();
  return mySymbolTable;
}

Symbol* SymbolTable::getSymbol(string s)
{
  for(auto const a:getSymbolTable()->symbols)
  {
    if (a->name == s)
      return a;
  }
  Utilities::error("Symbol " + s + " not found!");
}

Symbol* SymbolTable::getSymbol(unsigned int i)
{
  for(auto const a:getSymbolTable()->symbols)
  {
    if (a->number == i)
      return a;
  }
  Utilities::error("Symbol " + to_string(i) + " not found!");
}
