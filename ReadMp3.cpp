#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <stdexcept>

// Các cấu trúc dữ liệu
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

// Các bảng tra cứu
const int bitrate_table[16] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0};
const int sample_rate_table[4] = {44100, 48000, 32000, 0};

// Các hàm xử lý
bool parseFrameHeader(const std::vector<uint8_t>& data, size_t offset, MP3FrameHeader& header) {
    if (offset + 4 > data.size()) return false;
    if ((data[offset] != 0xFF) || ((data[offset + 1] & 0xE0) != 0xE0)) return false;

    header.version = (data[offset + 1] & 0x18) >> 3;
    header.layer = (data[offset + 1] & 0x06) >> 1;
    header.protection = !(data[offset + 1] & 0x01);
    header.bitrate_index = (data[offset + 2] & 0xF0) >> 4;
    header.sampling_rate_index = (data[offset + 2] & 0x0C) >> 2;
    header.padding = (data[offset + 2] & 0x02) >> 1;
    header.private_bit = data[offset + 2] & 0x01;
    header.channel_mode = (data[offset + 3] & 0xC0) >> 6;
    header.mode_extension = (data[offset + 3] & 0x30) >> 4;
    header.copyright = (data[offset + 3] & 0x08) >> 3;
    header.original = (data[offset + 3] & 0x04) >> 2;
    header.emphasis = data[offset + 3] & 0x03;

    return true;
}

std::vector<double> decodeMP3Frame(const std::vector<uint8_t>& data, size_t offset, int frameSize) {
    std::vector<double> decodedSamples;
    for (int i = 4; i < frameSize; i++) {
        double sample = (static_cast<double>(data[offset + i]) - 128.0) / 128.0;
        decodedSamples.push_back(sample);
    }
    return decodedSamples;
}

int calculateFrameSize(const MP3FrameHeader& header) {
    int bitrate = bitrate_table[header.bitrate_index] * 1000;
    int sampleRate = sample_rate_table[header.sampling_rate_index];
    int padding = header.padding ? 1 : 0;
    return ((144 * bitrate) / sampleRate) + padding;
}

void printMP3Header(const MP3FrameHeader& header) {
	std::cout << std::endl;
    std::cout << "Thong tin header MP3:" << std::endl;
    std::cout << "Version: " << header.version << std::endl;
    std::cout << "Layer: " << header.layer << std::endl;
    std::cout << "Protection: " << (header.protection ? "Yes" : "No") << std::endl;
    std::cout << "Bitrate: " << bitrate_table[header.bitrate_index] << " kbps" << std::endl;
    std::cout << "Sampling rate: " << sample_rate_table[header.sampling_rate_index] << " Hz" << std::endl;
    std::cout << "Padding: " << (header.padding ? "Yes" : "No") << std::endl;
    std::cout << "Private bit: " << (header.private_bit ? "Yes" : "No") << std::endl;
    std::cout << "Channel mode: " << header.channel_mode << std::endl;
    std::cout << "Mode extension: " << header.mode_extension << std::endl;
    std::cout << "Copyright: " << (header.copyright ? "Yes" : "No") << std::endl;
    std::cout << "Original: " << (header.original ? "Yes" : "No") << std::endl;
    std::cout << "Emphasis: " << header.emphasis << std::endl;
    std::cout << std::endl;
}

std::vector<double> readMP3File(const std::string& filename, double& totalDuration) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Khong the mo file input: " + filename);
    }

    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> fileData(fileSize);
    file.read(reinterpret_cast<char*>(fileData.data()), fileSize);
    file.close();

    std::vector<double> samples;
    size_t offset = 0;
    totalDuration = 0.0;
    MP3FrameHeader header;
    bool headerPrinted = false;

    while (offset < fileSize - 4) {
        if (parseFrameHeader(fileData, offset, header)) {
        	if (!headerPrinted) {
        		headerPrinted = !headerPrinted;
        		printMP3Header(header);
			}
            if (header.version == 3 && header.layer == 1) { // MPEG Version 1, Layer 3
                int frameSize = calculateFrameSize(header);
                double frameDuration = 1152.0 / sample_rate_table[header.sampling_rate_index];
                totalDuration += frameDuration;

                auto decodedSamples = decodeMP3Frame(fileData, offset, frameSize);
                samples.insert(samples.end(), decodedSamples.begin(), decodedSamples.end());

                offset += frameSize;
            } else {
                offset++;
            }
        } else {
            offset++;
        }
    }

    return samples;
}

void writeMP3DataToFile(const std::string& outputTxt, const std::vector<double>& samples) {
    std::string outputPath = "output/" + outputTxt;
    std::ofstream outFile(outputPath);
    if (!outFile.is_open()) {
        throw std::runtime_error("Khong the mo file output: " + outputTxt);
    }

    for (double sample : samples) {
        outFile << sample << " ";
    }

    outFile.close();
    std::cout << "Ghi file hoan tat: " << outputTxt << std::endl;
}

int main() {
    try {
        std::string inputFile;
        std::string outputFile;
        std::cout << "Nhap ten file input(khong can nhap phan mo rong, ten file khong co dau cach): ";
        std::cin >> inputFile;
        inputFile = "resources/sounds/" + inputFile + ".mp3";
        std::cout << "Nhap ten file output(khong can nhap phan mo rong, ten file khong co dau cach): ";
        std::cin >> outputFile;
        outputFile += ".txt";
		double duration;

        std::vector<double> samples = readMP3File(inputFile, duration);
        std::cout << "Thoi luong: " << duration << " giay" << std::endl;

        writeMP3DataToFile(outputFile, samples);
    } catch (const std::exception& e) {
        std::cerr << "Loi: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
