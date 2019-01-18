#ifndef _ImageProcessor_h
#define _ImageProcessor_h

#include "ImageContainer.hpp"

class ImageProcessor: public ImageContainer
{
public:
	void applyBlur(unsigned kernelsize=3);
	void applyDemosaic();
	void applyLog();
	void applyNormalization();
	void applyResize(int width, int height);
	void applySharpen(float strength);
};

#endif
