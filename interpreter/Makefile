all: driver.o

analyze: driver.o
	lamac -b ../performance/Sort.lama
	./driver -a ../performance/Sort.bc

analyze1: driver.o
	lamac -b ../regression/test002.lama
	./driver -a ../regression/test002.bc

driver.o: interpreter.o analyzer.o driver.cpp
	g++ -m32 driver.cpp interpreter.o analyzer.o instructions.o ../runtime/runtime.a ../runtime/gc_runtime.o ../runtime/runtime.o ../byterun/lib.o -o driver

interpreter.o: byterun gc_runtime instructions.o utils.hpp interpreter.cpp interpreter.hpp
	g++ -m32 -c interpreter.cpp

analyzer.o: byterun instructions.o analyzer.hpp analyzer.cpp
	g++ -m32 -c analyzer.cpp

instructions.o: byterun utils.hpp instructions.cpp instructions.hpp
	g++ -m32 -c instructions.cpp

byterun: gc_runtime
	$(MAKE) -C ../byterun

gc_runtime:
	$(MAKE) -C ../runtime

clean:
	$(RM) *.a *.o *~ driver
