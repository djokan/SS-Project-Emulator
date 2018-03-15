FLAGS= -std=gnu++0x
INC_DIR=inc/
SRC_DIR=src/
SRC=Utilities.cpp main.cpp Loader.cpp Emulator.cpp SymbolTable.cpp SegmentTable.cpp Memory.cpp
ABS_SRC=$(addprefix $(SRC_DIR),$(SRC))
TARGET=../test/int
INCLUDE=$(addprefix -I,$(INC_DIR))
project: $(ABS_SRC) $(ABS_INC)
	g++ $(FLAGS) -o $(TARGET) $(INCLUDE) $(ABS_SRC) 
clean:
	rm -f $(TARGET)
