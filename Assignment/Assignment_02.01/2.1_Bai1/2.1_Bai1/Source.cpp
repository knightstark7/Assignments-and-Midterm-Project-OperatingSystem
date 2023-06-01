#include <iostream>
#include <conio.h>
#include<dos.h>

using namespace std;

// địa chỉ bộ nhớ của bàn phím
#define KEYBOARD_ADDRESS 0x0040
#define KEYBOARD_STATUS 0x0061

// địa chỉ bộ nhớ của màn hình
#define SCREEN_ADDRESS 0xB800
#define SCREEN_ROWS 25



#define SCREEN_COLS 80

// định nghĩa các mã ASCII
#define ASCII_BACKSPACE 0x08
#define ASCII_ENTER 0x0D

int main() {
    char* keyboard_buffer = (char*)KEYBOARD_ADDRESS;
    char* screen_buffer = (char*)SCREEN_ADDRESS;
    int cursor_row = 0, cursor_col = 0;

    while (true) {
        // kiểm tra nếu bàn phím có ký tự mới
        if ((*keyboard_buffer & 0x01) == 1) {
            // đọc ký tự từ bàn phím
            char ch = *keyboard_buffer;

            // xóa cờ báo hiệu đã nhận ký tự
            *keyboard_buffer &= 0xFE;

            // xử lý ký tự đọc được
            switch (ch) {
            case ASCII_BACKSPACE:
                if (cursor_col > 0) {
                    cursor_col--;
                    screen_buffer[cursor_row * SCREEN_COLS + cursor_col] = ' ';
                }
                break;

            case ASCII_ENTER:
                cursor_row++;
                cursor_col = 0;
                break;

            default:
                screen_buffer[cursor_row * SCREEN_COLS + cursor_col] = ch;
                cursor_col++;
                break;
            }

            // kiểm tra nếu đến cuối màn hình thì cuộn trang
            if (cursor_col == SCREEN_COLS) {
                cursor_row++;
                cursor_col = 0;
            }

            if (cursor_row == SCREEN_ROWS) {
                cursor_row--;
                for (int i = 0; i < SCREEN_COLS * SCREEN_ROWS; i++) {
                    screen_buffer[i] = screen_buffer[i + SCREEN_COLS];
                }
                for (int i = 0; i < SCREEN_COLS; i++) {
                    screen_buffer[SCREEN_COLS * SCREEN_ROWS + i] = ' ';
                }
            }
            // cập nhật vị trí con trỏ
            int cursor_pos = cursor_row * SCREEN_COLS + cursor_col;
            /*outp(0x03D4, 0x0F);
            _outp(0x03D5, (unsigned char)(cursor_pos & 0xFF));
            _outp(0x03D4, 0x0E);
            _outp(0x03D5, (unsigned char)((cursor_pos >> 8) & 0xFF));*/
        }
    }
    return 0;
}