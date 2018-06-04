#pragma once
#include "rgbe_lib/rgbe_lib.h"

#define HDR_dmax 100.0
#define HDR_b 0.85
#define HDR_gama 2.2

//====================================================================================
// ¸ê®Æµ²ºc
struct basic_rgbeData {
	int width;
	int height;
	rgbe_header_info info;
	vector<float> img;
};
int  rgbeData_size(const basic_rgbeData & hdr);
void rgbeData_info(const basic_rgbeData & hdr);
void rgbeData_read(basic_rgbeData & hdr, std::string name);
void rgbeData_writeBMP(const basic_rgbeData & hdr, std::string name);

void rgb2Yxy(const float * src, float * dst, int size);
void Yxz2rgb(const float * src, float * dst, int size);
void globalToneMapping(float * dst, int size, float dmax=HDR_dmax, float b=HDR_b);
void gama_fix(float* dst, int size, float gam=HDR_gama);

void testMapping(std::string name);
//====================================================================================
// OpenHDR
class OpenHDR {
public:
	OpenHDR(string name){
		rgbeData_read(hdr, name);
	}
public:
	void info() {
		rgbeData_info(hdr);
	}
	void bmp(std::string name) {
		rgbeData_writeBMP(hdr, name);
	}
	void mapping(float dmax=HDR_dmax, float b=HDR_b, float gam=HDR_gama) {
		int imgSize = rgbeData_size(hdr);
		vector<float> Yxy(imgSize*3);
		// Mpping
		rgb2Yxy(hdr.img.data(), Yxy.data(), imgSize);
		globalToneMapping(Yxy.data(), imgSize, dmax, b);
		Yxz2rgb(Yxy.data(), hdr.img.data(), imgSize);
		// gama fix
		gama_fix(hdr.img.data(), imgSize, gam);
	}
private:
	basic_rgbeData hdr;
};



//====================================================================================