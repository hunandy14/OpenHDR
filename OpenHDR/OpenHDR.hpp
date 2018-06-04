#pragma once
#include "rgbe_lib/rgbe_lib.h"

#define HDR_dmax 100.0
#define HDR_b 0.85
#define HDR_gama 2.2



// ¸ê®Æµ²ºc
struct basic_rgbeData {
	int width;
	int height;
	rgbe_header_info info;
	vector<float> img;
};
int rgbeData_size(const basic_rgbeData & hdr);
void rgbeData_info(const basic_rgbeData & hdr);
void rgbeRead(basic_rgbeData & hdr, string name);

void rgb2Yxy(const float * src, float * dst, int size);
void Yxz2rgb(const float * src, float * dst, int size);

void basic_globalToneMapping(float * dst, int size, float dmax=HDR_dmax, float b=HDR_b);
void gama_fix(vector<float>& RGB_pix, float gam=HDR_gama);

void rgbeBMP(const basic_rgbeData & hdr, string name);
