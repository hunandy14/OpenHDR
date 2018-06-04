#include <iostream>
using namespace std;

#include "Timer.hpp"
#include "OpenBMP/OpenBMP.hpp"
#include "OpenHDR.hpp"


//====================================================================================
// rgbeData function
//====================================================================================
int rgbeData_size(const basic_rgbeData& hdr) {
	return (hdr.width * hdr.height);
}
void rgbeData_info(const basic_rgbeData& hdr) {
	cout << "width=" << hdr.width << endl;
	cout << "height=" << hdr.height << endl;
}
void rgbeData_read(basic_rgbeData& hdr, string name) {
	FILE* hdrFile;
	fopen_s(&hdrFile, name.c_str(), "rb");

	RGBE_ReadHeader(hdrFile, &hdr.width, &hdr.height, &hdr.info);
	hdr.img.resize(hdr.width*hdr.height*3);
	RGBE_ReadPixels_RLE(hdrFile, hdr.img.data(), hdr.width, hdr.height);

	fclose(hdrFile);
}
void rgbeData_writeBMP(const basic_rgbeData& hdr, string name) {
	bool overfix = 1;
	ImgData LDRimg(hdr.width, hdr.height, 24);
	for (size_t i = 0; i < LDRimg.size(); i++) {
		float p = hdr.img[i]*255.0;
		if (overfix == 1) {
			if (p > 255) {
				p = 255;
			} else if (p < 0) {
				p = 0;
			}
		}
		LDRimg[i] = p;
	}
	LDRimg.bmp(name);
}

//====================================================================================
// ToneMapping function
//====================================================================================
void rgb2Yxy(const float* src, float* dst, int size) {
//#pragma omp parallel for
	for(int i = 0; i < size; ++i) {
		float a, b, c;
		a = (0.412453) * src[i*3 + 0]+
			(0.357580) * src[i*3 + 1]+
			(0.180423) * src[i*3 + 2];

		b = (0.212671) * src[i*3 + 0]+
			(0.715160) * src[i*3 + 1]+
			(0.072169) * src[i*3 + 2];

		c = (0.019334) * src[i*3 + 0]+
			(0.119193) * src[i*3 + 1]+
			(0.950227) * src[i*3 + 2];
		dst[i*3 + 0] = b;
		dst[i*3 + 1] = a / (a+b+c);
		dst[i*3 + 2] = b / (a+b+c);
	}
}
void Yxz2rgb(const float* src, float* dst, int size){
//#pragma omp parallel for
	for(int i = 0; i < size; ++i) {
		float a, b, c, newW;
		newW = src[i*3 + 0] / src[i*3 + 2];
		a = src[i*3 + 0];
		b = newW * src[i*3 + 1];
		c = newW - b - a;

		dst[i*3 + 0]  =  float(3.240479)*b;
		dst[i*3 + 0] += -float(1.537150)*a;
		dst[i*3 + 0] += -float(0.498535)*c;

		dst[i*3 + 1]  = -float(0.969256)*b;
		dst[i*3 + 1] +=  float(1.875992)*a;
		dst[i*3 + 1] +=  float(0.041556)*c;

		dst[i*3 + 2]  =  float(0.055648)*b;
		dst[i*3 + 2] += -float(0.204043)*a;
		dst[i*3 + 2] +=  float(1.057311)*c;
	}
}
void globalToneMapping(float* dst, int size, float dmax, float b)
{
	constexpr int dim = 3; // 幾個通道
	constexpr int rgb = 0; // 選擇哪個通道
	
	float maxLum = dst[rgb];
	for(unsigned i = 1; i < size; ++i) {
		if(dst[i*dim+rgb] > maxLum)
			maxLum = dst[i*dim+rgb];
	}

	float logSum = 0.0;
	for(int i = 0; i < size; ++i) 
		logSum += log(dst[i*dim+rgb]);
	
	const float logAvgLum = logSum/size;
	const float avgLum = exp(logAvgLum);
	const float maxLumW = (maxLum / avgLum);
	const float coeff = (dmax*float(0.01)) / log10(maxLumW+1.0);

#pragma omp parallel for
	for(int i = 0; i < size; ++i) {
		auto& p = dst[i*dim+rgb];
		p /= avgLum;
		p = log(p+1.0) / log(2.0 + pow((p/maxLumW),(log(b)/log(0.5)))*8.0);
		p *= coeff;
	}
}
void gama_fix(float* dst, int size, float gam) {
	const float fgamma = (0.45/gam)*2.0;
	float slope = 4.5;
	float start = 0.018;
	// 判定係數
	if(gam >= float(2.1)) {
		start /= ((gam - 2) * float(7.5));
		slope *= ((gam - 2) * float(7.5));
	} else if(gam <= float(1.9)) {
		start *= ((2 - gam) * float(7.5));
		slope /= ((2 - gam) * float(7.5));
	}
	// 校正像素
#pragma omp parallel for
	for (int i = 0; i < size*3; i++) {
		if(dst[i] <= start) {
			dst[i] = dst[i]*slope;
		} else {
			dst[i] = float(1.099)*pow(dst[i], fgamma) - float(0.099);
		}
	}
}

void testMapping(string name) {
	Timer t;

	// read file
	basic_rgbeData hdr;
	rgbeData_read(hdr, name);
	rgbeData_info(hdr);
	//rgbeData_writeBMP(hdr, "resultIMG\HDR_non.bmp");

	// Mpping
	int imgSize = rgbeData_size(hdr);
	vector<float> Yxy(imgSize*3);

	t.start();
	rgb2Yxy(hdr.img.data(), Yxy.data(), imgSize);
	globalToneMapping(Yxy.data(), imgSize);
	Yxz2rgb(Yxy.data(), hdr.img.data(), imgSize);
	t.print("mapping");
	//rgbeData_writeBMP(hdr, "resultIMG\HDR_mapping.bmp");

	cout << hdr.info.gamma << endl;
	// gama fix
	t.start();
	gama_fix(hdr.img.data(), imgSize, 2.2);
	t.print("gamafix");
	rgbeData_writeBMP(hdr, "resultIMG/HDR_IMG.bmp");
}
//====================================================================================
