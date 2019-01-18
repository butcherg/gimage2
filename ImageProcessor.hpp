#ifndef _ImageProcessor_h
#define _ImageProcessor_h

#include "ImageContainer.hpp"

class ImageProcessor: public ImageContainer
{
public:
	void applyBlur();
	void applyDemosaic();
	void applyNormalization();
	void applyResize(int width, int height);
};

#endif
