#include "ImageProcessor.hpp"
#include <string>

//using namespace cv;
//using namespace std;

int main( int argc, char** argv )
{
	std::string imageName( "foo.jpg" ); // by default
	if( argc > 1) imageName = argv[1];

	ImageProcessor img;
	if (!img.openFile(imageName)) return 1;
	img.printMetadata();

	img.applyResize(640, 0);
	img.displayImage();

	imageName = "foo.png";
	if (argc > 2) imageName = argv[2];

	img.saveFile(imageName, "");

	return 0;
}

