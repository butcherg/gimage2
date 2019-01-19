#include "ImageCommandProcessor.hpp"


/*
void ImageCommandProcessor::applyDemosaic()
{

}


void ImageCommandProcessor::applyNormalization()
{

}
*/

void ImageCommandProcessor::applyBlur(std::string params)
{
	std::map<std::string,std::string> p = parseparams(params);
	unsigned kernelsize = p.find("kernelsize") != p.end() ? atoi(p["kernelsize"].c_str())  : 3;
	ImageProcessor::applyBlur(kernelsize);
}

void ImageCommandProcessor::applyResize(std::string params)
{
	int width=0, height=0, longest;
	std::map<std::string,std::string> p = parseparams(params);
	if (p.find("longest") != p.end()) {
		longest = atoi(p["longest"].c_str());
		if (image.rows > image.cols) 
			height = longest;
		else
			width = longest;
	}
	else {
		width  = p.find("width")  != p.end() ? atoi(p["width"].c_str())  : 0;
		height = p.find("height") != p.end() ? atoi(p["height"].c_str()) : 0;
	}
	ImageProcessor::applyResize(width, height);
}

void ImageCommandProcessor::applySharpen(std::string params)
{
	std::map<std::string,std::string> p = parseparams(params);
	float strength = p.find("strength") != p.end() ? atof(p["strength"].c_str())  : 1.0;
	ImageProcessor::applySharpen(strength);
}


//helper functions:

std::vector<std::string> split(std::string s, std::string delim)
{
	std::vector<std::string> v;
	size_t pos=0;
	size_t start;
	while (pos < s.length()) {
		start = pos;
		pos = s.find(delim,pos);
		if (pos == s.npos) {
			v.push_back(s.substr(start,s.length()-start));
			return v;
		}
		v.push_back(s.substr(start, pos-start));
		pos += delim.length();
	}
	return v;
}

//string of the form "name=val;name=val;..."
std::map<std::string, std::string> parseparams(std::string params)
{
	std::map<std::string, std::string> p;
	std::vector<std::string> l = split(params,";");
	for (std::vector<std::string>::iterator it=l.begin(); it!=l.end(); ++it) {
		std::string name, val;
		std::vector<std::string> nameval = split(*it,"=");
		if (nameval.size() == 2)
			p[nameval[0]] = nameval[1];
		else
			p[nameval[0]] = "1";
	}
	return p;
}
