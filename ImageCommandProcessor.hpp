#ifndef _ImageCommandProcessor_h
#define _ImageCommandProcessor_h

#include "ImageProcessor.hpp"

std::map<std::string, std::string> parseparams(std::string params);
std::vector<std::string> split(std::string s, std::string delim);

class ImageCommandProcessor: public ImageProcessor
{
public:
	bool applyBlur(std::string params);
	bool convertICCColorProfile(std::string params);
	//bool applyDemosaic();
	//bool applyNormalization();
	bool applyResize(std::string params);
	bool applySharpen(std::string params);
};

#endif
