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

    // ��ָ���Ĵ��ڣ�CDC���貨����
    bool open(const std::string& port, int baudRate = 12000000);
    // �رմ���
    void close();

    /**
    * ����ƶ�
    * @param x ��ֵ����
    * @param y ��ֵ����
    * @return ErrorCode
    */
    bool mouseMove(int16_t x, int16_t y);

    /**
     * ����㷨�Ż��ƶ�
     * @param x ��ֵ����
     * @param y ��ֵ����
     * @param ms �ƶ�ʱ��
     * @return ErrorCode
     */
    bool mouseMoveAuto(int16_t x, int16_t y, int16_t ms);

    /**
     * ��갴������
     * @param code ����ֵ��1��� 2�Ҽ� 3�м� 4���1 5���2
     * @param value ����1 �ͷ�0
     * @return ErrorCode
     */
    bool  mouseButton(uint16_t code, uint16_t value);

    /**
     * ��갴�¶���ms���ͷ�
     * @param code ����ֵ�ο�HIDTABLE
     * @param ms ����
     * @return ErrorCode
     */
    bool tapMouseButton(uint16_t code, int16_t ms);

    /**
     * ���̰�������
     * @param code ����ֵ���ο�event-codes
     * @param value ����1 �ͷ�0
     * @return ErrorCode
     */
    bool keyboardButton(uint16_t code, uint16_t value);

    /**
     * ���̰������¶���ms���ͷ�
     * @param code ����ֵ���ο�event-codes
     * @param ms ����
     * @return ErrorCode
     */
    bool tapKeyboardButton(uint16_t code, int16_t ms);

    /**
     * ���flash������̼�����ģʽ
     * @return ErrorCode
     */
    bool Firmware_Update(void);


private:
    HANDLE hSerial; // ���ھ��
    std::string portName; // ��������
    bool isOpen; // �Ƿ��Ѵ�
    bool err;

    // д�����ݵ��豸
    bool write(const std::vector<uint8_t>& data);
    // ���豸��ȡ����
    bool read(std::vector<uint8_t>& buffer, size_t length);

    // ��������
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
