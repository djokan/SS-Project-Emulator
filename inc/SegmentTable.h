#ifndef SEGMENTTABLE_H
#define SEGMENTTABLE_H
#include <map>
#include <iostream>
#include <list>

using namespace std;

struct RelInstance{
  unsigned int adr;
  bool absolute;
  unsigned int number;
};

struct Segment
{
  unsigned int adr;
  string name;
  bool reloc;
  map<int,unsigned char> code;
  list<RelInstance*> relTable;
  unsigned int length;
};


class SegmentTable{
public:
  list<Segment*> segments;
  static SegmentTable* mySegmentTable;
  SegmentTable();
  static void read(string& s);
  static void readSegment(string s);
  static SegmentTable* getSegmentTable();
  static void check();
  static void load();
};

#endif /* SEGMENTTABLE_H */
