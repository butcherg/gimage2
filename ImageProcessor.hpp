#ifndef _ImageProcessor_h
#define _ImageProcessor_h

#include "ImageContainer.hpp"

class ImageProcessor: public ImageContainer
{
public:
	ImageProcessor(std::string filename);
	void applyBlur();
	void applyResize(int width, int height);
};

#endif
