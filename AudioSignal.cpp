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
#include <cstdint>

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
    double mp3Duration = 0.0;

    const int bitrate_table[16] = {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0};

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
        header.data_bytes = samples.size() * (bitDepth / 8);
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
    
	double calculateWAVDuration() {
	    // Lấy thông tin từ header
	    WAVHeader header = getWavHeader();
	    uint32_t sampleRate = header.sample_rate;
	    uint32_t dataSize = header.data_bytes;
	    uint16_t numChannels = header.num_channels;
	    uint16_t bitsPerSample = header.bits_per_sample;
	
	    // Tính toán thời lượng
	    double duration = static_cast<double>(dataSize) / (sampleRate * numChannels * (bitsPerSample / 8));
	
	    return duration;
	}

    void setMp3Duration(double mp3Duration){this -> mp3Duration = mp3Duration;}
    double getMp3Duration() const{return this -> mp3Duration;}


    double getDuration() {
        if (getHeaderType() == 1) {
            return calculateWAVDuration();
        } else if (getHeaderType() == 2) {
            return getMp3Duration();
        } else if (getHeaderType() == 0){
        	return -1;	
		} else {
        	throw std::runtime_error("Unsupported file format");
        }
    }    
    

    const std::string& getFileName() const { return fileName; } 
    void setFileName(std::string filename){
        fileName = filename;
    }

    WAVHeader getWavHeader() const {return wavHeader;}
    void setWavHeader(WAVHeader& wavHeader){this -> wavHeader = wavHeader;}

    MP3FrameHeader getMp3Header() const { return mp3Header; }
    void setMp3Header(const MP3FrameHeader& header) { mp3Header = header; }

    int getHeaderType() const {return headerType;}
    void setHeaderType(int headerType) {
    	this -> headerType = headerType;
	}
	
	void setData(AudioSignal &as){
		// Set header
		this -> headerType = as.getHeaderType();
		if (this -> headerType == 1){
			wavHeader = as.getWavHeader();
		} else if (this -> headerType == 2){
			mp3Header = as.getMp3Header();
		} else {
			std::cerr << "Khong xac dinh duoc dinh dang file" << std::endl;
		}
		
		// Set Filename
		setFileName(as.getFileName());
		
		// Set Duration
		
		// Set Basic info
		sampleRate = as.getSampleRate();
		bitDepth = as.getBitDepth();
		channels = as.getChannels();
	}

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
            readWAVFile(fileName);
        } else if (fileExtension == ".mp3"){
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
        setWavHeader(header);
        setFileName(filename);
        setHeaderType(1);

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

    void writeToFileWAV(std::string& output_filename) {
    	std::string outputPath = "output/" + output_filename;
	    std::ofstream outputFile(outputPath);
	    if (!outputFile.is_open()) {
	        std::cerr << "Error: Could not open output file: " << output_filename << std::endl;
	        return;
	    }
	
	    for (const auto& sample : getSamples()) {
	        outputFile << sample << " ";
	    }
	
	    outputFile.close();
	    std::cout << "Successfully wrote " << samples.size() << " samples to " << output_filename << std::endl;
	}


    // Đọc file MP3
    void readMP3File(const std::string& filename) {
        resetAttributes();
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << filename << std::endl;
            return ;
        }

        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> fileData(fileSize);
        file.read(reinterpret_cast<char*>(fileData.data()), fileSize);
        file.close();


        size_t offset = 0;
        MP3FrameHeader header;
        double totalDuration = 0.0;
        
        // Thiết lập các thuộc tính audio
        setSampleRate(sample_rate_table[header.sampling_rate_index]);
        setBitDepth(16);
        setChannels(header.channel_mode == 3 ? 1 : 2);
        setMp3Header(header);
        setFileName(filename);
        setHeaderType(2);
        
        // Đọc và giải mã các frame MP3
        while (offset < fileSize - 4) {
            if (parseFrameHeader(fileData, offset, header)) {
                if (header.version == 3 && header.layer == 1) { // MPEG Version 1, Layer 3
                    int bitrate = bitrate_table[header.bitrate_index];
                    int sampleRate = sample_rate_table[header.sampling_rate_index];
                    if (bitrate > 0 && sampleRate > 0) {
                        int frameSize = (144 * bitrate * 1000 / sampleRate) + header.padding;
                        double frameDuration = 1152.0 / sampleRate;
                        totalDuration += frameDuration;
    
                        // Đẩy các mẫu vào vector
                        for (int i = 0; i < frameSize; i++) {
                            if (offset + i < fileSize) {
                                samples.push_back(static_cast<double>(fileData[offset + i]) / 255.0);
                            }
                        }
    
                        offset += frameSize;
                    } else {
                        offset++;
                    }
                } else {
                    offset++;
                }
            } else {
                offset++;
            } 
    
		}
        setMp3Duration(totalDuration);
	}
    
    std::vector<double> decodeMP3Frame(const std::vector<uint8_t>& data, size_t offset, int frameSize, const MP3FrameHeader& header) {
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
    
    size_t findMP3Frame(const std::vector<uint8_t>& data) {
        for (size_t i = 0; i < data.size() - 1; i++) {
            if (data[i] == 0xFF && (data[i + 1] & 0xE0) == 0xE0) {
                return i;
            }
        }
        return data.size();
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