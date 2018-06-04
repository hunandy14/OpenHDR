/*****************************************************************
Name : OpenHDR
Date : 2018/06/04
By   : CharlotteHonG
Final: 2018/06/04

rgbe source: http://www.graphics.cornell.edu/~bjw/rgbe/
*****************************************************************/
#include <iostream>
#include <Timer.hpp>
using namespace std;

#include "OpenBMP/OpenBMP.hpp"
#include "OpenHDR.hpp"
#include "rgbe_lib/rgbe_lib.h"


//====================================================================================
// 資料結構
struct basic_rgbeData {
	int width;
	int height;
	rgbe_header_info info;
	vector<float> img;
};
int rgbeData_size(const basic_rgbeData& hdr) {
	return hdr.width * hdr.height;
}
void rgbeData_info(const basic_rgbeData& hdr) {
	cout << "width=" << hdr.width << endl;
	cout << "height=" << hdr.height << endl;
}
void rgbeRead(basic_rgbeData& hdr, string name) {
	FILE* hdrFile;
	fopen_s(&hdrFile, name.c_str(), "rb");

	RGBE_ReadHeader(hdrFile, &hdr.width, &hdr.height, &hdr.info);
	hdr.img.resize(hdr.width*hdr.height*3);
	RGBE_ReadPixels_RLE(hdrFile, hdr.img.data(), hdr.width, hdr.height);

	fclose(hdrFile);
}


//====================================================================================
// RGB Yxz 色彩模型轉換
void rgb2Yxy(const float* src, float* dst, int size) {
	for(unsigned i = 0; i < size; ++i) {
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
	for(unsigned i = 0; i < size; ++i) {
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
// 全局映射
void Mapping_basic(size_t dim, vector<float>& lumi_map, int rgb_map,
	vector<float>& lumi, int rgb, 
	float dmax, float b)
{
	float maxLum = lumi[0*dim+rgb];
	for(unsigned i = 1; i < lumi.size()/dim; ++i) {
		if(lumi[i*dim+rgb] > maxLum)
			maxLum = lumi[i*dim+rgb];
	}
	size_t N = lumi.size()/dim;
	float logSum = 0;
	for(unsigned i = 0; i < N; ++i)
		logSum += log(lumi[i*dim+rgb]);
	float logAvgLum = logSum/N;
	float avgLum = exp(logAvgLum);
	float maxLumW = (maxLum / avgLum);
	float coeff = (dmax*float(0.01)) / log10(maxLumW+1);
	for(unsigned i = 0; i < N; ++i) {
		lumi_map[i*dim+rgb] /= avgLum;
		lumi_map[i*dim+rgb] = log(lumi[i*dim+rgb]+1) /
			log(2 + pow((lumi[i*dim+rgb]/maxLumW),(log(b)/log(0.5)))*8);
		lumi_map[i*dim+rgb] *= coeff;
	}
}
// gama校正
void gama_fix(vector<float>& RGB_pix, float gam) {
	float slope = 4.5;
	float start = 0.018;
	float fgamma = (0.45/gam)*2;
	// 判定係數
	if(gam >= float(2.1)) {
		start /= ((gam - 2) * float(7.5));
		slope *= ((gam - 2) * float(7.5));
	} else if(gam <= float(1.9)) {
		start *= ((2 - gam) * float(7.5));
		slope /= ((2 - gam) * float(7.5));
	}
	// 校正像素
	for(auto&& i : RGB_pix) {
		if(i <= start) {
			i = i*slope;
		} else {
			i = float(1.099)*pow(i, fgamma) - float(0.099);
		}
	}
}
// 輸出圖片
void rgbeBMP(basic_rgbeData hdr, string name, bool overfix=1) {
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
int main(int argc, char const *argv[]) {
	Timer t;
	string hdrName = "seymour_park.hdr";

	// 讀檔
	basic_rgbeData hdr;
	rgbeRead(hdr, hdrName);
	rgbeData_info(hdr);

	rgbeBMP(hdr, "HDR_non.bmp");

	// Mpping
	int imgSize = rgbeData_size(hdr);
	vector<float> Yxy(imgSize*3);
	t.start();

	rgb2Yxy(hdr.img.data(), Yxy.data(), imgSize);
	Mapping_basic(3, Yxy, 0, Yxy, 0, 100, 0.85);
	Yxz2rgb(Yxy.data(), hdr.img.data(), imgSize);
	
	t.print("mapping");
	rgbeBMP(hdr, "HDR_mapping.bmp");

	// gama fix
	gama_fix(hdr.img, 2.2);
	rgbeBMP(hdr, "HDR_mapping+gamafix.bmp");
	return 0;
}
//====================================================================================
