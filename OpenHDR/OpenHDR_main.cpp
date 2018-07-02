/*****************************************************************
Name : OpenHDR
Date : 2018/06/04
By   : CharlotteHonG
Final: 2018/07/02

site : https://charlottehong.blogspot.com/2018/06/hdr-tonemapping.html
*****************************************************************/
#include <iostream>
#include <vector>
using namespace std;

#include "OpenHDR.hpp"

const std::string hdrName = "seymour_park.hdr";
//====================================================================================
int main(int argc, char const *argv[]) {
	OpenHDR hdr(hdrName);
	hdr.info();
	hdr.mapping();
	hdr.bmp("resultIMG/HDR_IMG.bmp");
	return 0;
}
//====================================================================================
