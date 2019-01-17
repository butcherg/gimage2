#ifndef _ImageContainer_h
#define _ImageContainer_h


//OpenCV:
#include <opencv2/core.hpp>

//Exiv2:
#include <exiv2/exiv2.hpp>

//LittleCMS2
#include <lcms2.h>

//std library:
#include <string>


class ImageContainer
{
public:
	ImageContainer();
	ImageContainer(const ImageContainer &imagecontainer);

	bool openFile(std::string filename, std::map<std::string, std::string> params = std::map<std::string, std::string>());
	bool saveFile(std::string filename, std::map<std::string, std::string> params = std::map<std::string, std::string>());

	cv::Mat& getImage();
	Exiv2::ExifData& getEXIFData();
	Exiv2::IptcData& getIPTCData();
	Exiv2::XmpData& getXMPData();
	cmsHPROFILE& getProfile();

	bool isOk();
	int getWidth();
	int getHeight();
	int getColors();

	void displayImage();
	void printMetadata();


protected:
	cv::Mat image;
	Exiv2::ExifData exifdata;
	Exiv2::IptcData iptcdata;
	Exiv2::XmpData xmpdata;
	cmsHPROFILE profile;

};

#endif
