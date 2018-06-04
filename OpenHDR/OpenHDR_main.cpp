/*****************************************************************
Name : OpenHDR
Date : 2018/06/04
By   : CharlotteHonG
Final: 2018/06/04

rgbe source: http://www.graphics.cornell.edu/~bjw/rgbe/
*****************************************************************/
#include <iostream>
#include <vector>
using namespace std;

#include "Timer.hpp"
#include "OpenHDR.hpp"

//====================================================================================
int main(int argc, char const *argv[]) {
	Timer t;
	string hdrName = "seymour_park.hdr";

	// ÅªÀÉ
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
	t.start();
	gama_fix(hdr.img, 2.2);
	t.print("gamafix");
	rgbeBMP(hdr, "HDR_mapping+gamafix.bmp");

	return 0;
}
//====================================================================================
