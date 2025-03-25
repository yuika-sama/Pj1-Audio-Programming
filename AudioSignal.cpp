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

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "conio.lib")

using namespace std;

class AudioSignal {
private:
    std::vector<double> samples;
    unsigned int sampleRate;
    unsigned int bitDepth;
    unsigned int channels;
    std::string fileName;
    bool isPlaying = false;
    bool isPaused = false;
    int headerType = 0;

    const int sample_rate_table[4] = {44100, 48000, 32000, 0};

    // Reset các attributes
    void resetAttributes(){
        samples.clear();
        sampleRate = 0;
        bitDepth = 0;
        channels = 0;
        headerType = 0;
        fileName.clear();
        memset(&wavHeader, 0, sizeof(WAVHeader)); // Reset header WAV
        memset(&mp3Header, 0, sizeof(MP3FrameHeader)); // Reset header MP3
    }

    //Phát nhạc
    void playSound(std::string filename){
        string filePath = "resources/sounds/" + filename;
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
    // Dừng phát nhạc
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
    // Kiểm tra trạng thái đang phát/tạm dừng
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

    std::string createTempWavFile() {
        std::string tempFileName = "resources/sounds/temp_audio_" + std::to_string(rand()) + ".wav";
        std::ofstream outFile(tempFileName, std::ios::binary);
        if (!outFile.is_open()) {
            throw std::runtime_error("Không thể tạo file tạm thời");
        }

        // Ghi header WAV
        WAVHeader header = createWAVHeader();
        outFile.write(reinterpret_cast<const char*>(&header), sizeof(WAVHeader));

        // Ghi dữ liệu mẫu
        for (const auto& sample : samples) {
            int16_t intSample = static_cast<int16_t>(sample * 32767);
            outFile.write(reinterpret_cast<const char*>(&intSample), sizeof(int16_t));
        }

        outFile.close();
        return tempFileName;
    }

public:
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
    WAVHeader wavHeader;
    MP3FrameHeader mp3Header;
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
    WAVHeader createWAVHeader() {
        WAVHeader header;
        
        strncpy(header.riff_header, "RIFF", 4);
        strncpy(header.wave_header, "WAVE", 4);
        strncpy(header.fmt_header, "fmt ", 4);
        strncpy(header.data_header, "data", 4);
        
        header.fmt_chunk_size = 16;
        header.audio_format = 1; // PCM
        header.num_channels = channels;
        header.sample_rate = sampleRate;
        header.bits_per_sample = bitDepth;
        
        header.byte_rate = sampleRate * channels * (bitDepth / 8);
        header.block_align = channels * (bitDepth / 8);
        
        header.data_bytes = samples.size() * (bitDepth / 8) * channels;
        header.wav_size = 36 + header.data_bytes;
        
        return header;
    }
    // Constructor
    AudioSignal(unsigned int sampleRate = 0, unsigned int bitDepth = 0, unsigned int channels = 0)
        : sampleRate(sampleRate), bitDepth(bitDepth), channels(channels)  {}

    AudioSignal(std::string &fileName){loadFromFile(fileName);}

   	~AudioSignal() {}

    // Getter/Setter
    unsigned int getSampleRate() const { return sampleRate; }
    void setSampleRate(unsigned int sampleRate) { this->sampleRate = sampleRate; }

    unsigned int getBitDepth() const { return bitDepth; }
    void setBitDepth(unsigned int bitDepth) { this->bitDepth = bitDepth; }

    unsigned int getChannels() const { return channels; }
    void setChannels(unsigned int channels) { this->channels = channels; }

    std::vector<double> getSamples() const { return samples; }
    void setSamples(std::vector<double> samples) { this->samples = samples; }
    
    unsigned int getSampleSize() const {return samples.size();}

    float getDurations() const {
	    if (samples.empty()) {
	        return 0;
	    }
	    return samples.size() * 1.0f / sampleRate;
	}

    const std::string& getFileName() const { return fileName; } 
    void setFileName(std::string filename){
        fileName = filename;
    }

    WAVHeader getWavHeader() const {return wavHeader;}
    void setWavHeader(WAVHeader& wavHeader){wavHeader = wavHeader;}

    MP3FrameHeader getMp3Header() const { return mp3Header; }
    void setMp3Header(const MP3FrameHeader& header) { mp3Header = header; }

    int getHeaderType() const {return headerType;}

    //End of Getter/setter

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
    AudioSignal convolve(AudioSignal &other)
    {
        vector<double> filter = other.samples;
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

    // Xử lý nhập, xuất file

    //Validate file type
    void loadFromFile(std::string& fileName){
        std::string fileExtension = fileName.substr(fileName.size() - 4);
        if (fileExtension == ".wav"){
            headerType = 1;
            readWAVFile(fileName);
        } else if (fileExtension == ".mp3"){
            headerType = 2;
            readMP3File(fileName);
        } else {
            throw std::invalid_argument("File khong phu hop. File can thuoc loai .wav hoac .mp3");
            return;
        }
    }

    // Đọc file WAV
    void readWAVFile(const std::string& filename){
        resetAttributes();
        // Handle open file
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Loi: Khong the mo file WAV tai duong dan: " << filename << std::endl;
            return;
        }

        //Kiểm tra định dạng thông qua phần mở rộng của file
        if (filename.substr(filename.size() - 4) != ".wav"){
            std::cerr << "Dinh dang mo rong cua file phai la .wav: " << filename << std::endl;
        }

        // Cast về struct WAVHeader
        WAVHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
        //Kiểm tra định dạng thông qua Header
        if (std::string(header.riff_header, 4) != "RIFF" ||
            std::string(header.wave_header, 4) != "WAVE" ||
            std::string(header.fmt_header, 4) != "fmt " ||
            std::string(header.data_header, 4) != "data") {
            throw std::runtime_error("Dinh dang file WAV khong hop le");
        }

        // Đặt các giá trị của audio cho các thuộc tính
        setSampleRate(header.sample_rate);
        setBitDepth(header.bits_per_sample);
        setChannels(header.num_channels);
        setFileName(filename);
        setWavHeader(header);

        //Xử lý truyền tín hiệu vào vector theo các trường hợp BPS
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
                int32_t sample = (static_cast<int8_t>(raw_data[i*3 + 2]) << 16) | (raw_data[i*3 + 1] << 8) | raw_data[i*3];
                if (sample & 0x800000) {
                    sample |= 0xFF000000;
                }
                samples.push_back(sample / 8388608.0f);
            }
        }
        file.close();
    }

    // Xuất file tín hiệu của WAV
    void writeToFileWAV(std::string &output_filename){
        output_filename = "output/" + output_filename;
        std::ofstream outputFile(output_filename, std::ios::binary); // Thêm chế độ binary
        // Ghi lại header và dữ liệu nhị phân
        WAVHeader header = createWAVHeader();
        outputFile.write(reinterpret_cast<const char*>(&header), sizeof(WAVHeader));
        
        for (const auto& sample : samples) {
            int16_t intSample = static_cast<int16_t>(sample * 32767.0);
            outputFile.write(reinterpret_cast<const char*>(&intSample), sizeof(int16_t));
        }
    }

    // Đọc file MP3
    void readMP3File(std::string& filename){
        resetAttributes();
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

        if (fileData.empty()) {
            std::cerr << "Khong co du lieu de xu ly." << std::endl;
            return;
        }

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

        setSampleRate(sample_rate_table[header.sampling_rate_index]);
        setBitDepth(16); //giả định bit depth là 16
        setChannels(header.channel_mode == 3 ? 1 : 2);
        setMp3Header(header);
        
    }

    //Xuất file tín hiệu của MP3
    void writeMP3DataToFile(const std::string& outputTxt, const std::vector<double>& fileData) {
        string outputPath = "output/" + outputTxt;
        std::ofstream outFile(outputPath);
        if (!outFile.is_open()) {
            std::cerr << "Khong the mo file output: " << outputTxt << std::endl;
            return;
        }

        for (uint8_t byte : fileData) {
            double sample = byte / 255.0;
            outFile << sample << " ";
        }
    
        outFile.close();
        std::cout << "Ghi thanh cong vao file " << outputTxt << std::endl;
    }

    //Phát âm thanh
    void play() {
        std::string tempFile = createTempWavFile();
        playSound(tempFile);
        
        while (true) {
            if (_kbhit()) {
                if (_getch() == 32) {
                    pauseSound();
                }
            }
            checkSoundStatus();
            Sleep(100);
            
            if (!isPlaying) {
                break;
            }
        }
        
        std::remove(tempFile.c_str());
    }
    
    AudioSignal& operator=(const AudioSignal& other) {
	    if (this == &other) return *this;
	
	    // Copy các thành viên cơ bản
	    samples = other.samples;
	    sampleRate = other.sampleRate;
	    bitDepth = other.bitDepth;
	    channels = other.channels;
	    fileName = other.fileName;
	
	    // Copy WAVHeader
	    wavHeader = other.wavHeader;
	
	    // Copy MP3Header
	    mp3Header = other.mp3Header;
	
	    // Copy các trạng thái
	    isPlaying = other.isPlaying;
	    isPaused = other.isPaused;
	    headerType = other.headerType;
	
	    return *this;
	}
    
};