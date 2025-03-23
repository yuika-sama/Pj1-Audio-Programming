#include <Windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <conio.h>

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "conio.lib")

bool isPlaying = false;
bool isPaused = false;

void playSound(const std::string& filePath) {
    if (filePath.find(".wav") != std::string::npos || filePath.find(".mp3") != std::string::npos) {
        if (mciSendString(("open \"" + filePath + "\" type mpegvideo alias sound").c_str(), NULL, 0, NULL) == 0) {
            if (mciSendString("play sound", NULL, 0, NULL) == 0) {
                std::cout << "Playing sound..." << std::flush;
                isPlaying = true;
            } else {
                std::cout << "Error playing sound." << std::endl;
            }
        } else {
            std::cout << "Error opening sound file." << std::endl;
        }
    } else {
        std::cout << "Only .wav and .mp3 files are supported." << std::endl;
    }
}

void pauseSound() {
    if (isPlaying && !isPaused) {
        if (mciSendString("pause sound", NULL, 0, NULL) == 0) {
            std::cout << "\rSound paused..." << std::flush;
            isPaused = true;
        } else {
            std::cout << "\rError pausing sound." << std::endl;
        }
    } else if (isPlaying && isPaused) {
        if (mciSendString("resume sound", NULL, 0, NULL) == 0) {
            std::cout << "\rPlaying sound..." << std::flush;
            isPaused = false;
        } else {
            std::cout << "\rError resuming sound." << std::endl;
        }
    }
}

void checkSoundStatus() {
    char status[128] = {0};
    mciSendString("status sound mode", status, 128, NULL);
    if (strcmp(status, "stopped") == 0) {
        std::cout << "\rSound has finished playing." << std::endl;
        isPlaying = false;
        mciSendString("close sound", NULL, 0, NULL);
        exit(0);
    }
}

int main() {
    std::string filePath = "resources/sounds/guitar.mp3";

    playSound(filePath);

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
