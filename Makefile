CC=g++
EXT=.exe

CFLAGS=$(shell pkg-config --cflags libraw opencv exiv2 lcms2)
LIBS=$(shell pkg-config --libs libraw opencv exiv2 lcms2)

CFLAGS+=-DLIBRAW_NODLL
LIBS+=-ljpeg -lws2_32

gimg: gimg.o ImageContainer.o ImageProcessor.o
	$(CC) -pthread -o gimg gimg.o ImageContainer.o ImageProcessor.o $(LIBS)

gimg.o: gimg.cpp
	$(CC) $(CFLAGS) -o gimg.o -c gimg.cpp

ImageProcessor.o: ImageProcessor.cpp
	$(CC) $(CFLAGS) -o ImageProcessor.o -c ImageProcessor.cpp

ImageContainer.o: ImageContainer.cpp
	$(CC) $(CFLAGS) -o ImageContainer.o -c ImageContainer.cpp

clean:
	rm -rf *.o gimg$(EXT)
