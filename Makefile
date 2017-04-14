PREFIX=/usr/local

CXXFLAGS=-std=c++98 -O3 -Wall -Wno-deprecated-declarations

all:
	g++ $(CXXFLAGS) -c -o src/ssl.o src/ssl.cc

	g++ $(CXXFLAGS) -c -o src/cami.o src/cami.cc

	mkdir -p lib

	ar rcs lib/libcami.a src/*.o

test: all
	g++ $(CXXFLAGS) -L ./lib -o test test.cc -lcami -lssl -lcrypto

install: all
	mkdir -p $(PREFIX)/include
	mkdir -p $(PREFIX)/lib

	cp include/cami.h $(PREFIX)/include/cami.h

	cp lib/libcami.a $(PREFIX)/lib/libcami.a

clean:
	rm -fr test test.dSYM src/*.o lib/*.a
