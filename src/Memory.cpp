#include "Memory.h"
#include "SymbolTable.h"
#include "Utilities.h"
#include "Emulator.h"

Memory* Memory::myMemory = nullptr;


void Memory::write(unsigned int i, unsigned char c)
{
  if (getMemory()->memory.find(i) == getMemory()->memory.end())
  {
    Utilities::error("Runtime error: acessed memory that is not allocated: " + to_string(i));
  }
  if (isBlockedWrite(i))
  {
    Utilities::error("Can't write to address " + to_string(i) + ", write protected!");
  }
  if (i==128)
  {
    cout << c;
  }
  getMemory()->memory[i]=c;
}

void Memory::firstWrite(unsigned int i, unsigned char c)
{
  getMemory()->memory[i]=c;
}

bool Memory::isAllocated(unsigned int i)
{
  if (getMemory()->memory.find(i) == getMemory()->memory.end())
  {
    return false;
  }
  return true;
}


unsigned char Memory::read(unsigned int i)
{
  if (getMemory()->memory.find(i) == getMemory()->memory.end())
  {
    Utilities::error("Runtime error: acessed memory that is not allocated: "  + to_string(i) );
  }
  if (i == 129)
  {
    Emulator::getEmulator()->allowInput=true;
  }
  return getMemory()->memory[i];
}

unsigned int Memory::readInt(unsigned int a)
{
  unsigned char c;
  unsigned int i=0;
  c= read(a+3);
  i = (i<<8) + c;
  c= read(a+2);
  i = (i<<8) + c;
  c= read(a+1);
  i = (i<<8) + c;
  c= read(a);
  i = (i<<8) + c;
  return i;
}

unsigned int Memory::readInstr(unsigned int a)
{
  unsigned char c;
  unsigned int i=0;
  c= read(a);
  i = (i<<8) + c;
  c= read(a+1);
  i = (i<<8) + c;
  c= read(a+2);
  i = (i<<8) + c;
  c= read(a+3);
  i = (i<<8) + c;
  return i;
}


void Memory::writeInt(unsigned int a, unsigned int b)
{
  unsigned char c;
  c=b&0xFF;
  write(a,c);
  c=(b>>8)&0xFF;
  write(a+1,c);
  c=(b>>16)&0xFF;
  write(a+2,c);
  c=(b>>24)&0xFF;
  write(a+3,c);
}


void Memory::addInt(unsigned int a, unsigned int b)
{
  unsigned char c,d;
  
  unsigned int i= readInt(a);

  b+=i;

  writeInt(a,b);
}

void Memory::print(unsigned int a, unsigned int b)
{
  unsigned int i;
  unsigned char pp = 0x00;
  for (i=a+10;i<b;i+=10)
  {
    cout << i-10 << ": ";
    for (unsigned int p=i-10;p<i;p++)
    {
      pp=0x00;
      if (isAllocated(p))
      {
        pp = read(p);
      }
      cout << Utilities::toHex(pp) << " ";
    }
    cout << endl;
  }
  cout << i-10 << ": ";
  for (i=i-10;i<b;i++)
  {
    pp=0x00;
    if (isAllocated(i))
    {
      pp = read(i);
    }
    cout << Utilities::toHex(pp) << " ";
  }
  cout << endl;
}

Memory* Memory::getMemory()
{
  if (myMemory==nullptr)
    myMemory = new Memory();
  return myMemory;
}

void Memory::push(unsigned int* sp, unsigned int b)
{
  unsigned char c = (b>>24)&0xFF;
  (*sp)++;
  Memory::write(*sp,c);
  c = (b>>16)&0xFF;
  (*sp)++;
  Memory::write(*sp,c);
  c = (b>>8)&0xFF;
  (*sp)++;
  Memory::write(*sp,c);
  c = (b)&0xFF;
  (*sp)++;
  Memory::write(*sp,c);
}

unsigned int Memory::pop(unsigned int* sp)
{
  unsigned int b=0;
  unsigned char c;
  c= Memory::read(*sp);
  (*sp)--;
  b = (b)|(c);
  c= Memory::read(*sp);
  (*sp)--;
  b = (b)|(c<<8);
  c= Memory::read(*sp);
  (*sp)--;
  b = (b)|(c<<16);
  c= Memory::read(*sp);
  (*sp)--;
  b = (b)|(c<<24);
  if (*sp==Emulator::getEmulator()->intSP)
  {
    Emulator::getEmulator()->allowInt=true;
  }
  return b;
}

unsigned short Memory::readWord(unsigned int a)
{
  unsigned char c;
  unsigned short i=0;
  c= read(a+1);
  i = (i<<8) + c;
  c= read(a);
  i = (i<<8) + c;
  return i; 
}

void Memory::writeWord(unsigned int a,unsigned short b)
{
  unsigned char c;
  c=b&0xFF;
  write(a,c);
  c=(b>>8)&0xFF;
  write(a+1,c);
}

void Memory::blockWrite(unsigned int a)
{
  getMemory()->writeBlocked[a]='d';
}

void Memory::blockExec(unsigned int a)
{
  getMemory()->execBlocked[a]='d';
}

bool Memory::isBlockedWrite(unsigned int i)
{
  if (getMemory()->writeBlocked.find(i) == getMemory()->writeBlocked.end())
  {
    return false;
    
  }
  else
  {
    return true;
  }
}
bool Memory::isBlockedExec(unsigned int i)
{
  if (getMemory()->execBlocked.find(i) == getMemory()->execBlocked.end())
  {
    return false;
  }
  else
  {
    return true;
  }
}
