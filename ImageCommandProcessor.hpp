#ifndef _ImageCommandProcessor_h
#define _ImageCommandProcessor_h

#include "ImageProcessor.hpp"

std::map<std::string, std::string> parseparams(std::string params);
std::vector<std::string> split(std::string s, std::string delim);

class ImageCommandProcessor: public ImageProcessor
{
public:
	//void applyBlur();
	//void applyDemosaic();
	//void applyNormalization();
	void applyResize(std::string params);
};

#endif