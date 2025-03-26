#include "Interface.cpp"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <complex>
#include <algorithm>	
#include <numeric>
#include <Windows.h>
#include <mmsystem.h>
#include <conio.h>
#include <unistd.h>
#include <direct.h>
main(){
	
	const char *dir1Name = "resources/sounds";
	const char *dir2Name = "output";
	
	if (_mkdir(dir1Name) == 0 and _mkdir(dir2Name) == 0){
		std::cout << "Tao thu muc khoi dau thanh cong" << std::endl;
		system("pause");
	} else {
		std::cout << "Thu muc da ton tai hoac co loi xay ra khi thuc thi" << std::endl;
		system("pause");
	}
	
    Interface::mainMenu();
}