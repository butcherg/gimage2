sourcedir=.

CC=g++
EXT=.exe

CFLAGS=$(shell pkg-config --cflags libraw opencv exiv2 lcms2)
LIBS=$(shell pkg-config --libs libraw opencv exiv2 lcms2)

CFLAGS+=-DLIBRAW_NODLL
LIBS+=-ljpeg -lws2_32

gimg: gimg.o ImageContainer.o ImageProcessor.o ImageCommandProcessor.o
	$(CC) -pthread -o gimg gimg.o ImageContainer.o ImageProcessor.o ImageCommmandProcessor.o $(LIBS)

gimg.o: $(sourcedir)/gimg.cpp
	$(CC) $(CFLAGS) -o gimg.o -c $(sourcedir)/gimg.cpp

ImageCommandProcessor.o: $(sourcedir)/ImageCommandProcessor.cpp
	$(CC) $(CFLAGS) -o ImageCommandProcessor.o -c $(sourcedir)/ImageCommandProcessor.cpp

ImageProcessor.o: $(sourcedir)/ImageProcessor.cpp
	$(CC) $(CFLAGS) -o ImageProcessor.o -c $(sourcedir)/ImageProcessor.cpp

ImageContainer.o: $(sourcedir)/ImageContainer.cpp
	$(CC) $(CFLAGS) -o ImageContainer.o -c $(sourcedir)/ImageContainer.cpp

clean:
	rm -rf *.o gimg$(EXT)
