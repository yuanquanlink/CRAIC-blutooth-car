
// GM65 条码扫描器模块
// 使用软件串口与扫描器通信

#include <SoftwareSerial.h>

// 软件串口引脚定义
#define Software_Pin_Rx 17  // 接收引脚
#define Software_Pin_Tx 2   // 发送引脚

// 创建软件串口对象（用于与条码扫描器通信）
SoftwareSerial QRcode(Software_Pin_Rx, Software_Pin_Tx);

// 初始化条码扫描器
void gm_modele_init() {
  QRcode.begin(9600);  // 设置波特率为9600
  delay(50);
  while (QRcode.available()) QRcode.read();  // 清空接收缓冲区
  Serial.println(F("GM65 OK"));
}

// 读取条码数据
void gm65_read() {
  String receive_data = "";  // 用于存储接收到的数据
  while(1) {  // 循环等待数据
    while(QRcode.available()) {  // 如果有数据可读
      delay(10);  // 短暂延时，等待数据完整到达
      receive_data += (char)QRcode.read();  // 读取一个字符并添加到字符串
    }
    receive_data.trim();  // 去除首尾空白字符
    if(receive_data.length() > 0) {  // 如果接收到了数据
      Serial.print(F("gm65_data:"));
      Serial.println(receive_data);  // 通过串口打印条码数据
      oled_show_status(receive_data.c_str());  // 在OLED上显示条码
      receive_data = "";  // 清空字符串
      break;  // 退出循环
    }
  }
}
