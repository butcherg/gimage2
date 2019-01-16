#include "gimage.h"

#include "opencv2/imgproc.hpp"
#include <opencv2/highgui.hpp>



#include <exiv2/exiv2.hpp>

#include "lcms2.h"

#include <iostream>
#include <string>


gImage::gImage(std::string filename)
{
	image = cv::imread(filename, cv::IMREAD_COLOR);
	if ((image.depth() == CV_8U) | (image.depth() == CV_8S)) image.convertTo(image, CV_32F,  1.0/256.0);
	if ((image.depth() == CV_16U) | (image.depth() == CV_16S)) image.convertTo(image, CV_32F,  1.0/65536.0);

	Exiv2::Image::AutoPtr img = Exiv2::ImageFactory::open(filename);
	assert(img.get() != 0);
	img->readMetadata();
	metadata = img->exifData();

	if (img->iccProfileDefined()) {
		Exiv2::DataBuf * prof = img->iccProfile();
		profile = cmsOpenProfileFromMem(prof->pData_, prof->size_);
	}
}

bool gImage::saveToFile(std::string filename, std::string params)
{
	bool result;
	std::vector<int> p;
	cv::Mat img = image.clone();
	img.convertTo(img, CV_8U, 256.0);
	result = cv::imwrite(filename, img, p);
	if (!result) return false;

	Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filename);
	assert(image.get() != 0);

	if (profile) {
		cmsUInt32Number bufsize;
		image->clearIccProfile();
		if (cmsSaveProfileToMem(profile, NULL, &bufsize)) {
			Exiv2::DataBuf prof((int) bufsize);
			cmsSaveProfileToMem(profile, prof.pData_, &bufsize);
			image->setIccProfile(prof);
		}
	}

	image->setExifData(metadata);
	image->writeMetadata();
	return true;
}

int gImage::getWidth()
{
	return image.cols;
}

int gImage::getHeight()
{
	return image.rows;
}

int gImage::getColors()
{
	return image.channels();
}

cv::Mat& gImage::getImage()
{
	return image;
}

Exiv2::ExifData& gImage::getMetadata()
{
	return metadata;
}

cmsHPROFILE& gImage::getProfile()
{	
	return profile;
}

void gImage::displayImage()
{
	cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
	cv::imshow("Display Image", image);
	cv::waitKey(0);
}

void gImage::printMetadata()
{
	char * buf; cmsUInt32Number bufsize;
	if (profile) {
		bufsize = cmsGetProfileInfoASCII(profile, cmsInfoDescription, cmsNoLanguage, cmsNoCountry, NULL, 0);
		if (bufsize) {
			buf = (char *) malloc(bufsize+1);
			cmsGetProfileInfoASCII(profile, cmsInfoDescription, cmsNoLanguage, cmsNoCountry, buf, bufsize);
			buf[bufsize] = '\0';
			printf("ICC Profile Description: %s\n", buf); 
			//free buf;
		}
	}
	else printf("No ICC Profile!!!\n");
	
	Exiv2::ExifData::const_iterator end = metadata.end();
	for (Exiv2::ExifData::const_iterator i = metadata.begin(); i != end; ++i) {
		const char* tn = i->typeName();
		std::cout << std::setw(44) << std::setfill(' ') << std::left
			<< i->key() << " "
			<< "0x" << std::setw(4) << std::setfill('0') << std::right
			<< std::hex << i->tag() << " "
			<< std::setw(9) << std::setfill(' ') << std::left
			<< (tn ? tn : "Unknown") << " "
			<< std::dec << std::setw(3)
			<< std::setfill(' ') << std::right
			<< i->count() << "  "
			<< std::dec << i->value()
			<< "\n";
	}

	fflush(stdout);
}


void gImage::applyBlur()
{
	cv::Mat d = image.clone();
	cv::blur( image, d, cv::Size( 3, 3 ), cv::Point(-1,-1) );
	image = d;
}

void gImage::applyResize(int width, int height)
{
	cv::Mat d = image.clone();
	cv::resize( image, d, cv::Size( width, height), 0, 0, CV_INTER_AREA );
	image = d;
}



