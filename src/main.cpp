#include "Utilities.h"
#include "Loader.h"
#include "Emulator.h"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include "Memory.h"

using namespace std;
int main(int argc, char *argv[])
{
  if (argc!=2)
  {
    cout << "Invalid number of arguments" << endl;
    exit(1);
  }
  ifstream in;
  in.open(argv[1]);
  if (in==NULL)
  {
    cout << "Input file not found or locked" << endl;
    exit(1);
  }
  Loader::getLoader()->load(in);
  Emulator::getEmulator()->run();
  cout << endl;
  in.close();
  return 0;
}




