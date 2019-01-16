#ifndef _gimage_h
#define _gimage_h


//OpenCV:
#include <opencv2/core.hpp>

//Exiv2:
#include <exiv2/exiv2.hpp>

//LittleCMS2
#include <lcms2.h>

//std library:
//#include <iostream>
#include <string>


class gImage
{
public:
	gImage(std::string filename);
	bool saveToFile(std::string filename, std::string params);

	cv::Mat& getImage();
	Exiv2::ExifData& getMetadata();
	cmsHPROFILE& getProfile();

	bool isOk();
	int getWidth();
	int getHeight();
	int getColors();

	void displayImage();
	void printMetadata();

	void applyBlur();
	void applyResize(int width, int height);

private:
	cv::Mat image;
	Exiv2::ExifData metadata;
	cmsHPROFILE profile;

};

#endif
