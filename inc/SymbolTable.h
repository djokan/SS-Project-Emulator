#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <iostream>
#include <list>



using namespace std;

struct Symbol{
  bool isSegment;
  int number;
  int value;
  string name;
  int segment;
  int length;
  bool global;
  bool reloc;
};


class SymbolTable{
public:
  list<Symbol*> symbols;
  static SymbolTable* mySymbolTable;
  static SymbolTable* getSymbolTable();
  SymbolTable();
  static void read(string& s);
  static Symbol* getSymbol(string);
  static Symbol* getSymbol(unsigned int);
};




#endif /* SYMBOLTABLE_H */
