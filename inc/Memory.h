#include <iostream>
#include <map>


using namespace std;

class Memory{
public:
map<unsigned int,unsigned char> memory;
map<unsigned int, unsigned char> writeBlocked;
map<unsigned int, unsigned char> execBlocked;
static void blockWrite(unsigned int);
static void blockExec(unsigned int);
static bool isBlockedExec(unsigned int);
static bool isBlockedWrite(unsigned int);
static Memory* myMemory;
static Memory* getMemory();
static unsigned char read(unsigned int);
static void write(unsigned int,unsigned char);
static void firstWrite(unsigned int,unsigned char);
static bool isAllocated(unsigned int);
static void writeInt(unsigned int, unsigned int);
static unsigned int readInt(unsigned int);
static unsigned int readInstr(unsigned int);
static void addInt(unsigned int, unsigned int);
static unsigned short readWord(unsigned int);
static void writeWord(unsigned int,unsigned short);
static void print(unsigned int, unsigned int);
static void push(unsigned int*, unsigned int);
static unsigned int pop(unsigned int*);
};
