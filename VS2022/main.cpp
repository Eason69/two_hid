#include <iostream>
#include <vector>
#include "3rdparty/SerialPort.h"

int main()
{
    SerialPort serial;

    // 打开串口
    if (!serial.open("COM25")) {
        std::cerr << "打开失败" << std::endl;
        return -1;
    }

    //for (int i = 0; i < 1000; ++i) {
    //    serial.mouseMove(1, 1);
    //    Sleep(0.1);
    //}

    //serial.mouseMoveAuto(100, 100, 100);

    //serial.mouseButton(BTN_LEFT, 1);
    //Sleep(0.05);
    //serial.mouseButton(BTN_LEFT, 0);

    //serial.tapMouseButton(BTN_LEFT, 50);

    //serial.keyboardButton(HID_A, 1);
    //Sleep(0.05);
    //serial.keyboardButton(HID_A, 0);

    //serial.tapKeyboardButton(HID_A, 50);

    serial.close();

    return 0;
}