#run make data, or make elevatorDemo or make cacheDemo
compile:
	g++ -std=c++11 mainController.cpp cacheController.cpp diskController.cpp diskRequestHandler.cpp main.cpp -o disk.exe

data:
	./testData.py >input.txt
	make compile
	./disk.exe <input.txt >a.txt
data2:
	make compile
	./disk.exe <input.txt >a.txt
elevatorDemo:
	g++ elevatorDemo.cpp diskRequestHandler.cpp
	./a.out
cacheDemo:
	g++ -std=c++11 cacheController.cpp diskController.cpp diskRequestHandler.cpp cacheDemo.cpp
	./a.out

clean:	
	rm -rf *.o *~ testhash a.txt input.txt testpf disk.exe a.out file1 file2 report/finalreport.log report/finalreport.synctex.gz report/finalreport.aux
