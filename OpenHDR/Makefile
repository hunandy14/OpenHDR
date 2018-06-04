# cxx
CXX := g++
CXXFLAGS += -O3
CXXFLAGS += -std=c++11
CXXFLAGS += -fopenmp

CXXLIB := 
CXXLIB += 

CXXINC :=
CXXINC +=

CXXFLAGS += $(CXXLIB) $(CXXINC)

# =============================================
# 編譯檔案命令 [tag:cmd]
# =============================================
# 連結所有編譯後的檔案產生可執行檔
main.exe: OpenHDR.o rgbe_lib.o OpenBMP.o OpenHDR_main.o
	$(CXX) *.o -o main.exe $(CXXFLAGS)
#  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
# Main 檔案
OpenHDR_main.o: OpenHDR_main.cpp
	$(CXX) -c OpenHDR_main.cpp $(CXXFLAGS)

OpenHDR.o: OpenHDR.cpp OpenHDR.hpp
	$(CXX) -c OpenHDR.cpp $(CXXFLAGS)
rgbe_lib.o: rgbe_lib/rgbe_lib.cpp rgbe_lib/rgbe_lib.h
	$(CXX) -c rgbe_lib/rgbe_lib.cpp $(CXXFLAGS)
OpenBMP.o: OpenBMP/OpenBMP.cpp OpenBMP/OpenBMP.hpp
	$(CXX) -c OpenBMP/OpenBMP.cpp $(CXXFLAGS)
# =============================================
# 命令列
# =============================================
# make 預設執行
resule: ./main.exe
# 清理所有編譯的檔案
clear:
	rm -f *.o *.exe
# 僅編譯出可執行檔，並清除緩存
run: resule
	./main.exe
rerun: clean resule
	./main.exe
rebuild: clean resule
	./main.exe