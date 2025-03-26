#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <unistd.h>
#include "AudioSignal.cpp"

using std::cin;
using std::cout;
using std::string;
using std::endl;

#define AS AudioSignal
#define clrscr system("cls")
#define precision(x) setprecision(x) << fixed

std::vector<AS> waves(5);
namespace Interface{

    void listenWave(AS &as);

    void configWave(AS &as);

    void mainMenu();

    void getWaveInfo(AS &as);

    void changeFileSource(AS &as);

    void changeSampleRate(AS &as);

    void changeBitDepth(AS &as);

    void changeNumChannel(AS &as);

    void reverseSamples(AS &as);

    void multipleWithConst(AS &as);

    void timeShifting(AS &as);

    void selectWave(int &n);

    void addingWave(AS &as);

    void multipleWave(AS &as);

    void convolutionWave(AS &as);

    void crossCorrelationWave(AS &as);

    void upsamplingWave(AS &as);

    void downsamplingWave(AS &as);

    void writeToFile(AS &as);

    void mainMenu(){
        while (true){
            clrscr;
            cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
            cout << "1. Config tin hieu 1" << endl;
            cout << "2. Config tin hieu 2" << endl;
            cout << "3. Config tin hieu 3" << endl;
            cout << "0. Thoat chuong trinh" << endl;
            cout << "=============================================================" << endl;
            cout << "Lua chon: ";
            int choice;
            cin >> choice;
            switch (choice)
            {
            case 1:
                configWave(waves[1]);
                break;
            case 2:
                configWave(waves[2]);
                break;
            case 3:
                configWave(waves[3]);
                break;
            default:
                cout << "Nhom thuc hien: SNDPROGSP25PRJ007" << endl;
                cout << "Thanh vien nhom: " << endl;
                cout << '\t' << "- Nguyen Duc Anh - B22DPCT009" << endl;
                cout << '\t' << "- Nguyen Trong Chuc - B22DPCT033" << endl;
                cout << '\t' << "- Van Thien Phuc - B22DPCT209" << endl;
                cout << '\t' << "- Pham Ha Thang - B22DPCT261" << endl;
                sleep(3);
                return;
                break;
            }
        }
    }

    void configWave(AS &as){
        int choice;
        while (true){
            clrscr;
            cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
            getWaveInfo(as);
            cout << "1. Chon file" << endl;
            cout << "2. Thay doi Sampling Rate" << endl;
            cout << "3. Thay doi Bit Depth" << endl;
            cout << "4. Thay doi thay doi Channel Mode" << endl;
            cout << "5. Dao nguoc tin hieu" << endl;
            cout << "6. Phep dich thoi gian - Time Shift" << endl;
            cout << "7. Nhan tin hieu voi hang so" << endl;
            cout << "8. Cong hai tin hieu" << endl;
            cout << "9. Nhan hai tin hieu" << endl;
            cout << "10. Tich chap" << endl;
            cout << "11. Tuong quan cheo" << endl;
            cout << "12. Upsampling" << endl;
            cout << "13. Downsampling" << endl;
            cout << "14. Nghe file am thanh" << endl; //-> tach file rieng
            cout << "15. Ghi ra file" << endl; //fix 
            cout << "0. Quay ve" << endl;
            cout << "=============================================================" << endl;
            cout << "Lua chon: ";
            cin >> choice;
            switch (choice)
            {
            case 0:
                return;
                break;
            case 1:
                changeFileSource(as);
                break;
            case 2:
                changeSampleRate(as);
                break;
            case 3:
                /* code */
                changeBitDepth(as);
                break;
            case 4:
                /* code */
                changeNumChannel(as);
                break;
            case 5:
                reverseSamples(as);
                /* code */
                break;
            case 6:
                timeShifting(as);
                /* code */
                break;
            case 7:
                /* code */
                multipleWithConst(as);
                break;
            case 8:
                /* code */
                addingWave(as);
                break;
            case 9:
                /* code */
                multipleWave(as);
                break;
            case 10:
                /* code */
                convolutionWave(as);
                break;
            case 11:
                /* code */
                crossCorrelationWave(as);
                break;
            case 12:
                /* code */
                upsamplingWave(as);
                break;
            case 13:
                /* code */
                downsamplingWave(as);
                break;
            case 14:
                /* code */
                cout << "Chuc nang nay chua ho tro" << endl;
                system("pause");
                break;
            case 15:
                /* code */
                writeToFile(as);
                break;
            default:
                cout << "Chon lai chuc nang!" << endl;
                break;
            }
        }
    }

    void getWaveInfo(AS &as){
        cout << "=============================================================" << endl;
        cout << "Ten file: " << as.getFileName() << endl;
        cout << "So luong kenh: " << as.getChannels() << endl;
        cout << "Do sau cua Bit: " << as.getBitDepth() << endl;
        cout << "Tan so lay mau: " << as.getSampleRate() << endl;
        cout << "So luong sample: " << as.getSampleSize() << endl;
        cout << "Thoi luong file: " << precision(4) << as.getDuration() << endl;
        cout << "=============================================================" << endl;
        cout << "===============================Header========================" << endl;
        if (as.getHeaderType() == 1){
            AS::WAVHeader wavHeader = as.getWavHeader();
            std::cout << "\tWAV Header" << std::endl;
            std::cout << "RIFF Header: " << std::string(wavHeader.riff_header, 4) << std::endl;
            std::cout << "WAV Size: " << wavHeader.wav_size << std::endl;
            std::cout << "WAVE Header: " << std::string(wavHeader.wave_header, 4) << std::endl;
            std::cout << "FMT Header: " << std::string(wavHeader.fmt_header, 4) << std::endl;
            std::cout << "FMT Chunk Size: " << wavHeader.fmt_chunk_size << std::endl;
            std::cout << "Audio Format: " << wavHeader.audio_format << std::endl;
            std::cout << "Number of Channels: " << wavHeader.num_channels << std::endl;
            std::cout << "Sample Rate: " << wavHeader.sample_rate << std::endl;
            std::cout << "Byte Rate: " << wavHeader.byte_rate << std::endl;
            std::cout << "Block Align: " << wavHeader.block_align << std::endl;
            std::cout << "Bits Per Sample: " << wavHeader.bits_per_sample << std::endl;
            std::cout << "Data Header: " << std::string(wavHeader.data_header, 4) << std::endl;
            std::cout << "Data Bytes: " << wavHeader.data_bytes << std::endl;
        } else if (as.getHeaderType() == 2){
              AS::MP3FrameHeader mp3Header = as.getMp3Header();
            std::cout << "\tMP3 Header" << std::endl;
            std::cout << "Version: " << mp3Header.version << std::endl;
            std::cout << "Layer: " << mp3Header.layer << std::endl;
            std::cout << "Protection: " << (mp3Header.protection ? "Yes" : "No") << std::endl;
            std::cout << "Bitrate Index: " << mp3Header.bitrate_index << std::endl;
            std::cout << "Sampling Rate Index: " << mp3Header.sampling_rate_index << std::endl;
            std::cout << "Padding: " << (mp3Header.padding ? "Yes" : "No") << std::endl;
            std::cout << "Private Bit: " << (mp3Header.private_bit ? "Yes" : "No") << std::endl;
            std::cout << "Channel Mode: " << mp3Header.channel_mode << std::endl;
            std::cout << "Mode Extension: " << mp3Header.mode_extension << std::endl;
            std::cout << "Copyright: " << (mp3Header.copyright ? "Yes" : "No") << std::endl;
            std::cout << "Original: " << (mp3Header.original ? "Yes" : "No") << std::endl;
            std::cout << "Emphasis: " << mp3Header.emphasis << std::endl;
        } else {
            cout << "Hay load file wav hoac mp3 phu hop truoc." << endl;
        }
        cout << "=============================================================" << endl;
    }

    void changeFileSource(AS &as){
        string fileName;
        bool check = true;
        while (check){
            clrscr;
            cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
            cout << "Chuong trinh chi ho tro file .wav va .mp3" << endl;
            cout << "Nhap ten file day du ca ten + phan mo rong" << endl;
            cout << "File can phai dat trong folder resources/sounds" << endl;
            cout << "Ten file can phai khong co dau cach" << endl;
            cout << "\tNhap ten file: ";
            cin >> fileName;
            fileName = "resources/sounds/" + fileName;
            string extension = fileName.substr(fileName.length() - 4);
            if (extension == ".mp3" or extension == ".wav"){
                try{
                    as.loadFromFile(fileName);
                } catch (exception &e){
                    std::cerr << "Can't load your file" << endl;
                }
                system("pause");
                check = false;
            } else {
                char c;
                cout << "Chuong trinh chi ho tro file .wav va .mp3" << endl;
                cout << "Ban co muon thu lai? (Y/y = yes, N/n = no): ";
                cin >> c;
                if (c == 'y' or c == 'Y'){
                    check = true;
                } else check = false;
            }
        }
    }

    void changeSampleRate(AS &as){
        int v;
        clrscr;
        cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
        cout << "\tNhap BitDepth: ";
        cin >> v;
        as.setSampleRate(v);
        cout << "\tBitDepth hien tai: " << as.getBitDepth() << "Hz" << endl;
        system("pause");
        return;
    }
    
    void changeBitDepth(AS &as){
        int v;
        clrscr;
        cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
        cout << "\tNhap tan so: ";
        cin >> v;
        as.setBitDepth(v);
        cout << "\tTan so mau hien tai: " << as.getSampleRate() << "Hz" << endl;
        system("pause");
        return;
    }

    void changeNumChannel(AS &as){
        int value;
        bool check = true;
        char c;
        while (check){
            clrscr;
            cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
            cout << "\tMONO = 1 | STEREO = 2" << endl;
            cout << "Nhap kenh muon thay doi: " << endl;
            cin >> value;
            if (value < 3 && value > 0){
                as.setChannels(value);
                cout << "\tSo luong kenh hien tai: " << as.getChannels() << endl;
                check = false;
                sleep(2);
            } else {
                cout << "\tKhong ho tro loai kenh nay." << endl;
                cout << "\tBan co muon nhap lai?(Y/y = yes, N/n = no)";
                cin >> c;
                if (c == 'Y' or c == 'y'){
                    check = true;
                } else {
                    check = false;
                }
            }
        }
    }

    void reverseSamples(AS &as){
        clrscr;
        cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
        cout << "\tDang xu li..." << endl;
        AS res = as.reverse();	
		res.setData(as);
		as = res;		
        cout << "\tDao nguoc tin hieu thanh cong!" << endl;
        system("pause");
    }

    void multipleWithConst(AS &as){
        double n;
        bool check = true;
        char c;
        while (check){
            clrscr;
            cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
            cout << "\tNhap hang so(>1, so thuc): ";
            cin >> n;
            if (n <= 0){
                cout << "\tKhong hop le. Ban co muon nhap lai?(Y/y = yes, N/n = no):";
                cin >> c;
                if (c == 'Y' or c == 'y'){
                    check = true;
                } else check = false;
            } else if (n >= 5){
                cout << "\tSo qua lon, anh huong den hieu suat may" << endl;
                cout << "\tBan co muon tiep tuc?(Y/y = yes, N/n = no): ";
                cin >> c;
                if (c == 'Y' or c == 'y'){

                } else {
                    check =true;
                    continue;
                }
            }
            cout << "\tDang xu ly..." << endl;
            as = as * n;
            cout << "\tXu ly thanh cong!" << endl;
            system("pause");
            return;
        }
    }

    void timeShifting(AS &as){
        int n;
        bool check = true;
        char c;
        clrscr;
        cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
        cout << "\tNhap hang so(so nguyen): ";
        cin >> n;
        cout << "\tDang xu ly..." << endl;
        AS res = as.timeShift(n);	
		res.setData(as);
		as = res;
        cout << "\tXu ly thanh cong!" << endl;
        system("pause");
    }

    void selectWave(int &n){
        bool check = true;
        while (check){
            clrscr;
            cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
            cout << "Lua chon tin hieu de xu ly phep toan" << endl;
            cout << "\t1. Tin hieu 1" << endl;
            cout << "\t2. Tin hieu 2" << endl;
            cout << "\t3. Tin hieu 3" << endl;
            cout << "\tLua chon: ";
            int s; cin >> s;
            switch(s){
                case 1:
                    n = s;
                    check = false;
                    break;
                case 2:
                    n = s;
                    check = false;
                    break;
                case 3:
                    n = s;
                    check = false;
                    break;
                default:
                    cout << "\tTin hieu khong phu hop. Vui long nhap lai!" << endl;
                    system("pause");
                    break;
            }
        }
    }

    void addingWave(AS &as){
        int n;
        int c;
        selectWave(n);
        clrscr;
        cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
        waves[0] = as + waves[n];
        waves[0].setData(as);
        cout << "Hoan thanh. Ban co muon ghi ra file?" << endl;
        cout << "1. Ghi ra file" << endl;
        cout << "0. Khong lam gi" << endl;
        cout << "Lua chon: " << endl;
        cin >> c;
        if (c == 1){
            writeToFile(waves[0]);
        }
    }

    void multipleWave(AS &as){
        int n;
        int c;
        selectWave(n);
        clrscr;
        cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
        waves[0] = as * waves[n];
        waves[0].setData(as);
        cout << "Hoan thanh. Ban co muon nghe thu hay ghi ra file?" << endl;
        cout << "1. Ghi ra file" << endl;
        cout << "0. Khong lam gi" << endl;
        cout << "Lua chon: " << endl;
        cin >> c;
        if (c == 1){
             writeToFile(waves[0]);
        }
    }

    void convolutionWave(AS &as){
        int n;
        int c;
        selectWave(n);
        clrscr;
        cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
        waves[0] = as.convolve(waves[n]);
        waves[0].setData(as);
        cout << "Hoan thanh. Ban co muon nghe thu hay ghi ra file?" << endl;
        cout << "1. Ghi ra file" << endl;
        cout << "0. Khong lam gi" << endl;
        cout << "Lua chon: " << endl;
        cin >> c;
        if (c == 1){
            writeToFile(waves[0]);
        }
    }

    void crossCorrelationWave(AS &as){
        int n;
        int c;
        selectWave(n);
        clrscr;
        cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
        waves[0] = as.crossCorrelation(waves[n]);
        waves[0].setData(as);
        cout << "Hoan thanh. Ban co muon nghe thu hay ghi ra file?" << endl;
        cout << "1. Ghi ra file" << endl;
        cout << "0. Khong lam gi" << endl;
        cout << "Lua chon: " << endl;
        cin >> c;
        if (c == 1){
            writeToFile(waves[0]);
        }
    }

    void upsamplingWave(AS &as){
        int n;
        bool check = true;
        char c;
        clrscr;
        cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
        cout << "\tNhap hang so(so nguyen): ";
        cin >> n;
        cout << "\tDang xu ly..." << endl;
        AS res = as.upSampling(n);	
		res.setData(as);
		as = res;
        cout << "\tXu ly thanh cong!" << endl;
        system("pause");
    }

    void downsamplingWave(AS &as){
        int n;
        bool check = true;
        char c;
        clrscr;
        cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
        cout << "\tNhap hang so(so nguyen): ";
        cin >> n;
        cout << "\tDang xu ly..." << endl;
        AS res = as.downSampling(n);	
		res.setData(as);
		as = res;
        cout << "\tXu ly thanh cong!" << endl;
        system("pause");
    }

    void writeToFile(AS &as){
        string filename="";
        bool check = true;
        while (check){
            clrscr;
            cout << "Chuong trinh dieu chinh am thanh - Sound Equalizer Program" << endl;
            cout << "\tBan co muon luu file?" << endl;
            cout << "\t1. Co" << endl;
            cout << "\t0. Quay lai" << endl;
            cout << "Lua chon: ";
            int choice;
            cin >> choice;
            switch(choice){
                case 1:
                    cout << "Nhap ten file(khong can nhap phan mo rong, khong nen chua dau cach):" ;
                    cin >> filename;
                    filename += ".txt";
                    if (as.getHeaderType() == 1){
                        as.writeToFileWAV(filename);
                        system("pause");
                        return;
                    } else if (as.getHeaderType() == 2){
                        as.writeMP3DataToFile(filename, as.getSamples());
                        system("pause");
                        return;
                    } else {
                        cout << "Can't write" << endl;
                        system("pause");
                        return;
                    }
                    break;
                default:
                    return;
            }
        }
    }
}