#include <algorithm>
#include <cstring>
#include "Memory.h"
#include <iostream>
#include "Emulator.h"
#include "SymbolTable.h"
#include "Utilities.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctime>

char Emulator::kbhit()
{
  struct termios oldt, newt;
  int ch;
  int oldf;
 
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
  ch = getchar();
 
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);
 
  return ch;
}

Emulator* Emulator::myEmulator = nullptr;

void Emulator::run()
{

  Symbol* start =SymbolTable::getSymbol("START");
  if (start->segment==-1)
  {
    R[17] = start->value;
  }
  else {
    Symbol* startseg =SymbolTable::getSymbol(start->segment);
    R[17]= start->value + startseg->value;
  }
  int ch;
  R[16] = Memory::readInt(0);
  clock_t begint = clock(),endt;
  while(!end)
  { 
    readInstruction();
    execInstruction();
    if (allowInt && allowInput && (ch=kbhit())!=EOF )
    {
      allowInt=false;
      allowInput=false;
      intSP=R[16];
      Memory::write(129,(unsigned char)(ch&0xFF));
      callTo(Memory::readInt(20));
    }
    endt = clock();
    if (allowInt && (((double)(endt-begint)/CLOCKS_PER_SEC)>0.1))
    {
      begint+=0.1*CLOCKS_PER_SEC;
      allowInt=false;
      intSP=R[16];
      callTo(Memory::readInt(16));
    }
  }
}

void Emulator::readInstruction()
{
  //  cout << R[17] << endl;
  if (Memory::isBlockedExec(R[17]))
  {
    Utilities::error("Can't execute data on address " + to_string(R[17]));
  }
  ip1 = Memory::readInstr(R[17]);
  R[17]+=4;
  if (longInstruction())
  {
    if (Memory::isBlockedExec(R[17]))
    {
      Utilities::error("Can't execute data on address " + to_string(R[17]));
    }
    ip2 = Memory::readInt(R[17]);
    R[17]+=4;
  }
}

bool Emulator::longInstruction()
{
  unsigned int t = Utilities::getBits(ip1,8,10);
  if (t==4 || t==6 || t==7)
  {
    return true;
  }
  return false;
}

void Emulator::jz()
{
  unsigned int reg = R[Utilities::getBits(ip1,16,20)];
  unsigned int op = Utilities::getBits(ip1,8,10);
  unsigned int adr;
  if (reg == 0)
  {
    if (op==6)
    {
      adr = ip2;
    }
    if (op==2)
    {
      adr = R[Utilities::getBits(ip1,11,15)];
    }
    if (op==7)
    {
      adr = R[Utilities::getBits(ip1,11,15)]+ip2;
    }
    R[17] = adr;
  }
}


void Emulator::execInstruction()
{
  unsigned char opCode = Utilities::getBits(ip1,0,7);
  switch (opCode)
  {
    case 0x00:
      interrupt();
      break;
    case 0x01:
      ret();
      break;
    case 0x02:
      jmp();
      break;
    case 0x03:
      call();
      break;
    case 0x04:
      jz();
      break;
    case 0x05:
      jnz();
      break;
    case 0x06:
      jgz();
      break;
    case 0x07:
      jgez();
      break;
    case 0x08:
      jlz();
      break;
    case 0x09:
      jlez();
      break;
    case 0x10:
      load();
      break;
    case 0x11:
      store();
      break;
    case 0x20:
      push();
      break;
    case 0x21:
      pop();
      break;
    case 0x30:
      add();
      break;
    case 0x31:
      sub();
      break;
    case 0x32:
      mul();
      break;
    case 0x33:
      div();
      break;
    case 0x34:
      mod();
      break;
    case 0x35:
      andd();
      break;
    case 0x36:
      orr();
      break;
    case 0x37:
      xorr();
      break;
    case 0x38:
      nott();
      break;
    case 0x39:
      asl();
      break;
    case 0x3A:
      asr();
      break;
    default:   
      callTo(Memory::readInt(12));
      break;
  }
}

void Emulator::asr()
{
  unsigned int i1,i2,i3;
  i1= Utilities::getBits(ip1,11,15);
  i2= Utilities::getBits(ip1,16,20);
  i3= Utilities::getBits(ip1,21,25);
  R[i1] = R[i2] >> R[i3];

  
}
void Emulator::asl()
{
  unsigned int i1,i2,i3;
  i1= Utilities::getBits(ip1,11,15);
  i2= Utilities::getBits(ip1,16,20);
  i3= Utilities::getBits(ip1,21,25);
  R[i1] = R[i2] << R[i3];
}
void Emulator::nott()
{
  unsigned int i1,i2,i3;
  i1= Utilities::getBits(ip1,11,15);
  i2= Utilities::getBits(ip1,16,20);
  i3= Utilities::getBits(ip1,21,25);

  R[i1] = ~R[i2];
}
void Emulator::orr()
{
  unsigned int i1,i2,i3;
  i1= Utilities::getBits(ip1,11,15);
  i2= Utilities::getBits(ip1,16,20);
  i3= Utilities::getBits(ip1,21,25);
  R[i1] = R[i2] | R[i3];
  
}
void Emulator::andd()
{
  unsigned int i1,i2,i3;
  i1= Utilities::getBits(ip1,11,15);
  i2= Utilities::getBits(ip1,16,20);
  i3= Utilities::getBits(ip1,21,25);
  R[i1] = R[i2] & R[i3];
  
}
void Emulator::mod()
{
  unsigned int i1,i2,i3;
  i1= Utilities::getBits(ip1,11,15);
  i2= Utilities::getBits(ip1,16,20);
  i3= Utilities::getBits(ip1,21,25);
  R[i1] = R[i2] % R[i3];
  
}
void Emulator::div()
{
  unsigned int i1,i2,i3;
  i1= Utilities::getBits(ip1,11,15);
  i2= Utilities::getBits(ip1,16,20);
  i3= Utilities::getBits(ip1,21,25);
  R[i1] = R[i2] / R[i3];
  
}
void Emulator::mul()
{
  unsigned int i1,i2,i3;
  i1= Utilities::getBits(ip1,11,15);
  i2= Utilities::getBits(ip1,16,20);
  i3= Utilities::getBits(ip1,21,25);
  R[i1] = R[i2] * R[i3];
}
void Emulator::sub()
{
  unsigned int i1,i2,i3;
  i1= Utilities::getBits(ip1,11,15);
  i2= Utilities::getBits(ip1,16,20);
  i3= Utilities::getBits(ip1,21,25);
  R[i1] = R[i2] - R[i3];
}
void Emulator::pop()
{
  unsigned int i1;
  i1= Utilities::getBits(ip1,11,15);
  R[i1] = Memory::pop(&R[16]);
}
void Emulator::push()
{
  unsigned int i1;
  i1= Utilities::getBits(ip1,11,15);
  Memory::push(&R[16],R[i1]);
}
void Emulator::jmp()
{
  unsigned int reg = R[Utilities::getBits(ip1,16,20)];
  unsigned int op = Utilities::getBits(ip1,8,10);
  unsigned int adr;  
  if (op==6)
  {
    adr = ip2;
  }
  if (op==2)
  {
    adr = R[Utilities::getBits(ip1,11,15)];
  }
  if (op==7)
  {
    adr = R[Utilities::getBits(ip1,11,15)]+ip2;
  }
  R[17] = adr;
}
void Emulator::call()
{
  unsigned int op = Utilities::getBits(ip1,8,10);
  unsigned int adr;  
  if (op==6)
  {
    adr = ip2;
  }
  if (op==2)
  {
    adr = R[Utilities::getBits(ip1,11,15)];
  }
  if (op==7)
  {
    adr = R[Utilities::getBits(ip1,11,15)]+ip2;
  }

  callTo(adr);
  
}
void Emulator::jnz()
{
  unsigned int reg = R[Utilities::getBits(ip1,16,20)];
  unsigned int op = Utilities::getBits(ip1,8,10);
  unsigned int adr;
  if (reg != 0)
  {
    if (op==6)
    {
      adr = ip2;
    }
    if (op==2)
    {
      adr = R[Utilities::getBits(ip1,11,15)];
    }
    if (op==7)
    {
      adr = R[Utilities::getBits(ip1,11,15)]+ip2;
    }
    R[17] = adr;
  }

}
void Emulator::jgz()
{
  unsigned int reg = R[Utilities::getBits(ip1,16,20)];
  unsigned int op = Utilities::getBits(ip1,8,10);
  unsigned int adr;
  if (reg>0 && reg<0x80000000)
  {
    
    if (op==6)
    {
      adr = ip2;
    }
    if (op==2)
    {
      adr = R[Utilities::getBits(ip1,11,15)];
    }
    if (op==7)
    {
      adr = R[Utilities::getBits(ip1,11,15)]+ip2;
    }
    R[17] = adr;
  }

}
void Emulator::jgez()
{
  unsigned int reg = R[Utilities::getBits(ip1,16,20)];
  unsigned int op = Utilities::getBits(ip1,8,10);
  unsigned int adr;
  if (reg<0x80000000)
  {  
    if (op==6)
    {
      adr = ip2;
    }
    if (op==2)
    {
      adr = R[Utilities::getBits(ip1,11,15)];
    }
    if (op==7)
    {
      adr = R[Utilities::getBits(ip1,11,15)]+ip2;
    }
    R[17] = adr;
  }

}
void Emulator::jlz()
{
  unsigned int reg = R[Utilities::getBits(ip1,16,20)];
  unsigned int op = Utilities::getBits(ip1,8,10);
  unsigned int adr;
  if (reg>=0x80000000)
  {
    
    if (op==6)
    {
      adr = ip2;
    }
    if (op==2)
    {
      adr = R[Utilities::getBits(ip1,11,15)];
    }
    if (op==7)
    {
      adr = R[Utilities::getBits(ip1,11,15)]+ip2;
    }
    R[17] = adr;
  }

}
void Emulator::jlez()
{
  unsigned int reg = R[Utilities::getBits(ip1,16,20)];
  unsigned int op = Utilities::getBits(ip1,8,10);
  unsigned int adr;
  if (reg == 0 || reg>=0x80000000)
  {   
    if (op==6)
    {
      adr = ip2;
    }
    if (op==2)
    {
      adr = R[Utilities::getBits(ip1,11,15)];
    }
    if (op==7)
    {
      adr = R[Utilities::getBits(ip1,11,15)]+ip2;
    }
    R[17] = adr;
  }
}

void Emulator::ret()
{
  R[17] = Memory::pop(&R[16]);
}


void Emulator::load()
{
  unsigned int new1,val;
  unsigned int op;
  op = Utilities::getBits(ip1,8,10);
  unsigned int reg = Utilities::getBits(ip1,16,20);
  unsigned int adr;
  unsigned int type = Utilities::getBits(ip1,26,28);
  if (op==6 || op==2 || op==7)
  {

    if (op==6)
    {
      adr = ip2;
    }
    if (op==2)
    {

      adr = R[Utilities::getBits(ip1,11,15)];
    }
    if (op==7)
    {
      adr = R[Utilities::getBits(ip1,11,15)]+ip2;
    }
    
    if (type==0)
      val = Memory::readInt(adr);
    if (type==1)
    {
      val = Memory::readWord(adr);
    }
    if (type==5)
    {
      unsigned short us = Memory::readWord(adr);
      val = us;
      if (us>=0x8000)
      {
        val = 0xFFFF0000 | us;
      }
      
    }
    if (type == 3)
      val = Memory::read(adr);
    if (type==7)
    {
      unsigned char us = Memory::readWord(adr);
      val = us;
      if (us>=0x80)
      {
        val = 0xFFFFFF00 | us;
      }
      
    }
  }
  if (op==4)
  {
    val = ip2;
    
  }
  if (op==0)
  {
    val = R[Utilities::getBits(ip1,11,15)];
  }

  if (op==4 || op==0)
  {
    if (type==1)
    {
      val = 0x0000FFFF & val;
    }
    if (type==5)
    {
      val = 0x0000FFFF & val;
      if (val>=0x8000)
      {
        val = 0xFFFF0000 | val;
      }
    }
    if (type==3)
    {
      val = 0x000000FF & val;
    }
    if (type==7)
    {
      val = 0x000000FF & val;
      if (val>=0x8000)
      {
        val = 0xFFFFFF00 | val;
      }
    }
    
  }

  R[reg]= val;


  Utilities::getBits(ip1,16,20);
  
  
}

void Emulator::callTo(unsigned int a)
{
  Memory::push(&R[16],R[17]);
  R[17] = a;
}

void Emulator::xorr()
{
  unsigned int i1,i2,i3;
  i1= Utilities::getBits(ip1,11,15);
  i2= Utilities::getBits(ip1,16,20);
  i3= Utilities::getBits(ip1,21,25);
  R[i1] = R[i2] ^ R[i3];
}

void Emulator::add()
{
  unsigned int i1,i2,i3;
  i1= Utilities::getBits(ip1,11,15);
  i2= Utilities::getBits(ip1,16,20);
  i3= Utilities::getBits(ip1,21,25);
  R[i1] = R[i2] + R[i3];
}


void Emulator::interrupt()
{
  unsigned int reg = Utilities::getBits(ip1,11,15);
  if (allowInt==false)
  {
    Utilities::error("Runtime error interrupt called in interrupt!");
  }
  if (R[reg]==0)
  {
    end=true;
  }
  else
  {
    allowInt=false;
    intSP=R[16];
    callTo(Memory::readInt(R[reg]*4));
  }
}

void Emulator::store()
{
  unsigned int op;
  op = Utilities::getBits(ip1,8,10);
  unsigned int reg = R[Utilities::getBits(ip1,16,20)];
  unsigned int adr;
  unsigned int type = Utilities::getBits(ip1,26,28);
  if (op==4)
  {
    callTo(20);
  }


  if (op==6 || op==2 || op==7)
  {
    if (op==6)
    {
      adr = ip2;
    }
    if (op==2)
    {
      adr = R[Utilities::getBits(ip1,11,15)];
    }
    if (op==7)
    {
      adr = R[Utilities::getBits(ip1,11,15)]+ip2;
    }
  }
  

  
  if (op==6|| op==2 || op==7)
  {
    if (type==0)
      Memory::writeInt(adr,reg);
    if (type==1 || type==5)
      Memory::writeWord(adr,(unsigned short)reg);
    if (type == 3 || type == 7)
      Memory::write(adr,(unsigned char)reg);
  }
  if (op==0)
  {
    R[Utilities::getBits(ip1,11,15)] = reg;
  }


}

unsigned int Emulator::getMemoryLocation()
{
  
}

bool Emulator::isMemoryOperand()
{
  unsigned int op = Utilities::getBits(ip1,8,10);
  if (op == 2 || op==6 || op == 7 )
  {
    return true;
  }
  return false;
}





Emulator::Emulator()
{
  end=false;
  R= new unsigned int[18];
  allowInput=true;
  allowInt=true;
}


Emulator* Emulator::getEmulator()
{
  if (myEmulator==nullptr)
  {
    myEmulator = new Emulator();
  }
  return myEmulator;
}
 
