#include "gimage.h"
#include <string>

//using namespace cv;
//using namespace std;

int main( int argc, char** argv )
{
	std::string imageName( "foo.jpg" ); // by default
	if( argc > 1) imageName = argv[1];

	gImage img(imageName);
	img.printMetadata();

	img.applyResize(640, 480);
	img.displayImage();

	imageName = "foo.png";
	if (argc > 2) imageName = argv[2];

	img.saveToFile(imageName, "");

	return 0;
}

