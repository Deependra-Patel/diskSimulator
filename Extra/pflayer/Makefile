#PUBLICDIR= /usr0/cs564/public/project
SRC= buf.c hash.c pf.c
OBJ= buf.o hash.o pf.o
HDR = pftypes.h pf.h 

pflayer.o: $(OBJ)
	ld -r -o pflayer.o $(OBJ)

tests: testhash testpf

testpf: testpf.o pflayer.o
	cc -o testpf testpf.o pflayer.o

testhash: testhash.o pflayer.o
	cc -o testhash testhash.o pflayer.o

$(OBJ): $(HDR)

testhash.o: $(HDR)

testpf.o: $(HDR)

lint: 
	lint $(SRC)

install: pflayer.o 

clean:	
	rm -rf *.o *~ testhash testpf a.out file1 file2 report/finalreport.log report/finalreport.synctex.gz report/finalreport.aux

compile:
	g++ diskController.cpp diskRequestHandler.cpp main.cpp

data:
	./testData.py >input.txt
	make compile2
	./disk.exe <input.txt >a.txt
data2:
	make compile2
	./disk.exe <input.txt >a.txt

compile2:
	g++ -std=c++11 mainController.cpp cacheController.cpp diskController.cpp diskRequestHandler.cpp main.cpp -o disk.exe

elevatorDemo:
	g++ elevatorDemo.cpp diskRequestHandler.cpp
	./a.out
cacheDemo:
	g++ -std=c++11 cacheController.cpp diskController.cpp diskRequestHandler.cpp cacheDemo.cpp
	./a.out