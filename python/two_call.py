import time
import signal
import sys
import twoHid
import event_code #按键头文件

hid = twoHid.SerialPort()

if hid.open('COM30',12000000): #设备管理器查看的串口号，波特率随意填
    print('设备打开成功')
else:
    print('设备打开失败')

# for i in range(100):
#     hid.mouseMove(1,1) #立即移动 x的距离 y的距离
#     time.sleep(0.001)

# hid.mouseMoveAuto(100, 100, 10) #自动移动 x的距离 y的距离 多少时间完成

# hid.mouseButton(event_code.BTN_LEFT, 1) #鼠标按键控制 
# time.sleep(0.01)
# hid.mouseButton(event_code.BTN_LEFT, 0)

# hid.tapMouseButton(event_code.BTN_LEFT, 50) #鼠标按键按下50ms后释放

# hid.keyboardButton(event_code.HID_A, 1)
# time.sleep(0.01)
# hid.keyboardButton(event_code.HID_A, 0)

# hid.tapKeyboardButton(event_code.HID_A, 50) #键盘按键按下50ms后释放

def signal_handler(sig, frame):
    print('捕获到 Ctrl+C,正在关闭...')
    hid.close()
    sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)







