#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

struct MP3FrameHeader {
    int version;
    int layer;
    bool protection;
    int bitrate_index;
    int sampling_rate_index;
    bool padding;
    bool private_bit;
    int channel_mode;
    int mode_extension;
    bool copyright;
    bool original;
    int emphasis;
};

const int bitrate_table[16] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0};

const int sample_rate_table[4] = {44100, 48000, 32000, 0};

bool parseMP3FrameHeader(const std::vector<uint8_t>& data, size_t offset, MP3FrameHeader& header) {
    if (offset + 4 > data.size()) {
        return false;
    }
    
    if ((data[offset] != 0xFF) || ((data[offset+1] & 0xE0) != 0xE0)) {
        return false;
    }
    
    header.version = (data[offset+1] & 0x18) >> 3;
    header.layer = (data[offset+1] & 0x06) >> 1;
    header.protection = !(data[offset+1] & 0x01);
    header.bitrate_index = (data[offset+2] & 0xF0) >> 4;
    header.sampling_rate_index = (data[offset+2] & 0x0C) >> 2;
    header.padding = (data[offset+2] & 0x02) >> 1;
    header.private_bit = data[offset+2] & 0x01;
    header.channel_mode = (data[offset+3] & 0xC0) >> 6;
    header.mode_extension = (data[offset+3] & 0x30) >> 4;
    header.copyright = (data[offset+3] & 0x08) >> 3;
    header.original = (data[offset+3] & 0x04) >> 2;
    header.emphasis = data[offset+3] & 0x03;
    
    return true;
}

void analyzeMP3File(const std::string& filename, const std::string& outputTxt) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return;
    }
    
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> fileData(fileSize);
    file.read(reinterpret_cast<char*>(fileData.data()), fileSize);
    file.close();
    
    size_t offset = 0;
    MP3FrameHeader header;
    bool foundFrame = false;
    
    while (offset < fileSize - 4) {
        if (parseMP3FrameHeader(fileData, offset, header)) {
            foundFrame = true;
            break;
        }
        offset++;
    }
    
    if (!foundFrame) {
        std::cerr << "Could not find any valid MP3 frame" << std::endl;
        return;
    }
    
    std::cout << "Found MP3 frame at offset: " << offset << std::endl;
    std::cout << "MPEG Version: " << (header.version == 3 ? "1" : (header.version == 2 ? "2" : "2.5")) << std::endl;
    std::cout << "Layer: " << (4 - header.layer) << std::endl;
    
    if (header.version == 3 && header.layer == 1) { 
        std::cout << "Bitrate: " << bitrate_table[header.bitrate_index] << " kbps" << std::endl;
        std::cout << "Sample Rate: " << sample_rate_table[header.sampling_rate_index] << " Hz" << std::endl;
    } else {
        std::cout << "Bitrate index: " << header.bitrate_index << std::endl;
        std::cout << "Sample rate index: " << header.sampling_rate_index << std::endl;
    }
    
    std::cout << "Channel Mode: ";
    switch(header.channel_mode) {
        case 0: std::cout << "Stereo"; break;
        case 1: std::cout << "Joint Stereo"; break;
        case 2: std::cout << "Dual Channel"; break;
        case 3: std::cout << "Mono"; break;
    }
    std::cout << std::endl;

    std::ofstream outFile(outputTxt);
    if (!outFile.is_open()) {
        std::cerr << "Could not open output file: " << outputTxt << std::endl;
        return;
    }

    for (uint8_t byte : fileData) {
        double normalizedValue = byte / 255.0;
        outFile << normalizedValue << "\n";
    }

    outFile.close();
    std::cout << "MP3 data successfully written to " << outputTxt << std::endl;
}

int main() {
    std::string mp3File = "resources/sounds/guitar.mp3"; 
    std::string txtFile = "outputmp3.txt";

    analyzeMP3File(mp3File, txtFile);
    return 0;
}
