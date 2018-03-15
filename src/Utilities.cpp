#include <cstdlib>
#include "Utilities.h"



void Utilities::error(string s)
{
  cout << s << endl;
  exit(1);
}

string Utilities::toHex(unsigned char a)
{
  string s="";
  unsigned char c=(a>>4)&0x0F;
  if (c==0)
    s+="0";
  if (c==1)
    s+="1";
  if (c==2)
    s+="2";
  if (c==3)
    s+="3";
  if (c==4)
    s+="4";
  if (c==5)
    s+="5";
  if (c==6)
    s+="6";
  if (c==7)
    s+="7";
  if (c==8)
    s+="8";
  if (c==9)
    s+="9";
  if (c==10)
    s += "A";
  if (c==11)
    s += "B";
  if (c==12)
    s += "C";
  if (c==13)
    s += "D";
  if (c==14)
    s += "E";
  if (c==15)
    s += "F";
  c=(a)&0x0F;
  if (c==0)
    s+="0";
  if (c==1)
    s+="1";
  if (c==2)
    s+="2";
  if (c==3)
    s+="3";
  if (c==4)
    s+="4";
  if (c==5)
    s+="5";
  if (c==6)
    s+="6";
  if (c==7)
    s+="7";
  if (c==8)
    s+="8";
  if (c==9)
    s+="9";
  if (c==10)
    s += "A";
  if (c==11)
    s += "B";
  if (c==12)
    s += "C";
  if (c==13)
    s += "D";
  if (c==14)
    s += "E";
  if (c==15)
    s += "F";
  return s;
}

string Utilities::toHex(int a)
{
  string s="";
  unsigned char c = (((unsigned char)a>>24)&0xFF);
  string temp = Utilities::toHex(c);
  s += temp;
  c = (((unsigned char)a>>16)&0xFF);
  temp = Utilities::toHex(c);
  s += temp;
  c = (((unsigned char)a>>8)&0xFF);
  temp = Utilities::toHex(c);
  s += temp;
  c = (((unsigned char)a)&0xFF);
  temp = Utilities::toHex(c);
  s += temp;
  return s;
}

unsigned int Utilities::getBits(unsigned int from, int start, int end)
{
  if (end<31)
    from = from >> (32-end-1);
  unsigned int temp = 1;
  temp = temp << (end-start+1);
  temp--;
  from = from & temp;
  return from;
}
