#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <complex>
#include <algorithm>
#include <numeric>

using namespace std;

class AudioSignal {
private:
    std::vector<double> samples;
    unsigned int sampleRate;
    unsigned int bitDepth;
    unsigned int channels;

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

    const int bitrate_table[16] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0};
    const int sample_rate_table[4] = {44100, 48000, 32000, 0};

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

public:
    // Constructor
    AudioSignal(unsigned int sampleRate, unsigned int bitDepth, unsigned int channels)
        : sampleRate(sampleRate), bitDepth(bitDepth), channels(channels) {}

    // Getter/Setter
    unsigned int getSampleRate() { return sampleRate; }
    void setSampleRate(unsigned int sampleRate) { this->sampleRate = sampleRate; }

    unsigned int getBitDepth() { return bitDepth; }
    void setBitDepth(unsigned int bitDepth) { this->bitDepth = bitDepth; }

    unsigned int getChannels() { return channels; }
    void setChannels(unsigned int channels) { this->channels = channels; }

    std::vector<double> getSamples() { return samples; }
    void setSamples(std::vector<double> samples) { this->samples = samples; }

    // Thêm mẫu vào tín hiệu
    void addSample(double sample) {
        samples.push_back(sample);
    }
    
    unsigned int gcd(unsigned int a, unsigned int b) {
	    while (b != 0) {
	        unsigned int temp = b;
	        b = a % b;
	        a = temp;
	    }
	    return a;
	}
    
    unsigned int lcm(unsigned int a, unsigned int b) {
		return (a * b) / gcd(a, b);
	}

    // Cộng hai tín hiệu
    AudioSignal operator+(AudioSignal &other)
    {
        if (sampleRate != other.sampleRate)
        {
            // Xác định tần số chung
            unsigned int commonRate = lcm(sampleRate, other.sampleRate);
            // Điều chỉnh cả hai tín hiệu về cùng tần số
            AudioSignal resampledThis = this->upSampling(commonRate / sampleRate);
            AudioSignal resampledOther = other.upSampling(commonRate / other.sampleRate);
            unsigned long long newSize = std::max(resampledThis.samples.size(), resampledOther.samples.size());
            std::vector<double> newSamples(newSize, 0.0);

            for (unsigned long long i = 0; i < resampledThis.samples.size(); i++)
            {
                newSamples[i] += resampledThis.samples[i];
            }
            for (unsigned long long i = 0; i < resampledOther.samples.size(); i++)
            {
                newSamples[i] += resampledOther.samples[i];
            }
            AudioSignal result(commonRate, bitDepth, channels);
            result.samples = newSamples;
            return result;
        }
        else
        {
            unsigned long long newSize = max(samples.size(), other.samples.size());
            vector<double> newSamples(newSize, 0.0);

            for (unsigned long long i = 0; i < samples.size(); i++)
            {
                newSamples[i] += samples[i];
            }
            for (unsigned long long i = 0; i < other.samples.size(); i++)
            {
                newSamples[i] += other.samples[i];
            }

            AudioSignal result(sampleRate, bitDepth, channels);
            result.samples = newSamples;
            return result;
        }
    }

    // Nhân tín hiệu với hằng số
    AudioSignal operator*(const double scalar)
    {
        AudioSignal result = *this;
        for (auto &s : result.samples)
        {
            s *= scalar;
        }
        return result;
    }

    // Nhân hai tín hiệu
    AudioSignal operator*(const AudioSignal &other)
    {
        AudioSignal result(sampleRate, bitDepth, channels);
        unsigned long long N = min(samples.size(), other.samples.size());
        result.samples.resize(N);
        for (unsigned long long i = 0; i < N; ++i)
        {
            result.samples[i] = samples[i] * other.samples[i];
        }
        return result;
    }

    // Dịch thời gian
    AudioSignal timeShift(int k)
    {
        AudioSignal result(sampleRate, bitDepth, channels);
        result.samples.resize(samples.size(), 0.0);
        if (k >= 0)
        {
            for (unsigned long long i = 0; i < samples.size() - k; ++i)
                result.samples[i + k] = samples[i];
        }
        else
        {
            for (unsigned long long i = -k; i < samples.size(); ++i)
                result.samples[i + k] = samples[i];
        }
        return result;
    }

    // Đảo ngược tín hiệu
    AudioSignal reverse()
    {
        AudioSignal result(sampleRate, bitDepth, channels);
        result.samples.assign(samples.rbegin(), samples.rend());
        return result;
    }

    // Tích chập
    AudioSignal convolve(std::vector<double> &filter)
    {
        AudioSignal result(sampleRate, bitDepth, channels);
        unsigned long long N = samples.size();
        unsigned long long M = filter.size();
        result.samples.resize(N + M - 1, 0.0);
        for (unsigned long long n = 0; n < N + M - 1; ++n)
        {
            for (unsigned long long m = 0; m < M; ++m)
            {
                if (n >= m && n - m < N)
                {
                    result.samples[n] += samples[n - m] * filter[m];
                }
            }
        }
        return result;
    }

    // Tương quan chéo
    AudioSignal crossCorrelation(AudioSignal &other)
    {
        AudioSignal result(sampleRate, bitDepth, channels);
        vector<double> x = this->samples;
        vector<double> y = other.reverse().samples;
        int sizeResult = x.size() + y.size() - 1;
        result.samples.resize(sizeResult, 0.0);
        for (int n = 0; n < sizeResult; ++n)
        {
            for (int k = 0; k < x.size(); ++k)
            {
                int index = n - k;
                if (index >= 0 && index < y.size())
                {
                    result.samples[n] += x[k] * y[index];
                }
            }
        }
        return result;
    }

    // UpSampling
    AudioSignal upSampling(int factor)
    {
        if (factor <= 1)
            return *this;
        AudioSignal result(sampleRate * factor, bitDepth, channels);
        result.samples.resize(samples.size() * factor, 0.0);

        for (unsigned long long i = 0; i < samples.size(); ++i)
        {
            result.samples[i * factor] = samples[i]; // Giữ nguyên giá trị mẫu, chèn 0 xen kẽ
        }
        return result;
    }

    // DownSampling
    AudioSignal downSampling(int factor)
    {
        if (factor <= 1)
            return *this;
        AudioSignal result(sampleRate / factor, bitDepth, channels);
        for (unsigned long long i = 0; i < samples.size(); i += factor)
        {
            result.samples.push_back(samples[i]); // Chỉ lấy mẫu thứ i, bỏ qua các mẫu khác
        }
        return result;
    }

    // Đọc file WAV
    void readWAVFile(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Loi: Khong the mo file WAV tai duong dan: " << filename << std::endl;
            return;
        }

        WAVHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));

        if (std::string(header.riff_header, 4) != "RIFF" ||
            std::string(header.wave_header, 4) != "WAVE" ||
            std::string(header.fmt_header, 4) != "fmt " ||
            std::string(header.data_header, 4) != "data") {
            throw std::runtime_error("Dinh dang file WAV khong hop le");
        }

        std::cout << "So kenh: " << header.num_channels << std::endl;
        std::cout << "Tan so lay mau: " << header.sample_rate << std::endl;
        std::cout << "bps: " << header.bits_per_sample << std::endl;

        setSampleRate(header.sample_rate);
        setBitDepth(header.bits_per_sample);
        setChannels(header.num_channels);

        if (header.bits_per_sample == 16) {
            std::vector<int16_t> raw_data(header.data_bytes / 2);
            file.read(reinterpret_cast<char*>(raw_data.data()), header.data_bytes);
            for (size_t i = 0; i < raw_data.size(); ++i) {
                samples.push_back(raw_data[i] / 32768.0f);
            }
        } else if (header.bits_per_sample == 8) {
            std::vector<uint8_t> raw_data(header.data_bytes);
            file.read(reinterpret_cast<char*>(raw_data.data()), header.data_bytes);
            for (size_t i = 0; i < raw_data.size(); ++i) {
                samples.push_back((raw_data[i] - 128) / 128.0f);
            }
        } else if (header.bits_per_sample == 24) {
            std::vector<char> raw_data(header.data_bytes);
            file.read(raw_data.data(), header.data_bytes);
            for (size_t i = 0; i < header.data_bytes / 3; ++i) {
                int32_t sample = (raw_data[i*3+2] << 16) | (raw_data[i*3+1] << 8) | raw_data[i*3];
                if (sample & 0x800000) {
                    sample |= 0xFF000000;
                }
                samples.push_back(sample / 8388608.0f);
            }
        }

        file.close();
    }

    // Ghi dữ liệu WAV ra file
    void writeToFile(const std::string& output_filename) {
        std::ofstream outputFile(output_filename);
        if (!outputFile.is_open()) {
            std::cerr << "Loi: Khong the mo file output: " << output_filename << std::endl;
            return;
        }

        for (const auto& sample : samples) {
            outputFile << sample << " ";
        }

        outputFile.close();
        std::cout << "Ghi thanh cong " << samples.size() << " samples vao file " << output_filename << std::endl;
    }


    // Đọc và ghi dữ liệu MP3
    void analyzeMP3File(const std::string& filename, const std::string& outputTxt) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Loi: Khong the mo file MP3: " << filename << std::endl;
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
            if (parseFrameHeader(fileData, offset, header)) {
                foundFrame = true;
                break;
            }
            offset++;
        }
        
        if (!foundFrame) {
            std::cerr << "Khong tim thay frame MP3 phu hop" << std::endl;
            return;
        }
        
        std::cout << "Tim thay MP3 frame tai offset: " << offset << std::endl;
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
            std::cerr << "Khong the mo file output: " << outputTxt << std::endl;
            return;
        }
    
        for (uint8_t byte : fileData) {
            double normalizedValue = byte / 255.0;
            outFile << normalizedValue << " ";
        }
    
        outFile.close();
        std::cout << "Ghi thanh cong vao file " << outputTxt << std::endl;
    }

};

int main() {
    std::string input_filename = "resources/sounds/samplewav.wav";
    std::string input_filename_mp3 = "resources/sounds/guitar.mp3";
    std::string output_filename = "output/outputwav.txt";
    std::string output_filename_mp3 = "output/outputmp3.txt";

    AudioSignal audio_signal(0, 0, 0);
    int choice;
    std::cout << "Choice:\n1. Doc file WAV\n2. Doc file MP3\n";
    std::cin >> choice;

    switch (choice) {
        case 1:
            audio_signal.readWAVFile(input_filename);
            if (!audio_signal.getSamples().empty()) {
                audio_signal.writeToFile(output_filename);
            }
            break;
        case 2:
            audio_signal.analyzeMP3File(input_filename_mp3, output_filename_mp3);
            break;
        case 0:
            exit(0);
            break;
        default:
            std::cout << "Lua chon khong hop le." << std::endl;
            break;
    }
    return 0;
}
