CPP=g++
FLAGS=-g -I./ -DDEBUG


OBJS= cpu.o  sim.o

all: sim

sim: $(OBJS)
	$(CPP) $(FLAGS) $(OBJS) -o sim

sim.o: sim.cpp
	$(CPP) $(FLAGS) -c $^ -o sim.o

cpu.o: cpu.cpp
	$(CPP) $(FLAGS) -c $^ -o cpu.o


clean:
	rm -rf *.o
