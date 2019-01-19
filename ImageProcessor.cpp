#include "ImageProcessor.hpp"

#include <opencv2/imgproc.hpp>

struct pix {
	float r, g, b;
};

void ImageProcessor::applyBlur(unsigned kernelsize)
{
	cv::blur( image, image, cv::Size( kernelsize, kernelsize ), cv::Point(-1,-1) );
}

void ImageProcessor::applyLog()
{
	cv::log( image, image);
}

void ImageProcessor::applyDemosaic()
{
	unsigned w = image.cols;  //image.step?
	unsigned h = image.rows;

	cv::Mat H(image.rows/2, image.cols/2, CV_32FC3);

	pix * halfimage = (pix *) H.data;
	pix * img = (pix *) image.data;

	
	

	std::vector<unsigned> q = {0, 1, 1, 2};  //default pattern is RGGB, where R=0, G=1, B=2
	//if (imginfo["LibrawCFAPattern"] == "GRBG") q = {1, 0, 2, 1};
	//if (imginfo["LibrawCFAPattern"] == "GBRG") q = {1, 1, 0, 1};
	//if (imginfo["LibrawCFAPattern"] == "BGGR") q = {2, 1, 1, 0};


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
				pix[q[i]] += img[pos[i]].r;  //use r, in grayscale, they're all the same...
			pix[1] /= 2.0;
			halfimage[Hpos].r = pix[0];
			halfimage[Hpos].g = pix[1];
			halfimage[Hpos].b = pix[2];
		}
	}
	image = H;

/* //candidate code for image walking (https://jayrambhia.wordpress.com/2012/06/23/get-data-from-mat-cvmat-in-opencv/):
	unsigned char *input = (unsigned char*)(img.data);

	int i,j,r,g,b;
	for(int i = 0;i < img.rows ;i++){
		for(int j = 0;j < img.cols ;j++){
			b = img.step * j + i;
			g = img.step * j + i + 1;  //or, b+1;
			r = img.step * j + i + 2;  //or, g+1;

			input[r] = //some processing... ;
			input[g] = //some processing... ;
			input[b] = //some processing... ;
		}
	}

*/
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



