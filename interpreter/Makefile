all: driver.o

analyze: driver.o
	lamac -b ../performance/Sort.lama
	./driver -a Sort.bc

analyze24: driver.o
	lamac -b ../regression/test024.lama
	./driver -a test024.bc

run1: driver.o byterun
	cat ../regression/test001.lama
	lamac -b ../regression/test001.lama
	../byterun/-byterun test001.bc
	./driver test001.bc

run24: driver.o byterun
	cat ../regression/test024.lama
	lamac -b ../regression/test024.lama
	../byterun/-byterun test024.bc
	./driver test024.bc

runSort: driver.o byterun
	cat ../performance/Sort.lama
	# lamac -b ../performance/Sort.lama - does not work, use precompiled Sort.bc file
	../byterun/-byterun Sort.bc
	./driver Sort.bc

driver.o: interpreter.o analyzer.o driver.cpp runtime
	g++ -m32 driver.cpp interpreter.o analyzer.o instructions.o ../runtime/runtime.a ../byterun/lib.o -o driver

interpreter.o: byterun runtime instructions.o utils.hpp interpreter.cpp interpreter.hpp
	g++ -m32 -c interpreter.cpp

analyzer.o: byterun instructions.o analyzer.hpp analyzer.cpp
	g++ -m32 -c analyzer.cpp

instructions.o: byterun utils.hpp instructions.cpp instructions.hpp
	g++ -m32 -c instructions.cpp

byterun: runtime
	$(MAKE) -C ../byterun

runtime:
	$(MAKE) -C ../runtime

clean:
	$(RM) *.a *.o *~ driver
