#include <iostream>
using namespace std;

#include "OpenBMP/OpenBMP.hpp"
#include "OpenHDR.hpp"


//====================================================================================
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
// RGB Yxz ��m�ҫ��ഫ
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
// �����M�g
void Mapping_basic(size_t dim, vector<float>& lumi_map, int rgb_map,
	vector<float>& lumi, int rgb, float dmax, float b)
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

#pragma omp parallel for
	for(int i = 0; i < N; ++i) {
		lumi_map[i*dim+rgb] /= avgLum;
		lumi_map[i*dim+rgb] = log(lumi[i*dim+rgb]+1) /
			log(2 + pow((lumi[i*dim+rgb]/maxLumW),(log(b)/log(0.5)))*8);
		lumi_map[i*dim+rgb] *= coeff;
	}
}
// gama�ե�
void gama_fix(vector<float>& RGB_pix, float gam) {
	float slope = 4.5;
	float start = 0.018;
	float fgamma = (0.45/gam)*2;
	// �P�w�Y��
	if(gam >= float(2.1)) {
		start /= ((gam - 2) * float(7.5));
		slope *= ((gam - 2) * float(7.5));
	} else if(gam <= float(1.9)) {
		start *= ((2 - gam) * float(7.5));
		slope /= ((2 - gam) * float(7.5));
	}
	// �ե�����
#pragma omp parallel for
	for (int i = 0; i < RGB_pix.size(); i++) {
		if(RGB_pix[i] <= start) {
			RGB_pix[i] = RGB_pix[i]*slope;
		} else {
			RGB_pix[i] = float(1.099)*pow(RGB_pix[i], fgamma) - float(0.099);
		}
	}
}
// ��X�Ϥ�
void rgbeBMP(const basic_rgbeData& hdr, string name) {
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