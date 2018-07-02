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

const std::string hdrName = "seymour_park.hdr";
//====================================================================================
int main(int argc, char const *argv[]) {
	Timer t;
	OpenHDR hdr(hdrName);
	hdr.info();
	
	t.start();
	hdr.mapping();
	t.print("hdr.mapping();");

	hdr.bmp("resultIMG/HDR_IMG.bmp");
	return 0;
}
//====================================================================================
