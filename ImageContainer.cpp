#include "ImageContainer.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <exiv2/error.hpp>

#include <libraw/libraw.h>

#include <iostream>
#include <string>




ImageContainer::ImageContainer()
{
	profile = NULL;
}

ImageContainer::ImageContainer(const ImageContainer &imagecontainer)
{
	image = imagecontainer.image.clone();
	exifdata = Exiv2::ExifData(imagecontainer.exifdata);
	iptcdata = Exiv2::IptcData(imagecontainer.iptcdata);
	xmpdata = Exiv2::XmpData(imagecontainer.xmpdata);
	profile = imagecontainer.profile;  //lcms maintains the profile guts, cmsHPROFILE is a handle to it
}

bool ImageContainer::openFile(std::string filename, std::map<std::string, std::string> params)
{	
	//First, try raw file:
	LibRaw RawProcessor;
	int w, h, c, b; int t;
	cv::Mat r;

	std::string rawmode = params.find("mode") != params.end() ? params["mode"].c_str() : "dcraw"; // mode=dcraw|rawimage, default: dcraw

	RawProcessor.imgdata.params.shot_select = 	params.find("shot_select")    != params.end() ? atoi(params["shot_select"].c_str())	: 0;
	RawProcessor.imgdata.params.use_camera_wb = 	params.find("use_camera_wb")  != params.end() ? atoi(params["use_camera_wb"].c_str())	: 1;
	RawProcessor.imgdata.params.output_color =	params.find("output_color")   != params.end() ? atoi(params["output_color"].c_str())	: 1;  //sRGB
	RawProcessor.imgdata.params.user_qual = 	params.find("user_qual")      != params.end() ? atoi(params["user_qual"].c_str())  	: 3;  //AHD
	RawProcessor.imgdata.params.no_auto_bright = 	params.find("no_auto_bright") != params.end() ? atoi(params["no_auto_bright"].c_str())	: 0;

	RawProcessor.imgdata.params.output_bps = 	params.find("output_bps")     != params.end() ? atoi(params["output_bps"].c_str()) 	: 16;
	
	RawProcessor.imgdata.params.gamm[0] = 		params.find("gamm0")	      != params.end() ? atof(params["gamm0"].c_str())		: 1/2.4; //sRGB
	RawProcessor.imgdata.params.gamm[1] = 		params.find("gamm1")	      != params.end() ? atof(params["gamm1"].c_str())		: 12.92; //sRGB
	
	if (RawProcessor.open_file(filename.c_str())==LIBRAW_SUCCESS) {
		RawProcessor.unpack();

		if (rawmode == "rawimage") {
			r = cv::Mat(RawProcessor.imgdata.sizes.raw_height, RawProcessor.imgdata.sizes.raw_width, CV_16UC1,  RawProcessor.imgdata.rawdata.raw_image);
			image = r.clone();
			image.convertTo(image, CV_32F, 1.0/65536.0);
		}
		else { //dcraw:
			RawProcessor.dcraw_process();
			RawProcessor.get_mem_image_format(&w, &h, &c, &b);
			libraw_processed_image_t *img = RawProcessor.dcraw_make_mem_image();
			if (b==8)  
				r = cv::Mat(img->height, img->width, CV_8UC3,  img->data);
			else if (b==16) 
				r = cv::Mat(img->height, img->width, CV_16UC3,  img->data);
			image = r.clone();
			LibRaw::dcraw_clear_mem(img);
			
			if (b==8)  
				image.convertTo(image, CV_32F, 1.0/256.0);
			else if (b==16) 
				image.convertTo(image, CV_32F, 1.0/65536.0);
		
			cvtColor(image, image, cv::COLOR_RGB2BGR);
		}
		RawProcessor.recycle();
	}
	
	//Then, try the other image formats with opencv:
	else {
		image = cv::imread(filename, cv::IMREAD_COLOR);
		if (image.data == NULL) return false;
		if ((image.depth() == CV_8U) | (image.depth() == CV_8S)) image.convertTo(image, CV_32F,  1.0/256.0);
		if ((image.depth() == CV_16U) | (image.depth() == CV_16S)) image.convertTo(image, CV_32F,  1.0/65536.0);

	}

	//Retrieve metadata:
	Exiv2::LogMsg::setLevel (Exiv2::LogMsg::mute);
	Exiv2::Image::AutoPtr img = Exiv2::ImageFactory::open(filename);
	assert(img.get() != 0);
	img->readMetadata();
	exifdata = img->exifData();
	iptcdata = img->iptcData();
	xmpdata = img->xmpData();

	//Retrieve ICC profile:
	if (img->iccProfileDefined()) {
		Exiv2::DataBuf * prof = img->iccProfile();
		profile = cmsOpenProfileFromMem(prof->pData_, prof->size_);
	}
	else profile = NULL;
	return true;
}


bool ImageContainer::saveFile(std::string filename, std::map<std::string, std::string> params)
{
	bool result;
	std::vector<int> p;
	cv::Mat img = image.clone();
	img.convertTo(img, CV_8U, 256.0);
	result = cv::imwrite(filename, img, p);
	if (!result) return false;

	if (true) {  //reserve conditional for a parameter (excludemetadata?)
		Exiv2::LogMsg::setLevel (Exiv2::LogMsg::mute);
		Exiv2::Image::AutoPtr meta = Exiv2::ImageFactory::open(filename);
		assert(meta.get() != 0);

		if (profile) {
			cmsUInt32Number bufsize;
			meta->clearIccProfile();
			if (cmsSaveProfileToMem(profile, NULL, &bufsize)) {
				Exiv2::DataBuf prof((int) bufsize);
				cmsSaveProfileToMem(profile, prof.pData_, &bufsize);
				meta->setIccProfile(prof);
			}
		}

		if (!exifdata.empty()) meta->setExifData(exifdata);
		if (!iptcdata.empty()) meta->setIptcData(iptcdata);
		if (!xmpdata.empty())  meta->setXmpData(xmpdata);
		meta->writeMetadata();
	}

	return true;
}

bool ImageContainer::isOk()
{
	if (image.data == NULL) return false;
	return true;
}

int ImageContainer::getWidth()
{
	return image.cols;
}

int ImageContainer::getHeight()
{
	return image.rows;
}

int ImageContainer::getColors()
{
	return image.channels();
}

cv::Mat& ImageContainer::getImage()
{
	return image;
}

Exiv2::ExifData& ImageContainer::getEXIFData()
{
	return exifdata;
}

Exiv2::IptcData& ImageContainer::getIPTCData()
{
	return iptcdata;
}

Exiv2::XmpData& ImageContainer::getXMPData()
{
	return xmpdata;
}

cmsHPROFILE& ImageContainer::getProfile()
{	
	return profile;
}

void ImageContainer::displayImage()
{
	cv::Mat draw;
	image.convertTo(draw, CV_8U, 256.0);
	cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
	cv::imshow("Display Image", draw);
	cv::waitKey(0);
}

void ImageContainer::displayImage(std::string displayname)
{
	cv::Mat draw;

	unsigned width = 1000;
	unsigned height = 0;

	unsigned dw = getWidth();
	unsigned dh = getHeight();
	if (height ==  0) height = dh * ((float)width/(float)dw);
	if (width == 0)  width = dw * ((float)height/(float)dh); 

	image.convertTo(draw, CV_8U, 256.0);
	if (image.cols > 1000) cv::resize( draw, draw, cv::Size( width, height), 0, 0, cv::INTER_LANCZOS4 );
	cv::imshow(displayname, draw);
	cv::waitKey(0);
}

void ImageContainer::printMetadata()
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
	//else printf("No ICC Profile!!!\n");

	printf("EXIF:\n");
	if (!exifdata.empty()) {
		Exiv2::ExifData::const_iterator end = exifdata.end();
		for (Exiv2::ExifData::const_iterator i = exifdata.begin(); i != end; ++i) {
			const char* tn = i->typeName();
			std::cout << std::setw(44) << std::setfill(' ') << std::left
				<< i->key() << " "						//key
				<< "0x" << std::setw(4) << std::setfill('0') << std::right
				<< std::hex << i->tag() << " "					//tagID
				<< std::setw(9) << std::setfill(' ') << std::left
				<< (tn ? tn : "Unknown") << " "					//typename
				<< std::dec << std::setw(3)
				<< std::setfill(' ') << std::right
				<< i->count() << "  ";						//number of values
				if (i->count() < 80)
					std::cout << std::dec << i->value();			//value, or (...) if count > 10
				else
					std::cout << "(...)";
				std::cout << "\n";
		}
	}

	printf("\n\nIPTC:\n");
	if (!iptcdata.empty()) {
		Exiv2::IptcData::const_iterator end = iptcdata.end();
		for (Exiv2::IptcData::const_iterator i = iptcdata.begin(); i != end; ++i) {
			const char* tn = i->typeName();
			std::cout << std::setw(44) << std::setfill(' ') << std::left
				<< i->key() << " "						//key
				<< "0x" << std::setw(4) << std::setfill('0') << std::right
				<< std::hex << i->tag() << " "					//tagID
				<< std::setw(9) << std::setfill(' ') << std::left
				<< (tn ? tn : "Unknown") << " "					//typename
				<< std::dec << std::setw(3)
				<< std::setfill(' ') << std::right
				<< i->count() << "  ";						//number of values
				if (i->count() < 80)
					std::cout << std::dec << i->value();			//value, or (...) if count > 10
				else
					std::cout << "(...)";
				std::cout << "\n";
		}
	}

	printf("\n\nXMP:\n");
	if (!xmpdata.empty()) {
		Exiv2::XmpData::const_iterator end = xmpdata.end();
		for (Exiv2::XmpData::const_iterator i = xmpdata.begin(); i != end; ++i) {
			const char* tn = i->typeName();
			std::cout << std::setw(44) << std::setfill(' ') << std::left
				<< i->key() << " "						//key
				<< "0x" << std::setw(4) << std::setfill('0') << std::right
				<< std::hex << i->tag() << " "					//tagID
				<< std::setw(9) << std::setfill(' ') << std::left
				<< (tn ? tn : "Unknown") << " "					//typename
				<< std::dec << std::setw(3)
				<< std::setfill(' ') << std::right
				<< i->count() << "  ";						//number of values
				if (i->count() < 80)
					std::cout << std::dec << i->value();			//value, or (...) if count > 10
				else
					std::cout << "(...)";
				std::cout << "\n";
		}
	}
	fflush(stdout);
}




