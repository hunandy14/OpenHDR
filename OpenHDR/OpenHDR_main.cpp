/*****************************************************************
Name : OpenHDR
Date : 2018/06/04
By   : CharlotteHonG
Final: 2018/06/04

rgbe source: http://www.graphics.cornell.edu/~bjw/rgbe/
*****************************************************************/
#include <iostream>
using namespace std;

#include "OpenBMP/OpenBMP.hpp"
#include "OpenHDR.hpp"
#include "rgbe_lib/rgbe_lib.h"

struct basic_rgbeData {
	int width;
	int height;
	rgbe_header_info info;
	vector<float> img;
};

int rgbeData_size(basic_rgbeData& hdr) {
	return hdr.width*hdr.height;
}
void rgbeData_info(basic_rgbeData& hdr) {
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
// RGB Âà Yxz
void rgb2Yxy(float* Yxy_pix, float* RGB_pix, int size) {
	for(unsigned i = 0; i < size; ++i) {
		float a, b, c;
		a = (0.412453) * RGB_pix[i*3 + 0]+
			(0.357580) * RGB_pix[i*3 + 1]+
			(0.180423) * RGB_pix[i*3 + 2];

		b = (0.212671) * RGB_pix[i*3 + 0]+
			(0.715160) * RGB_pix[i*3 + 1]+
			(0.072169) * RGB_pix[i*3 + 2];

		c = (0.019334) * RGB_pix[i*3 + 0]+
			(0.119193) * RGB_pix[i*3 + 1]+
			(0.950227) * RGB_pix[i*3 + 2];
		Yxy_pix[i*3 + 0] = b;
		Yxy_pix[i*3 + 1] = a / (a+b+c);
		Yxy_pix[i*3 + 2] = b / (a+b+c);
	}
}


//====================================================================================
int main(int argc, char const *argv[]) {
	string hdrName = "seymour_park.hdr";

	// ÅªÀÉ
	basic_rgbeData hdr;
	rgbeRead(hdr, hdrName);
	rgbeData_info(hdr);

	ImgData LDRimg(hdr.width, hdr.height, 24);
	for (size_t i = 0; i < LDRimg.size(); i++) {
		LDRimg[i] = (unsigned char)(hdr.img[i]*255.0);
	}
	LDRimg.bmp("HDR_non.bmp");

	return 0;
}
//====================================================================================
