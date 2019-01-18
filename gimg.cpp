#include "ImageCommandProcessor.hpp"
#include <string>

//using namespace cv;
//using namespace std;

void err(std::string msg) {
	printf("Error: %s\n", msg.c_str());
	exit(1);
}

int main( int argc, char** argv )
{
	std::map<std::string,std::string> p;
	std::string imageName; 
	ImageCommandProcessor img;
	
	if (argc < 2) err("Need at least a filename.\n");

	std::vector<std::string> inp = split(argv[1], ":");
	imageName = inp[0];
	if (inp.size() > 1) p =  parseparams(inp[1]);

	printf("open... "); fflush(stdout);
	if (!img.openFile(imageName, p)) err("File open failed.\n");
	printf("done.\n"); fflush(stdout);
	
	if (argc == 2) {
		printf("display... "); fflush(stdout);
		img.displayImage();
		printf("done.\n"); fflush(stdout);
		return 0;
	}
	
	if (argc == 3 && std::string(argv[2]) == "metadata") {
		img.printMetadata();
		return 0;
	}

	//execute image ops:
	for (unsigned i = 2; i < argc-1; i++) {
		std::vector<std::string> op = split(argv[i], ":");
		if (op[0] == "display") {
			printf("display... "); fflush(stdout);
			img.displayImage();
			printf("done.\n"); fflush(stdout);
		}
		if (op[0] == "resize") {
			printf("resize... "); fflush(stdout);
			if (op.size() < 2) err("resize error: needs parameters");
			img.applyResize(op[1]);
			printf("done.\n"); fflush(stdout);
		}
	}
	
	printf("save... "); fflush(stdout);
	imageName = argv[argc-1];
	img.saveFile(imageName);
	printf("done.\n"); fflush(stdout);

	return 0;
}

