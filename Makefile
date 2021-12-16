torhc.exe: torhc.o sha.o
	g++ torhc.o sha.o -o torhc.exe

torhc.o: torhc.cpp
	g++ -c torhc.cpp

mysha1.o: mysha1.cpp
	g++ -c mysha1.cpp

clean:
	rm *.o torhc.exe