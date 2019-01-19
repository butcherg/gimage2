#include "ImageCommandProcessor.hpp"
#include <string>

#include <opencv2/highgui.hpp>

//https://github.com/ccxvii/asstools/blob/master/getopt.c
/*
 * This is a version of the public domain getopt implementation by
 * Henry Spencer originally posted to net.sources.
 *
 * This file is in the public domain.
 */


#define getopt xgetopt
#define optarg xoptarg
#define optind xoptind

char *optarg; /* Global argument pointer. */
int optind = 0; /* Global argv index. */

static char *scan = NULL; /* Private scan pointer. */

int
getopt(int argc, char *argv[], char *optstring)
{
	char c;
	char *place;

	optarg = NULL;

	if (!scan || *scan == '\0') {
		if (optind == 0)
			optind++;

		if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
			return EOF;
		if (argv[optind][1] == '-' && argv[optind][2] == '\0') {
			optind++;
			return EOF;
		}

		scan = argv[optind]+1;
		optind++;
	}

	c = *scan++;
	place = strchr(optstring, c);

	if (!place || c == ':') {
		fprintf(stderr, "%s: unknown option -%c\n", argv[0], c);
		return '?';
	}

	place++;
	if (*place == ':') {
		if (*scan != '\0') {
			optarg = scan;
			scan = NULL;
		} else if( optind < argc ) {
			optarg = argv[optind];
			optind++;
		} else {
			fprintf(stderr, "%s: option requires argument -%c\n", argv[0], c);
			return ':';
		}
	}

	return c;
}

//end getopt.c

void err(std::string msg) {
	printf("Error: %s\n", msg.c_str());
	exit(1);
}

int main( int argc, char** argv )
{
	std::map<std::string,std::string> p;
	std::string imageName, displayName; 
	ImageCommandProcessor img;
	bool display=false;
	
	if (argc < 2) err("Need at least a filename.\n");

	int f;
	while ((f = getopt(argc, argv, (char *) "d")) != -1)
		switch(f) {
			case 'd':  //display each step
				display = true;
				break;
			default:
				exit(-1);

		}


	std::vector<std::string> inp = split(argv[optind], ":"); optind++;
	imageName = inp[0];
	if (inp.size() > 1) p =  parseparams(inp[1]);


	printf("open... "); fflush(stdout);
	if (!img.openFile(imageName, p)) err("File open failed.\n");
	printf("done.\n"); fflush(stdout);
	
	//if (argc == 2) {
	//	printf("display... "); fflush(stdout);
	//	img.displayImage();
	//	printf("done.\n"); fflush(stdout);
	//	return 0;
	//}
	
	//if (argc == 3 && std::string(argv[2]) == "metadata") {
	//	img.printMetadata();
	//	return 0;
	//}

	displayName = imageName;
	if (display) cv::namedWindow(displayName);
	if (display) img.displayImage(displayName);

	//execute image ops:
	//for (unsigned i = 2; i < argc-1; i++) {
	for (unsigned i = optind; i<argc-1; i++) {
		std::vector<std::string> op = split(argv[i], ":");

		if (op[0] == "blur") {
			printf("blur... "); fflush(stdout);
			if (op.size() >= 2)
				img.applyBlur(op[1]);
			else
				img.applyBlur("");
			if (display) img.displayImage(displayName);
			printf("done.\n"); fflush(stdout);
		}

		else if (op[0] == "demosaic") {
			printf("demosaic... "); fflush(stdout);
			img.applyDemosaic();
			if (display) img.displayImage(displayName);
			printf("done.\n"); fflush(stdout);
		}

		else if (op[0] == "display") {
			printf("display... "); fflush(stdout);
			if (display) img.displayImage(displayName);
			printf("done.\n"); fflush(stdout);
 		}
		
		else if (op[0] == "log") {
			printf("log... "); fflush(stdout);
			img.applyLog();
			if (display) img.displayImage(displayName);
			printf("done.\n"); fflush(stdout);
		}
		
		else if (op[0] == "normalize") {
			printf("normalize... "); fflush(stdout);
			img.applyNormalization();
			if (display) img.displayImage(displayName);
			printf("done.\n"); fflush(stdout);
		}

		else if (op[0] == "resize") {
			printf("resize... "); fflush(stdout);
			if (op.size() < 2) 
				err("resize error: needs parameters");
			else
				img.applyResize(op[1]);
			if (display) img.displayImage(displayName);
			printf("done.\n"); fflush(stdout);
		}

		else if (op[0] == "sharpen") {
			printf("sharpen... "); fflush(stdout);
			if (op.size() >= 2)
				img.applySharpen(op[1]);
			else
				img.applySharpen("");
			if (display) img.displayImage(displayName);
			printf("done.\n"); fflush(stdout);
		}

		else printf("%s... unrecognized operator.\n",op[0].c_str()); fflush(stdout);
	}
	
	printf("save... "); fflush(stdout);
	imageName = argv[argc-1];
	img.saveFile(imageName);
	printf("done.\n"); fflush(stdout);

	return 0;
}

