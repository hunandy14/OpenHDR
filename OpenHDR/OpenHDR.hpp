#pragma once
#include "rgbe_lib/rgbe_lib.h"

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

void Mapping_basic(size_t dim, vector<float>& lumi_map, int rgb_map, vector<float>& lumi, int rgb, float dmax, float b);
void gama_fix(vector<float>& RGB_pix, float gam);

void rgbeBMP(const basic_rgbeData & hdr, string name);
