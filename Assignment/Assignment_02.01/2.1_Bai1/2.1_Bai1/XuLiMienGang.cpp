#include <iostream>
#include <Windows.h>
#include <Gdiplus.h>
#include <fstream>

using namespace std;
using namespace Gdiplus;

CRITICAL_SECTION cs;

// Hàm trợ giúp để lấy CLSID của encoder tương ứng với định dạng ảnh
// Chúng ta sử dụng hàm này để lưu ảnh với định dạng đúng
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;           // số lượng encoder
    UINT size = 0;          // kích thước thông tin encoder
    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if (size == 0) {
        return -1;          // không tìm thấy encoder
    }

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) {
        return -1;          // không đủ bộ nhớ
    }

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;       // trả về vị trí của encoder
        }
    }

    free(pImageCodecInfo);
    return -1;              // không tìm thấy encoder
}


void takeScreenshot() {
    EnterCriticalSection(&cs);

    // Khởi tạo thư viện GDI+
    ULONG_PTR token;
    GdiplusStartupInput input;
    GdiplusStartupOutput output;
    GdiplusStartup(&token, &input, &output);

    // Lấy thông tin màn hình
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    HDC screenDC = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(screenDC);
    HBITMAP bitmap = CreateCompatibleBitmap(screenDC, screenWidth, screenHeight);
    SelectObject(memDC, bitmap);

    // Chụp ảnh màn hình
    BitBlt(memDC, 0, 0, screenWidth, screenHeight, screenDC, 0, 0, SRCCOPY);

    // Lưu ảnh màn hình vào file
    CLSID pngClsid;
    GetEncoderClsid(L"image/png", &pngClsid);
    WCHAR filename[MAX_PATH];
    GetTempFileName(L".", L"screenshot_", 0, filename);
    Image* image = new Bitmap(bitmap, (HPALETTE)NULL);
    image->Save(filename, &pngClsid, NULL);

    // Giải phóng tài nguyên
    delete image;
    DeleteObject(bitmap);
    DeleteDC(memDC);
    ReleaseDC(NULL, screenDC);
    GdiplusShutdown(token);

    cout << "Screenshot saved to: " << filename << endl;

    LeaveCriticalSection(&cs);
}

void captureKeyboard() {
    EnterCriticalSection(&cs);

    // Mở file để lưu kết quả
    ofstream outFile;
    WCHAR filename[MAX_PATH];
    GetTempFileName(L".", L"keylog_", 0, filename);
    outFile.open(filename, ios_base::out | ios_base::app);

    // Bắt đầu capture bàn phím
    cout << "Press ESC to stop capturing keyboard" << endl;
    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            break;
        }
        for (int i = 8; i < 256; i++) {
            if (GetAsyncKeyState(i) & 0x8000) {
                outFile << (char)i;
            }
        }
    }

    // Đóng file
    outFile.close();

    cout << "Keyboard captured and saved to: " << filename << endl;

    LeaveCriticalSection(&cs);
}

void shutdown() {
    EnterCriticalSection(&cs);

    // Tắt máy tính
    ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCEIFHUNG, SHTDN_REASON_MAJOR_OPERATINGSYSTEM);

    LeaveCriticalSection(&cs);
}

int main() {
    // Khởi tạo critical section
    InitializeCriticalSection(&cs);

    while (true) {
        cout << "Select an option:" << endl;
        cout << "1. Take a screenshot" << endl;
        cout << "2. Capture keyboard" << endl;
        cout << "3. Shutdown" << endl;
        cout << "4. Exit" << endl;
        cout << "Your choice: ";
        int choice;
        cin >> choice;
        switch (choice) {
        case 1:
            takeScreenshot();
            break;
        case 2:
            captureKeyboard();
            break;
        case 3:
            shutdown();
            break;
        case 4:
            return 0;
        default:
            cout << "Invalid choice" << endl;
        }
    }
}
