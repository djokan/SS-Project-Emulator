#ifndef EMULATOR_H
#define EMULATOR_H

class Emulator{
public:
  static Emulator* myEmulator;
  bool end;
  unsigned int pc;
  unsigned int ip1;
  unsigned int ip2;
  unsigned int sp;
  unsigned int* R;
  bool allowInt;
  bool allowInput;
  unsigned int intSP;
  Emulator();
  void run();
  static Emulator* getEmulator();
  void readInstruction();
  void execInstruction();
  bool longInstruction();
  int getchr();
  bool isMemoryOperand();
  char kbhit();
  void interrupt();
  void xorr();
  void callTo(unsigned int);
  void store();
  unsigned int getMemoryLocation();
  void load();
  void add();
  void jz();
  void ret();
  void jmp();
  void call();
  void jnz();
  void jgz();
  void jgez();
  void jlz();
  void jlez();
  void push();
  void pop();
  void sub();
  void mul();
  void div();
  void mod();
  void andd();
  void orr();
  void nott();
  void asl();
  void asr();
};



#endif /* EMULATOR_H */
