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
	
	const char *dir1Name = "resources";
	const char *dir2Name = "output";
	
	if (mkdir(dir1Name) == 0){
		std::cout << "Tao thu muc resources thanh cong" << std::endl;
		system("pause");
	} else {
		std::cout << "Thu muc resources da ton tai hoac co loi xay ra khi thuc thi" << std::endl;
		system("pause");
	}
	
	const char *dir3Name = "resources/sounds";
	if (mkdir(dir3Name) == 0){
		std::cout << "Tao thu muc resources/sounds thanh cong" << std::endl;
		system("pause");
	} else {
		std::cout << "Thu muc resources/sounds da ton tai hoac co loi xay ra khi thuc thi" << std::endl;
		system("pause");
	}
	
	if (mkdir(dir2Name) == 0){
		std::cout << "Tao thu muc output thanh cong" << std::endl;
		system("pause");
	} else {
		std::cout << "Thu muc output da ton tai hoac co loi xay ra khi thuc thi" << std::endl;
		system("pause");
	}
	
    Interface::mainMenu();
}