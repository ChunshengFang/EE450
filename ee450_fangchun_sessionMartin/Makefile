# the compiler: gcc for C program, define as g++ for C++
CC = g++

all: clean serverC.out serverS.out serverT.out serverP.out clientA.out clientB.out

serverC: serverC.out
	./serverC

serverT: serverT.out
	./serverT

serverP: serverP.out
	./serverP

serverS: serverS.out
	./serverS

serverC.out: central.cpp common.cpp
	@ $(CC) -Wall -std=c++11 -o serverC central.cpp common.cpp

serverS.out: serverS.cpp common.cpp
	@ $(CC) -Wall -std=c++11 -o serverS serverS.cpp common.cpp

serverT.out: serverT.cpp common.cpp
	@ $(CC) -Wall -std=c++11 -o serverT serverT.cpp common.cpp

serverP.out: serverP.cpp common.cpp
	@ $(CC) -Wall -std=c++11 -o serverP serverP.cpp common.cpp

clientA.out: clientA.cpp common.cpp
	@ $(CC) -Wall -std=c++11 -o clientA clientA.cpp common.cpp

clientB.out: clientB.cpp common.cpp
	@ $(CC) -Wall -std=c++11 -o clientB clientB.cpp common.cpp

clean:
	@ $(RM) serverC.out serverS.out serverT.out serverP.out clientA.out clientB.out
