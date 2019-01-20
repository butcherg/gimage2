#include "ImageProcessor.hpp"

#include <opencv2/imgproc.hpp>
#include <math.h>

/*
struct pix {
	float b, g, r;
};
*/

union pix {
  struct { float b, g, r; } ;
  float v[3] ;
} ;


void ImageProcessor::applyBlur(unsigned kernelsize)
{
	cv::blur( image, image, cv::Size( kernelsize, kernelsize ), cv::Point(-1,-1) );
}

void ImageProcessor::applyLog()
{
	pix * src = (pix *) image.data;
	double a = 0.17883277, b = 0.28466892, c = 0.55991073;
	double rubicon = 1.0/12.0;
	double sqrt3 = sqrt(3);

	for (unsigned i=0; i< image.rows*image.cols; i++) {
		if (src[i].r > 0.0) if (src[i].r > rubicon) src[i].r = a * log(12*src[i].r - b) + c; else src[i].r = sqrt3 * pow(src[i].r, 0.5); 
		if (src[i].g > 0.0) if (src[i].g > rubicon) src[i].g = a * log(12*src[i].g - b) + c; else src[i].g = sqrt3 * pow(src[i].g, 0.5); 
		if (src[i].b > 0.0) if (src[i].b > rubicon) src[i].b = a * log(12*src[i].b - b) + c; else src[i].b = sqrt3 * pow(src[i].b, 0.5); 
	}
}

void ImageProcessor::applyDemosaic()
{
	//This is the half algorithm from gimage.cpp

	//ToDo: check to make sure data is CV_32FC1, exception?

	unsigned w = image.cols;
	unsigned h = image.rows;

	cv::Mat H(image.rows/2, image.cols/2, CV_32FC3);

	pix * halfimage = (pix *) H.data;  //float RGB destination, half-sized
	float * img = (float *) image.data; //single-channel float 

	std::vector<unsigned> q; // e.g., {0, 1, 1, 2}; Nikon pattern is RGGB, where R=0, G=1, B=2; Exiv2 value is "0 2 0 2 0 1 1 2"

	Exiv2::ExifKey key = Exiv2::ExifKey("Exif.Photo.CFAPattern");
	Exiv2::ExifData::iterator pos = exifdata.findKey(key);
	if (pos != exifdata.end()) {
		Exiv2::Value::AutoPtr v = pos->getValue();
		std::string cfa =  v->toString();
		//printf("CFA: \"%s\", ",cfa.c_str()); fflush(stdout);  //keep for eventual debugging
		if (cfa == "0 2 0 2 0 1 1 2") q = {0, 1, 1, 2}; //RGGB
		else if (cfa == "0 2 0 2 1 0 2 1") q = {1, 0, 2, 1}; //GRBG
		else if (cfa == "0 2 0 2 1 2 0 1") q = {1, 2, 0, 1}; //GBRG
		else if (cfa == "0 2 0 2 2 1 1 0") q = {2, 1, 1, 0}; //BGGR
		else return;  //unsupported CFA pattern
	}

	//taken directly from gimage:
	for (unsigned y=0; y<h-2; y+=2) {
		for (unsigned x=0; x<w-2; x+=2) {
			unsigned Hpos = (x/2) + (y/2)*(w/2);
			float pix[3] = {0.0, 0.0, 0.0};
			unsigned pos[4];
			pos[0] = x + y*w;  //upper left
			pos[1] = (x+1) + y*w; //upper right
			pos[2] = x + (y+1)*w; //lower left
			pos[3] = (x+1) + (y+1)*w;  //lower right
			for (unsigned i=0; i<q.size(); i++) 
				pix[q[i]] += 
					img[pos[i]]; //.r;  //use r, in grayscale, they're all the same...
			pix[1] /= 2.0;
			halfimage[Hpos].r = pix[0];
			halfimage[Hpos].g = pix[1];
			halfimage[Hpos].b = pix[2];
		}
	}
	image = H;
}

void ImageProcessor::applyNormalization()
{
	cv::normalize(image, image, 0, 1, cv::NORM_MINMAX);
}

void ImageProcessor::applyResize(int width, int height)
{
	unsigned dw = getWidth();
	unsigned dh = getHeight();
	if (height ==  0) height = dh * ((float)width/(float)dw);
	if (width == 0)  width = dw * ((float)height/(float)dh); 
	
	cv::resize( image, image, cv::Size( width, height), 0, 0, cv::INTER_LANCZOS4 );
}

void ImageProcessor::applySharpen(float strength)
{
	float k[3][3] =
	{
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0
	};

	//build a kernel corresponding to the specified strength
	float x = -((strength)/4.0);
	k[0][1] = x;
	k[1][0] = x;
	k[1][2] = x;
	k[2][1] = x;
	k[1][1] = strength+1;

	cv::Mat kernel(3, 3, CV_32F, k);
	cv::filter2D(image, image, -1, kernel);
}



