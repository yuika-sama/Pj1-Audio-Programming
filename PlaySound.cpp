#include <Windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <conio.h>

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "conio.lib")

bool isPlaying = false;
bool isPaused = false;
std::string filePath = "resources/sounds/";
void playSound(const std::string& filePath) {
    if (filePath.find(".wav") != std::string::npos || filePath.find(".mp3") != std::string::npos) {
        if (mciSendString(("open \"" + filePath + "\" type mpegvideo alias sound").c_str(), NULL, 0, NULL) == 0) {
            if (mciSendString("play sound", NULL, 0, NULL) == 0) {
                std::cout << "Dang phat nhac voi file " << filePath << "..." << std::flush << std::endl;
                isPlaying = true;
            } else {
                std::cout << "Loi khi phat nhac." << std::endl;
            }
        } else {
            std::cout << "Loi khi mo file am thanh." << std::endl;
        }
    } else {
        std::cout << "Chi ho tro file .wav hoac .mp3." << std::endl;
    }
}

void pauseSound() {
    if (isPlaying && !isPaused) {
        if (mciSendString("pause sound", NULL, 0, NULL) == 0) {
            std::cout << "\rTam dung..." << std::flush << std::endl;
            isPaused = true;
        } else {
            std::cout << "\rLoi khi tam dung phat." << std::endl;
        }
    } else if (isPlaying && isPaused) {
        if (mciSendString("resume sound", NULL, 0, NULL) == 0) {
            std::cout << "Dang phat nhac voi file " << filePath << "..." << std::flush << std::endl;
            isPaused = false;
        } else {
            std::cout << "\rLoi khi tiep tuc phat nhac." << std::endl;
        }
    }
}

void checkSoundStatus() {
    char status[128] = {0};
    mciSendString("status sound mode", status, 128, NULL);
    if (strcmp(status, "stopped") == 0) {
        std::cout << "\rPhat nhac da xong." << std::endl;
        isPlaying = false;
        mciSendString("close sound", NULL, 0, NULL);
        exit(0);
    }
}

int main() {
	std::string filename;
	std::cout << "Nhap ten file(chua day du ca ten va phan mo rong, khong co dau cach): ";
	std::cin >> filename;
	filePath += filename;
    playSound(filePath);
    std::cout << "Nhan dau |Space - dau cach| de tam dung/tiep tuc phat" << std::endl;
    while (true) {
        if (_kbhit()) {
            if (_getch() == 32) {
                pauseSound();
            }
        }
        checkSoundStatus();
        Sleep(100);
    }

    return 0;
}