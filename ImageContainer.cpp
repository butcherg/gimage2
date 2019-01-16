#include "ImageProcessor.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <libraw/libraw.h>

#include <iostream>
#include <string>


ImageProcessor::ImageProcessor(std::string filename)
{
	printf("opening...\n"); fflush(stdout);
	
	//First, try raw file:
	LibRaw RawProcessor;
	int w, h, c, b; int t;
	
	RawProcessor.imgdata.params.shot_select = 0;
	RawProcessor.imgdata.params.use_camera_wb = 1;
	RawProcessor.imgdata.params.output_color = 1;	//sRGB
	RawProcessor.imgdata.params.user_qual = 3;	//AHD
	//RawProcessor.imgdata.params.no_auto_bright = 1;

	RawProcessor.imgdata.params.output_bps = 16;

	RawProcessor.imgdata.params.gamm[0] = 1/2.4;  //1/1.0
	RawProcessor.imgdata.params.gamm[1] = 12.92; //1.0
	
	if (RawProcessor.open_file(filename.c_str())==LIBRAW_SUCCESS) {
		RawProcessor.unpack();
		RawProcessor.dcraw_process();
		RawProcessor.get_mem_image_format(&w, &h, &c, &b);
		printf("w:%d h:%d c:%d b:%d\n",w,h,c,b); fflush(stdout); 
		if (b==8)  t = CV_8UC3;
		if (b==16) t = CV_16UC3;
		cv::Mat r(h, w, t, RawProcessor.dcraw_make_mem_image());
		image = r.clone();
		if (b==8)  image.convertTo(image, CV_32FC3, 1.0/256.0);
		if (b==16) image.convertTo(image, CV_32FC3, 1.0/65536.0);
		//cvtColor(image, image, cv::COLOR_RGB2BGR);
		cvtColor(image, image, cv::COLOR_BGR2RGB);
		LibRaw::dcraw_clear_mem((libraw_processed_image_t*) r.data);
		RawProcessor.recycle();
	}
	else {
		image = cv::imread(filename, cv::IMREAD_COLOR);
		if (image.data == NULL) {
			printf("couldn't open image.\n"); fflush(stdout);
			return;
		}
		if ((image.depth() == CV_8U) | (image.depth() == CV_8S)) image.convertTo(image, CV_32F,  1.0/256.0);
		if ((image.depth() == CV_16U) | (image.depth() == CV_16S)) image.convertTo(image, CV_32F,  1.0/65536.0);

	}
	
	Exiv2::Image::AutoPtr img = Exiv2::ImageFactory::open(filename);
	assert(img.get() != 0);
	img->readMetadata();
	metadata = img->exifData();

	if (img->iccProfileDefined()) {
		Exiv2::DataBuf * prof = img->iccProfile();
		profile = cmsOpenProfileFromMem(prof->pData_, prof->size_);
	}
	else profile = NULL;

}

bool ImageProcessor::saveToFile(std::string filename, std::string params)
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

	if (!metadata.empty()) {
		image->setExifData(metadata);
		image->writeMetadata();
	}
	return true;
}

bool ImageProcessor::isOk()
{
	if (image.data == NULL) return false;
	return true;
}

int ImageProcessor::getWidth()
{
	return image.cols;
}

int ImageProcessor::getHeight()
{
	return image.rows;
}

int ImageProcessor::getColors()
{
	return image.channels();
}

cv::Mat& ImageProcessor::getImage()
{
	return image;
}

Exiv2::ExifData& ImageProcessor::getMetadata()
{
	return metadata;
}

cmsHPROFILE& ImageProcessor::getProfile()
{	
	return profile;
}

void ImageProcessor::displayImage()
{
	cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
	cv::imshow("Display Image", image);
	cv::waitKey(0);
}

void ImageProcessor::printMetadata()
{

	char * buf; cmsUInt32Number bufsize;
	if (profile) {
		bufsize = cmsGetProfileInfoASCII(profile, cmsInfoDescription, cmsNoLanguage, cmsNoCountry, NULL, 0);
		if (bufsize) {
			buf = (char *) malloc(bufsize+1);
			cmsGetProfileInfoASCII(profile, cmsInfoDescription, cmsNoLanguage, cmsNoCountry, buf, bufsize);
			buf[bufsize] = '\0';
			printf("ICC Profile Description: %s\n", buf); 
			free(buf);
		}
	}
	else printf("No ICC Profile!!!\n");
/*
	if (!metadata.empty()) {
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
	}
*/
	fflush(stdout);
}


void ImageProcessor::applyBlur()
{
	cv::Mat d = image.clone();
	cv::blur( image, d, cv::Size( 3, 3 ), cv::Point(-1,-1) );
	image = d;
}

void ImageProcessor::applyResize(int width, int height)
{
	unsigned dw = getWidth();
	unsigned dh = getHeight();
	if (height ==  0) height = dh * ((float)width/(float)dw);
	if (width == 0)  width = dw * ((float)height/(float)dh); 
	
	cv::Mat d = image.clone();
	cv::resize( image, d, cv::Size( width, height), 0, 0, cv::INTER_LANCZOS4 );
	image = d;
}



