#include <iostream>
#include <chrono>
#include <thread>
#include <windows.h>

// Thời gian sử dụng tối đa (tính bằng phút)
const int MAX_TIME = 60;

// Thời gian kiểm tra (tính bằng giây)
const int CHECK_INTERVAL = 10;

int main() {
    // Lấy handle của desktop và quyền điều khiển desktop
    HDESK desktop = GetThreadDesktop(GetCurrentThreadId());
    HDESK defaultDesktop = OpenDesktop(L"Default", 0, FALSE, GENERIC_ALL);
    if (desktop == NULL || defaultDesktop == NULL) {
        std::cerr << "Failed to get desktop handle" << std::endl;
        return 1;
    }

    // Lấy thời gian hiện tại
    auto startTime = std::chrono::system_clock::now();

    while (true) {
        // Tính thời gian sử dụng
        auto currentTime = std::chrono::system_clock::now();
        int elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
        int remainingSeconds = MAX_TIME * 1 - elapsedSeconds;

        // Kiểm tra thời gian sử dụng
        if (remainingSeconds <= 0) {
            // Hiển thị thông báo
            MessageBox(NULL, L"Time limit exceeded!", L"Alert", MB_OK | MB_ICONWARNING);

            // Đổi về desktop mặc định
            if (!SetThreadDesktop(defaultDesktop)) {
                std::cerr << "Failed to switch to default desktop" << std::endl;
                return 1;
            }

            // Thoát chương trình
            return 0;
        }

        // Hiển thị thời gian còn lại
        std::cout << "Time remaining: " << remainingSeconds / 60 << " minutes " << remainingSeconds % 60 << " seconds" << std::endl;

        // Đợi một khoảng thời gian để kiểm tra lại
        std::this_thread::sleep_for(std::chrono::seconds(CHECK_INTERVAL));

        // Kiểm tra xem desktop hiện tại có phải là desktop mặc định không
        if (GetThreadDesktop(GetCurrentThreadId()) != desktop) {
            // Đổi về desktop mặc định
            if (!SetThreadDesktop(defaultDesktop)) {
                std::cerr << "Failed to switch to default desktop" << std::endl;
                return 1;
            }

            // Hiển thị thông báo
            MessageBox(NULL, L"Access to other desktops is not allowed!", L"Alert", MB_OK | MB_ICONWARNING);
        }
    }
}
