CC=g++
EXT=

LIBS=$(shell pkg-config --libs opencv exiv2 lcms2 zlib)

gimg: gimg.o gimage.o
	$(CC) -L/usr/local/lib -pthread -o gimg gimg.o gimage.o $(LIBS)

gimg.o: gimg.cpp
	$(CC) -o gimg.o -c gimg.cpp

gimage.o: gimage.cpp
	$(CC) -o gimage.o -c gimage.cpp

clean:
	rm -rf *.o gimg$(EXT)
