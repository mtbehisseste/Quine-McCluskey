EXE = hw1
OBJS = hw1.o

all: $(EXE)

$(EXE): $(OBJS)
	g++ -g -o $@ $^

%.o: %.cpp
	g++ -c -o $@ $<
