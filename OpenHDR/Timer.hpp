/*****************************************************************
Name : Timer.hpp
Date : 2017/12/19
By   : CharlotteHonG
Final: 2017/12/19
*****************************************************************/
#pragma once
#include <string>
#include <ctime>

class Timer {
public:
	Timer(std::string name=""): name(name){
		startTime = clock();
	}
	operator double() {
		return time;
	}
public:
	void start() {
		startTime = clock();
		flag=0;
	}
	void end() {
		finalTime = clock();
		time = (double)(finalTime - startTime)/CLOCKS_PER_SEC;
	}
	void print(std::string name="") {
		if (flag==0) {
			flag = 1;
			end();
		}
		if(name=="") {
			name=this->name;
		}
		if(priSta) {
			std::cout << "#" << name << ", " << " time = " << time << "s" << std::endl;
		}
	}
private:
	std::string name;
	clock_t startTime;
	clock_t finalTime;
	double time;
	bool flag = 0;
public:
	bool priSta = 1;
};