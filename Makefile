CC=g++
EXT=.exe

CFLAGS=$(shell pkg-config --cflags libraw opencv4 exiv2 lcms2)
LIBS=$(shell pkg-config --libs libraw opencv4 exiv2 lcms2)

CFLAGS+=-DLIBRAW_NODLL
LIBS+=-ljpeg -lws2_32

gimg: gimg.o gimage.o
	$(CC) -L/usr/local/lib -pthread -o gimg gimg.o gimage.o $(LIBS)

gimg.o: gimg.cpp
	$(CC) $(CFLAGS) -o gimg.o -c gimg.cpp

gimage.o: gimage.cpp
	$(CC) $(CFLAGS) -o gimage.o -c gimage.cpp

clean:
	rm -rf *.o gimg$(EXT)
