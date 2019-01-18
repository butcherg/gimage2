#include "ImageProcessor.hpp"

#include <opencv2/imgproc.hpp>

void ImageProcessor::applyBlur()
{
	cv::Mat d = image.clone();
	cv::blur( image, d, cv::Size( 3, 3 ), cv::Point(-1,-1) );
	image = d;
}

void ImageProcessor::applyDemosaic()
{
/*
	cv::Mat H(image.rows/2, image.cols/2, CV_32FC3);
	

	std::vector<unsigned> q = {0, 1, 1, 2};  //default pattern is RGGB, where R=0, G=1, B=2
	//if (imginfo["LibrawCFAPattern"] == "GRBG") q = {1, 0, 2, 1};
	//if (imginfo["LibrawCFAPattern"] == "GBRG") q = {1, 1, 0, 1};
	//if (imginfo["LibrawCFAPattern"] == "BGGR") q = {2, 1, 1, 0};

	//if (algorithm == DEMOSAIC_HALF | algorithm == DEMOSAIC_HALF_RESIZE) {
	//	#pragma omp parallel for num_threads(threadcount)
		for (unsigned y=0; y<h; y+=2) {
			for (unsigned x=0; x<w; x+=2) {
				unsigned Hpos = (x/2) + (y/2)*(w/2);
				float pix[3] = {0.0, 0.0, 0.0};
				unsigned pos[4];
				pos[0] = x + y*w;  //upper left
				pos[1] = (x+1) + y*w; //upper right
				pos[2] = x + (y+1)*w; //lower left
				pos[3] = (x+1) + (y+1)*w;  //lower right
				for (unsigned i=0; i<q.size(); i++) 
					pix[q[i]] += image[pos[i]].r;  //use r, in grayscale, they're all the same...
				pix[1] /= 2.0;
				halfimage[Hpos].r = pix[0];
				halfimage[Hpos].g = pix[1];
				halfimage[Hpos].b = pix[2];
			}
		}
		image = halfimage;
		w /=2;
		h /=2;
		c = 3;
*/
}

void ImageProcessor::applyNormalization()
{
	double min, max;
	cv::Mat d = image.clone();
	cv::normalize(image, d, 0, 1, cv::NORM_MINMAX);
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



