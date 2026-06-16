
// OLED 显示模块
// 使用 SSD1306 128x64 OLED 显示屏

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED 参数定义
#define SCREEN_WIDTH 128   // 屏幕宽度
#define SCREEN_HEIGHT 64   // 屏幕高度
#define OLED_RESET    -1   // 复位引脚（-1表示不使用）

// 创建 OLED 显示对象
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 初始化 OLED 显示屏
void oled_init() {
  // 初始化显示，使用 I2C 地址 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED fail"));  // 初始化失败
    return;
  }
  display.clearDisplay();  // 清屏
  display.display();       // 更新显示
}

// 显示状态信息（普通字符串版本）
void oled_show_status(const char* s) {
  display.clearDisplay();       // 清屏
  display.setTextSize(1);       // 设置文字大小
  display.setTextColor(SSD1306_WHITE);  // 设置文字颜色
  display.setCursor(0, 0);      // 设置光标位置
  display.print(F("Status:"));  // 显示标题
  display.setCursor(0, 16);     // 设置内容光标位置
  display.print(s);             // 显示状态内容
  display.display();            // 更新显示
}

// 显示状态信息（Flash字符串版本，节省内存）
void oled_show_status(const __FlashStringHelper* s) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("Status:"));
  display.setCursor(0, 16);
  display.print(s);
  display.display();
}

// 显示机械臂状态（三个舵机的角度/爪子状态）
void oled_show_arm_status(int a, int b, int c) {
  char buf[20];  // 字符缓冲区
  // 将爪子脉冲宽度映射为0-100的百分比（700=0%, 1300=100%）
  int gripperPercent = map(c, 700, 1300, 0, 100);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  // 格式化显示：大臂角度、小臂角度、爪子百分比
  sprintf(buf, "U:%d L:%d G:%d%%", a, b, gripperPercent);
  display.print(buf);
  display.display();
}

// 显示初始化状态
void oled_show_init_status() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 28);
  display.print(F("Init..."));  // 显示 "正在初始化..."
  display.display();
}

// 显示当前速度
void oled_show_speed(int speed) {
  char buf[20];  // 字符缓冲区
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(F("Speed:"));  // 显示标题
  display.setCursor(0, 16);
  sprintf(buf, "%d", speed);   // 格式化速度值
  display.print(buf);          // 显示速度
  display.display();
}
