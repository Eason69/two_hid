#include <iostream>
#include <thread>
#include "SerialPort.h"

SerialPort::SerialPort() : hSerial(INVALID_HANDLE_VALUE), isOpen(false) {}

SerialPort::~SerialPort() {
    close();
}

bool SerialPort::open(const std::string& port, int baudRate) {
    if (isOpen) {
        std::cerr << "Serial port already opened!" << std::endl;
        return false;
    }

    portName = "\\\\.\\" + port; // 支持 COM10 及以上
    std::wstring wPortName(portName.begin(), portName.end());

    hSerial = CreateFile(
        wPortName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Unable to open serial port " << port << std::endl;
        return false;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error: Unable to get serial port state" << std::endl;
        close();
        return false;
    }

    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error: Unable to configure serial port" << std::endl;
        close();
        return false;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "Error: Unable to set timeouts for serial port" << std::endl;
        close();
        return false;
    }

    isOpen = true;
    std::cout << "Serial port " << port << " opened successfully!" << std::endl;
    return true;
}

void SerialPort::close() {
    if (isOpen) {
        CloseHandle(hSerial);
        isOpen = false;
        std::cout << "Serial port closed." << std::endl;
    }
}


bool SerialPort::mouseMove(int16_t x, int16_t y) {
    std::vector<uint8_t> data(sizeof(MouseData));

    mouseData.code = REL_X;
    mouseData.value1 = static_cast<uint8_t>(x >> 8);
    mouseData.value2 = static_cast<uint8_t>(x);
    std::memcpy(data.data(), &mouseData, sizeof(MouseData));
    err = write(data);

    mouseData.code = REL_Y;
    mouseData.value1 = static_cast<uint8_t>(y >> 8);
    mouseData.value2 = static_cast<uint8_t>(y);
    std::memcpy(data.data(), &mouseData, sizeof(MouseData));
    err = write(data);

    return err;
}

bool SerialPort::mouseMoveAuto(int16_t x, int16_t y, int16_t ms) {
    if (ms <= 0) return false;

    int steps = ms; // 每1ms更新一次，步数等于时间
    double startX = 0, startY = 0; // 起始位置
    double deltaX = static_cast<double>(x); // X方向的总移动距离
    double deltaY = static_cast<double>(y); // Y方向的总移动距离

    for (int i = 0; i <= steps; ++i) {
        double t = i; // 当前时间（以 ms 为单位）

        // 计算缓动后的位置
        double currentX = easeInOut(t, startX, deltaX, ms);
        double currentY = easeInOut(t, startY, deltaY, ms);

        // 计算需要移动的增量
        int16_t moveX = static_cast<int16_t>(std::round(currentX - startX));
        int16_t moveY = static_cast<int16_t>(std::round(currentY - startY));

        // 如果增量为 0，则跳过
        if (moveX == 0 && moveY == 0) continue;

        // 调用 mouseMove 实现实际的移动
        if (!mouseMove(moveX, moveY)) return false;

        // 更新起始位置
        startX += moveX;
        startY += moveY;

        // 延时 1ms
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return true;
}

bool SerialPort::mouseButton(uint16_t code, uint16_t value) {
    std::vector<uint8_t> data(sizeof(MouseData));
    mouseData.code = code;
    mouseData.value1 = static_cast<uint8_t>(value);
    std::memcpy(data.data(), &mouseData, sizeof(MouseData));
    err = write(data);
    return err;
}

bool SerialPort::tapMouseButton(uint16_t code, int16_t ms) {
    err = mouseButton(code, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    err = mouseButton(code, 0);
    return err;
}

bool SerialPort::keyboardButton(uint16_t code, uint16_t value) {
    std::vector<uint8_t> data(sizeof(KeyboardData));
    keyboardData.code = code;
    keyboardData.value = static_cast<uint8_t>(value);
    std::memcpy(data.data(), &keyboardData, sizeof(KeyboardData));
    err = write(data);
    return err;
}

bool SerialPort::tapKeyboardButton(uint16_t code, int16_t ms) {
    err = keyboardButton(code, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    err = keyboardButton(code, 0);
    return err;
}

bool SerialPort::Firmware_Update(void) {
    std::vector<uint8_t> data = { 0xAF, 0xFA, 0x02, 0x02, 0x00};
    err = write(data);
    return err;
}


bool SerialPort::write(const std::vector<uint8_t>& data) {
    if (!isOpen) {
        std::cerr << "Error: Serial port not opened!" << std::endl;
        return false;
    }

    DWORD bytesWritten;
    if (!WriteFile(hSerial, data.data(), data.size(), &bytesWritten, nullptr)) {
        std::cerr << "Error: Failed to send data." << std::endl;
        return false;
    }
    if (bytesWritten != data.size()) {
        std::cerr << "Warning: Partial data written." << std::endl;
    }
    return true;
}

bool SerialPort::read(std::vector<uint8_t>& buffer, size_t length) {
    if (!isOpen) {
        std::cerr << "Error: Serial port not opened!" << std::endl;
        return false;
    }

    // 临时缓冲区，用于存储读取的数据
    std::vector<uint8_t> tempBuffer(length);
    DWORD bytesRead = 0;

    if (!ReadFile(hSerial, tempBuffer.data(), static_cast<DWORD>(length), &bytesRead, nullptr)) {
        std::cerr << "Error: Failed to read data from serial port." << std::endl;
        return false;
    }

    if (bytesRead == 0) {
        std::cerr << "Warning: No data available to read." << std::endl;
        return false;
    }

    // 将实际读取的数据存入用户提供的 buffer
    buffer.assign(tempBuffer.begin(), tempBuffer.begin() + bytesRead);
    return true;
}

double SerialPort::easeInOut(double t, double b, double c, double d) {
    t /= d / 2.0;
    if (t < 1) return c / 2.0 * t * t + b;
    t--;
    return -c / 2.0 * (t * (t - 2) - 1) + b;
}
