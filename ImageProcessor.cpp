#include "ImageProcessor.hpp"

#include <opencv2/imgproc.hpp>

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



