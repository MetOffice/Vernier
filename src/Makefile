

OBJ=hashtable.o  profiler.o 

all: lib
	g++ -DNDEBUG -O3 -fopenmp -std=c++17 main.cpp -L. -lprofiler

lib: $(OBJ)
	ar crs libprofiler.a *.o
  
clean:
	rm -f main.x *.o *.out

%.o: %.cpp
	g++ -c -DNDEBUG -O3 -fopenmp -std=c++17 -o $@ $<

