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
	testMapping(hdrName);
	return 0;
}
//====================================================================================
