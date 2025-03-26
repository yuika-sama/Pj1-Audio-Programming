#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#pragma pack(push, 1)
struct WAVHeader {
    char riff_header[4];  
    uint32_t wav_size;     
    char wave_header[4];    
    char fmt_header[4];    
    uint32_t fmt_chunk_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char data_header[4];   
    uint32_t data_bytes;
};
#pragma pack(pop)

void printWAVHeader(const WAVHeader& header) {
	std::cout << std::endl;
    std::cout << "Thong tin header WAV:" << std::endl;
    std::cout << "RIFF header: " << std::string(header.riff_header, 4) << std::endl;
    std::cout << "WAV size: " << header.wav_size << std::endl;
    std::cout << "WAVE header: " << std::string(header.wave_header, 4) << std::endl;
    std::cout << "FMT header: " << std::string(header.fmt_header, 4) << std::endl;
    std::cout << "FMT chunk size: " << header.fmt_chunk_size << std::endl;
    std::cout << "Audio format: " << header.audio_format << std::endl;
    std::cout << "Number of channels: " << header.num_channels << std::endl;
    std::cout << "Sample rate: " << header.sample_rate << std::endl;
    std::cout << "Byte rate: " << header.byte_rate << std::endl;
    std::cout << "Block align: " << header.block_align << std::endl;
    std::cout << "Bits per sample: " << header.bits_per_sample << std::endl;
    std::cout << "Data header: " << std::string(header.data_header, 4) << std::endl;
    std::cout << "Data bytes: " << header.data_bytes << std::endl;
    std::cout << std::endl;
}

std::vector<double> readWAVFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Loi: khong the mo file tai: " << filename << std::endl;
        return {};
    }

    WAVHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
    
    if (std::string(header.riff_header, 4) != "RIFF" ||
        std::string(header.wave_header, 4) != "WAVE" ||
        std::string(header.fmt_header, 4) != "fmt " ||
        std::string(header.data_header, 4) != "data") {
        throw std::runtime_error("Dinh dang WAV khong xac dinh");
    }
    
    printWAVHeader(header);
    
    std::vector<double> audio_data(header.data_bytes / (header.bits_per_sample / 8));
    
    if (header.bits_per_sample == 16) {
        std::vector<int16_t> raw_data(header.data_bytes / 2);
        file.read(reinterpret_cast<char*>(raw_data.data()), header.data_bytes);
        for (size_t i = 0; i < raw_data.size(); ++i) {
            audio_data[i] = raw_data[i] / 32768.0f;
        }
    } 
    else if (header.bits_per_sample == 8) {
        std::vector<uint8_t> raw_data(header.data_bytes);
        file.read(reinterpret_cast<char*>(raw_data.data()), header.data_bytes);
        for (size_t i = 0; i < raw_data.size(); ++i) {
            audio_data[i] = (raw_data[i] - 128) / 128.0f;
        }
    } 
    else if (header.bits_per_sample == 24) {
        std::vector<char> raw_data(header.data_bytes);
        file.read(raw_data.data(), header.data_bytes);
        for (size_t i = 0; i < header.data_bytes / 3; ++i) {
            int32_t sample = (raw_data[i*3+2] << 16) | (raw_data[i*3+1] << 8) | raw_data[i*3];
            if (sample & 0x800000) {
                sample |= 0xFF000000;
            }
            audio_data[i] = sample / 8388608.0f;
        }
    }
    
    file.close();
    return audio_data;
}

void writeToFile(const std::vector<double>& audio_data, const std::string& output_filename) {
    std::ofstream outputFile(output_filename);
    if (!outputFile.is_open()) {
        std::cerr << "Loi: khong the mo output file: " << output_filename << std::endl;
        return;
    }

    for (const auto& sample : audio_data) {
        outputFile << sample << '\n';
    }

    outputFile.close();
    std::cout << "Ghi thanh cong " << audio_data.size() << " samples vao " << output_filename << std::endl;
}

int main() {
    std::string inputFile;
    std::string outputFile;
    std::cout << "Nhap ten file input(khong can nhap phan mo rong, ten file khong co dau cach): ";
    std::cin >> inputFile;
    inputFile = "resources/sounds/" + inputFile + ".wav";
    std::cout << "Nhap ten file output(khong can nhap phan mo rong, ten file khong co dau cach): ";
    std::cin >> outputFile;
    outputFile = "output/" + outputFile +  ".txt";

    std::vector<double> audio_data = readWAVFile(inputFile);
    
    if (!audio_data.empty()) {
        writeToFile(audio_data, outputFile);
    }

    return 0;
}
