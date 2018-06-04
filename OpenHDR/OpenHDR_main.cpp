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

	// read file
	basic_rgbeData hdr;
	rgbe_read(hdr, hdrName);
	rgbeData_info(hdr);
	rgbe_writeBMP(hdr, "HDR_non.bmp");

	// Mpping
	int imgSize = rgbeData_size(hdr);
	vector<float> Yxy(imgSize*3);

	t.start();
	rgb2Yxy(hdr.img.data(), Yxy.data(), imgSize);
	globalToneMapping(Yxy.data(), imgSize);
	Yxz2rgb(Yxy.data(), hdr.img.data(), imgSize);
	t.print("mapping");
	rgbe_writeBMP(hdr, "HDR_mapping.bmp");

	// gama fix
	t.start();
	gama_fix(hdr.img.data(), imgSize);
	t.print("gamafix");
	rgbe_writeBMP(hdr, "HDR_mapping+gamafix.bmp");

	return 0;
}
//====================================================================================
