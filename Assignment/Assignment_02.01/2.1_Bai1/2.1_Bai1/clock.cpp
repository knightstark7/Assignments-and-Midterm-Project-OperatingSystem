#include <iostream>
#include <conio.h>
#include <windows.h>
#include <dos.h>

using namespace std;

void interrupt(*oldtimer) (...);
volatile unsigned long tickcount = 0;
volatile unsigned long oldtickcount = 0;

void interrupt newtimer(...) {
    tickcount++;
    if (tickcount - oldtickcount > 18) {
        oldtickcount = tickcount;
        cout << "\r" << "Current time: " << setw(2) << setfill('0') << hour << ":" << setw(2) << setfill('0') << minute << ":" << setw(2) << setfill('0') << second;
    }
    (*oldtimer)();
}

int main() {
    _disable(); // temporarily disable interrupts
    oldtimer = getvect(0x08); // save old timer interrupt vector
    setvect(0x08, newtimer); // set new timer interrupt vector
    _enable(); // enable interrupts

    while (true) {
        if (kbhit()) {
            char c = getch();
            if (c == 'q') break;
        }
    }

    _disable(); // temporarily disable interrupts
    setvect(0x08, oldtimer); // restore old timer interrupt vector
    _enable(); // enable interrupts

    return 0;
}

void interrupt(*oldtimer) (...)
{
}
