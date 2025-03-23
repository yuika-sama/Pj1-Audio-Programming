#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#pragma comment(lib, "winmm.lib")

// Cấu trúc header của WAV
struct Twavheader {
    char chunk_ID[4];
    uint32_t chunk_size;
    char format[4];
    char sub_chunk1_ID[4];
    uint32_t sub_chunk1_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    char sub_chunk2_ID[4];
    uint32_t sub_chunk2_size;
};

// Dữ liệu âm thanh
std::vector<int16_t> audioData;
std::string fileName;
uint32_t sampleRate = 0;
uint32_t fileSize = 0;
double duration = 0.0;

// Số lượng mẫu tín hiệu hiển thị trên màn hình
const int MAX_SAMPLES_ON_SCREEN = 1024;

// Đọc file WAV
bool readWavFile(const std::string& filePath) {
    std::ifstream wavFile(filePath, std::ios::binary);
    if (!wavFile.is_open()) {
        std::cerr << "Error opening WAV file." << std::endl;
        return false;
    }

    Twavheader wav;
    wavFile.read(reinterpret_cast<char*>(&wav), sizeof(Twavheader));

    if (strncmp(wav.chunk_ID, "RIFF", 4) != 0 || strncmp(wav.format, "WAVE", 4) != 0) {
        std::cerr << "Not a valid WAVE file!" << std::endl;
        return false;
    }

    sampleRate = wav.sample_rate;
    fileSize = wav.chunk_size + 8;
    duration = static_cast<double>(wav.sub_chunk2_size) / wav.byte_rate;

    audioData.resize(wav.sub_chunk2_size / sizeof(int16_t));
    wavFile.seekg(44);
    wavFile.read(reinterpret_cast<char*>(audioData.data()), wav.sub_chunk2_size);
    wavFile.close();

    return true;
}

// Vẽ cửa sổ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int scrollPos = 0;

    switch (uMsg) {
    case WM_CREATE: {
        char filePath[MAX_PATH] = "";
        OPENFILENAME ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFilter = "WAV Files\0*.wav\0";
        ofn.lpstrFile = filePath;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
        ofn.lpstrTitle = "Select a WAV File";

        if (GetOpenFileName(&ofn)) {
            fileName = filePath;
            if (readWavFile(fileName)) {
                SCROLLINFO si = {};
                si.cbSize = sizeof(SCROLLINFO);
                si.fMask = SIF_RANGE | SIF_PAGE;
                si.nMin = 0;
                si.nMax = static_cast<int>(audioData.size()) - 1;
                si.nPage = MAX_SAMPLES_ON_SCREEN;
                SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);

                InvalidateRect(hwnd, NULL, TRUE);
            }
        }
    } break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = 600;
        int height = rect.bottom; 
        int centerY = height / 2;
        int scaleY = centerY - 20;
        int offsetY = 10;

        // Xóa vùng vẽ trước khi vẽ lại
        FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // Hiển thị thông tin file
        std::ostringstream info;
        info << "File: " << fileName.substr(fileName.find_last_of("\\") + 1)
             << " | Size: " << fileSize / 1024 << " KB"
             << " | Duration: " << duration << " s";
        TextOut(hdc, 10, 10, info.str().c_str(), info.str().length());

        // Vẽ trục X (thời gian)
        MoveToEx(hdc, 50, centerY+ offsetY, NULL);
        LineTo(hdc, width - 10, centerY+ offsetY);
        TextOut(hdc, width - 50, centerY + 5+ offsetY, "Time (ms)", 10);

        // Vẽ trục Y (biên độ)
        MoveToEx(hdc, 50, 20+ offsetY, NULL);
        LineTo(hdc, 50, height - 20+ offsetY);
        TextOut(hdc, 10, 20+ offsetY, "+32767", 6);
        TextOut(hdc, 10, centerY - 5+ offsetY, "0", 1);
        TextOut(hdc, 10, height - 30+ offsetY, "-32768", 6);

		// Vẽ sóng âm
		MoveToEx(hdc, 50, centerY, NULL);
		for (int i = 0; i < MAX_SAMPLES_ON_SCREEN && (scrollPos + i) < audioData.size(); ++i) {
		    int x = 50 + (i * 0.5);
		    int y = centerY - (audioData[scrollPos + i] * scaleY / 16384) + offsetY;
		    LineTo(hdc, x, y);
		}

        EndPaint(hwnd, &ps);
    } break;

    case WM_HSCROLL: {
        SCROLLINFO si = {};
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_HORZ, &si);

        int scrollCode = LOWORD(wParam);
        int newScrollPos = scrollPos;

        switch (scrollCode) {
        case SB_LINELEFT:
            newScrollPos = std::max(0, scrollPos - 1);
            break;
        case SB_LINERIGHT:
            newScrollPos = std::min(static_cast<int>(audioData.size()) - MAX_SAMPLES_ON_SCREEN, scrollPos + 1);
            break;
        case SB_PAGELEFT:
            newScrollPos = std::max(0, scrollPos - MAX_SAMPLES_ON_SCREEN);
            break;
        case SB_PAGERIGHT:
            newScrollPos = std::min(static_cast<int>(audioData.size()) - MAX_SAMPLES_ON_SCREEN, scrollPos + MAX_SAMPLES_ON_SCREEN);
            break;
        case SB_THUMBTRACK:
            newScrollPos = si.nTrackPos;
            break;
        }

        if (newScrollPos != scrollPos) {
            scrollPos = newScrollPos;
            si.fMask = SIF_POS;
            si.nPos = scrollPos;
            SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
            InvalidateRect(hwnd, NULL, TRUE);
        }
    } break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    const char CLASS_NAME[] = "AudioVisualizer";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "WAV Visualizer", WS_OVERLAPPEDWINDOW | WS_HSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 600,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (!hwnd) {
        std::cerr << "Failed to create window!" << std::endl;
        return 1;
    }

    ShowWindow(hwnd, SW_SHOW);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}