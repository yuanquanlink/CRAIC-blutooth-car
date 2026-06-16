
// 蓝牙遥控小车主程序
// 整合了小车运动、机械臂、OLED显示和条码扫描功能

#include <Arduino.h>
#include "ServoTimer2.h"
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 外部变量声明（来自 arm.ino）
extern int servo1Angle, servo2Angle, servo3Angle;

unsigned long lastCmdTime = 0;          // 最后命令接收时间
const unsigned long HEARTBEAT_TIMEOUT = 2000;  // 心跳超时时间（2秒）

// 函数声明
void oled_init();                      // OLED初始化
void oled_show_status(const __FlashStringHelper* s);  // 显示状态（Flash字符串）
void oled_show_status(const char* s);  // 显示状态（普通字符串）
void oled_show_arm_status(int a, int b, int c);  // 显示机械臂状态
void oled_show_speed(int speed);       // 显示速度
void oled_show_init_status();          // 显示初始化状态

void car_init();                       // 小车初始化
void forward();                        // 前进
void backward();                       // 后退
void turnLeft();                       // 左转
void turnRight();                      // 右转
void turnSlightLeft();                 // 轻微左转
void turnSlightRight();                // 轻微右转
void stop();                           // 停止
void brakeMotors();                    // 刹车
void speedUp();                        // 加速
void speedDown();                      // 减速
void speedMax();                       // 全速
int getSpeed();                        // 获取速度

void arm_init();                       // 机械臂初始化
void arm_reset();                      // 机械臂复位
void armSetPositionA();                // 机械臂到位置A
void armSetPositionB();                // 机械臂到位置B
void gripperOpen();                    // 打开爪子
void gripperClose();                   // 闭合爪子
void upperArmUp();                     // 大臂升
void upperArmDown();                   // 大臂降
void lowerArmUp();                     // 小臂升
void lowerArmDown();                   // 小臂降

void gm_modele_init();                 // 条码扫描器初始化
void gm65_read();                      // 读取条码

// Arduino setup 函数，程序启动时执行一次
void setup() {
  Serial.begin(9600);  // 初始化串口通信，波特率9600
  delay(100);
  while (Serial.available()) Serial.read();  // 清空串口缓冲区

  // 初始化各个模块
  oled_init();
  car_init();
  arm_init();
  gm_modele_init();
  oled_show_init_status();

  Serial.println(F("Ready"));  // 打印准备就绪
  stop();                      // 停止小车
  lastCmdTime = millis();      // 记录当前时间
}

// Arduino loop 函数，循环执行
void loop() {
  processCommand();    // 处理串口命令
  checkHeartbeat();    // 检查心跳超时
  delay(10);           // 短暂延时
}

// 处理串口接收的命令
void processCommand() {
  if (Serial.available() <= 0) return;  // 没有数据则返回

  char cmd = Serial.read();  // 读取一个字符命令
  lastCmdTime = millis();    // 更新最后命令时间

  // 根据命令执行相应操作
  switch (cmd) {
    case 'F': forward(); oled_show_status(F("Fwd")); break;        // 前进
    case 'B': backward(); oled_show_status(F("Rev")); break;       // 后退
    case 'L': turnLeft(); oled_show_status(F("Left")); break;      // 左转
    case 'R': turnRight(); oled_show_status(F("Right")); break;    // 右转
    case 'J': turnSlightLeft(); oled_show_status(F("SLeft")); break;  // 轻微左转
    case 'K': turnSlightRight(); oled_show_status(F("SRight")); break;// 轻微右转
    case 'S': stop(); oled_show_status(F("Stop")); break;          // 停止
    case 'E': brakeMotors(); oled_show_status(F("Brk")); break;    // 刹车

    case 'W': handleSpeedChange(1); break;   // 加速
    case 'Z': handleSpeedChange(-1); break;  // 减速
    case 'G': handleSpeedChange(0); break;   // 全速

    case 'O': gripperOpen(); showArmStatus(); break;    // 打开爪子
    case 'C': gripperClose(); showArmStatus(); break;   // 闭合爪子
    case 'H': arm_reset(); oled_show_status(F("Home")); break;  // 机械臂复位
    case '1': armSetPositionA(); showArmStatus(); break; // 位置A
    case '2': armSetPositionB(); showArmStatus(); break; // 位置B
    case 'U': upperArmUp(); showArmStatus(); break;      // 大臂升
    case 'D': upperArmDown(); showArmStatus(); break;    // 大臂降
    case 'V': lowerArmUp(); showArmStatus(); break;      // 小臂升
    case 'N': lowerArmDown(); showArmStatus(); break;    // 小臂降

    case 'Q': oled_show_status(F("Scan")); gm65_read(); break;  // 扫描条码
    case 'I': showSystemInfo(); break;    // 显示系统信息
    case '?': showHelp(); break;          // 显示帮助
  }

  clearSerialBuffer();  // 清空串口缓冲区
}

// 处理速度变化
void handleSpeedChange(int mode) {
  if (mode > 0) speedUp();        // mode>0 加速
  else if (mode < 0) speedDown(); // mode<0 减速
  else speedMax();                // mode=0 全速

  oled_show_speed(getSpeed());    // OLED显示当前速度
  Serial.print(F("Speed: "));
  Serial.println(getSpeed());
  if (mode == 0) Serial.println(F("MAX!"));
}

// 显示机械臂状态
void showArmStatus() {
  oled_show_arm_status(servo1Angle, servo2Angle, servo3Angle);
}

// 显示系统信息（机械臂角度和速度）
void showSystemInfo() {
  // 将爪子脉冲宽度映射为0-100的百分比
  int gripperPercent = map(servo3Angle, 700, 2300, 0, 100);
  Serial.print(F("Arm: ")); Serial.print(servo1Angle);
  Serial.print(F("/")); Serial.print(servo2Angle);
  Serial.print(F("/")); Serial.print(gripperPercent); Serial.println(F("%"));
  Serial.print(F("Speed: ")); Serial.println(getSpeed());
}

// 显示帮助信息
void showHelp() {
  Serial.println(F("F/B/L/R/J/K/S/E | W/Z/G | O/C/H/1/2/U/D/V/N | Q/I/?"));
  Serial.println(F("U=大臂升 D=大臂降 V=小臂升 N=小臂降"));
}

// 清空串口缓冲区
void clearSerialBuffer() {
  while (Serial.available()) Serial.read();
}

// 检查心跳超时，如果超时则停止小车
void checkHeartbeat() {
  if (millis() - lastCmdTime > HEARTBEAT_TIMEOUT) {
    stop();
  }
}
