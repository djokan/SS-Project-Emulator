#ifndef UTILITIES_H
#define UTILITIES_H
#include <iostream>
using namespace std;

class Utilities{
public:
  static void error(string s);
  static string toHex(int);
  static string toHex(unsigned char);
  static unsigned int getBits(unsigned int from, int start, int end);
};




#endif /* UTILITIES_H */
