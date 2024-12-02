#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <windows.h>
#include <string>
#include <vector>
#include <cstdint>
#include "HidTable.h"


class SerialPort {
public:
    SerialPort();
    ~SerialPort();

    // 打开指定的串口，CDC无需波特率
    bool open(const std::string& port, int baudRate = 12000000);
    // 关闭串口
    void close();

    /**
    * 鼠标移动
    * @param x 正值向右
    * @param y 正值向下
    * @return ErrorCode
    */
    bool mouseMove(int16_t x, int16_t y);

    /**
     * 鼠标算法优化移动
     * @param x 正值向下
     * @param y 正值向右
     * @param ms 移动时间
     * @return ErrorCode
     */
    bool mouseMoveAuto(int16_t x, int16_t y, int16_t ms);

    /**
     * 鼠标按键触发
     * @param code 按键值，1左键 2右键 3中键 4侧键1 5侧键2
     * @param value 按下1 释放0
     * @return ErrorCode
     */
    bool  mouseButton(uint16_t code, uint16_t value);

    /**
     * 鼠标按下多少ms后释放
     * @param code 按键值参考HIDTABLE
     * @param ms 毫秒
     * @return ErrorCode
     */
    bool tapMouseButton(uint16_t code, int16_t ms);

    /**
     * 键盘按键触发
     * @param code 按键值，参考event-codes
     * @param value 按下1 释放0
     * @return ErrorCode
     */
    bool keyboardButton(uint16_t code, uint16_t value);

    /**
     * 键盘按键按下多少ms后释放
     * @param code 按键值，参考event-codes
     * @param ms 毫秒
     * @return ErrorCode
     */
    bool tapKeyboardButton(uint16_t code, int16_t ms);

    /**
     * 清空flash，进入固件更新模式
     * @return ErrorCode
     */
    bool Firmware_Update(void);


private:
    HANDLE hSerial; // 串口句柄
    std::string portName; // 串口名称
    bool isOpen; // 是否已打开
    bool err;

    // 写入数据到设备
    bool write(const std::vector<uint8_t>& data);
    // 从设备读取数据
    bool read(std::vector<uint8_t>& buffer, size_t length);

    // 缓动函数
    double easeInOut(double t, double b, double c, double d);

private:
    struct MouseData {
        uint8_t fdr[4];
        uint8_t code;
        uint8_t value1;
        uint8_t value2;
    };

    MouseData mouseData = {
            {0xAF, 0xFA, 0x04, 0x01},
            0x00,
            0x00,
            0x00
    };

    struct KeyboardData {
        uint8_t fdr[4];
        uint8_t code;
        uint8_t value;
    };

    KeyboardData keyboardData = {
            {0xAF, 0xFA, 0x03, 0x00},
            0x00,
            0x00
    };

};

#endif // SERIALPORT_H
