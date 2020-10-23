EXE = hw1.o
OBJS = tmp_hw1.o

all: $(EXE)

$(EXE): $(OBJS)
	g++ -g -o $@ $^

tmp_hw1.o: hw1.cpp
	g++ -c -o $@ $<
