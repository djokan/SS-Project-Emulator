#include "Memory.h"
#include "Utilities.h"
#include "SegmentTable.h"
#include <iostream>
#include <sstream>
#include "SymbolTable.h"

SegmentTable* SegmentTable::mySegmentTable= nullptr;


SegmentTable::SegmentTable()
{
  
}

void SegmentTable::read(string &s)
{
  int start = s.find("#rel");
  if (start != string::npos)
  {
    int end = s.find("#end");
    int next=start;
    do
    {
      next= s.find("#",next+1);
      readSegment(s.substr(start,next-start-1));
    }while ((start = next)<end);
  }
  for (auto const a: SymbolTable::getSymbolTable()->symbols)
  {
    if (a->isSegment==true && a->name.substr(0,4)==".bss")
    {
      Segment *t = new Segment;
      t->reloc = a->reloc;
      t->adr = a->value;
      t->length = a->length;
      t->name = a->name;
      for (int ii=0;ii<a->length;ii++)
      {
        t->code[ii]=0x00;
      }
      getSegmentTable()->segments.push_back(t);
    }
  }
}

void SegmentTable::readSegment(string s)
{
  stringstream iss(s);
  string line;
  getline(iss,line);
  string name = line.substr(4);
  Symbol* s1= SymbolTable::getSymbol(name);
  Segment* s2 = new Segment;
  s2->adr = s1->value;
  s2->length = s1->length;
  s2->reloc = s1->reloc;
  s2->name = name;
  while(getline(iss,line) && line!=name)
  {
    RelInstance* rel = new RelInstance;
    stringstream ss1(line);
    string word;
    ss1 >> hex >> rel->adr >> word >> rel->number;
    rel->absolute=true;
    if (word == "R")
    {
      rel->absolute=false;
    }
    s2->relTable.push_back(rel);
  }
  int iter=0;
  unsigned int iii;
  unsigned char c;
  while (!iss.eof())
  {
    iss >> hex >> iii;
    c = iii&0xFF;
    s2->code[iter]=c;
    iter++;
  }
  getSegmentTable()->segments.push_back(s2);
}

SegmentTable* SegmentTable::getSegmentTable()
{
  if(mySegmentTable==nullptr)
    mySegmentTable = new SegmentTable();
  return mySegmentTable;
}

void SegmentTable::check()
{
  for (auto const a: getSegmentTable()->segments)
    for (auto const b: getSegmentTable()->segments)
    {
      if ((!a->reloc) && (!b->reloc))
      {
        if (a->name!=b->name)
        if ((a->adr<b->adr && a->adr+a->length>b->adr)||a->adr==b->adr)
        {
          Utilities::error("Segments use same address space: " + a->name + " and " + b->name);
        }
      }
    }
  unsigned long sz=0;
  for (auto const a: getSegmentTable()->segments)
  {
    sz += ((unsigned int) a->length);
    if (sz>0xFFFFFFFF)
    {
      Utilities::error("Size of segments is more than 4 GB");
    }
  }
}

void SegmentTable::load()
{
  for (int iaa=0;iaa<32*4+2;iaa++)
  {
    Memory::firstWrite(iaa,0);
  }
  unsigned int adr,len;
  for (auto const a: getSegmentTable()->segments)
  {
    if (!a->reloc)
    {
      adr = a->adr;
      len = a->length;
      for (int i=0;i<len;i++)
      {
        Memory::firstWrite(a->adr+i,a->code[i]);
        if (a->name.substr(0,4)==".bss")
        {
          Memory::blockExec(a->adr+i);
        }
        if (a->name.substr(0,7)==".rodata")
        {
          Memory::blockWrite(a->adr+i);
        }
      }
    }
  }
  unsigned long i=0;
  unsigned long temp;

  for (auto const a: getSegmentTable()->segments)
  {
    i=0;
    if (a->reloc)
    while (i<=0xFFFFFFFF-a->length+1)
    {
      for (temp=0;temp<a->length;temp++)
      {
        if (Memory::isAllocated(i+temp))
        {
          break;
        }
      }
      if (temp==a->length)
      {
        a->adr=i;
        len = a->length;
        SymbolTable::getSymbol(a->name)->value= i;
        for (unsigned int ii=0;ii<len;ii++)
        {
          Memory::firstWrite(i+ii,a->code[ii]);
          if (a->name.length()>3 && a->name.substr(0,4)==".bss")
          {
            Memory::blockExec(i+ii);
          }
          if (a->name.length()>6 && a->name.substr(0,7)==".rodata")
          {
            Memory::blockWrite(i+ii);
          }
        }
        break;
      }
      else
      {
        i+=temp;
        while (Memory::isAllocated(i))
        {
          i++;
        }
      }
    }
    if (i>0xFFFFFFFF-a->length+1)
    {
      Utilities::error("Can not allocate memory");
    }
  }
  for (auto const a:getSegmentTable()->segments)
  {
    for (auto const rel:a->relTable)
    {
      unsigned int addon=0;
      Symbol* sym = SymbolTable::getSymbol(rel->number);
      addon= sym->value;
      if (!sym->isSegment)
      {
        Symbol* seg = SymbolTable::getSymbol(sym->segment);
        addon += seg->value;
      }
      if (!rel->absolute)
      {
        addon-=a->adr+rel->adr;
      }
      Memory::addInt(a->adr+rel->adr,addon);
    }
  }
}
