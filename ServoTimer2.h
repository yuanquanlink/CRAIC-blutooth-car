
// ServoTimer2 库 - 使用定时器2控制舵机
// 避免与使用定时器1的Servo库冲突

#ifndef ServoTimer2_h
#define ServoTimer2_h

#include <inttypes.h>

// 舵机参数定义
#define MIN_PULSE_WIDTH       750   // 最小脉冲宽度（微秒）
#define MAX_PULSE_WIDTH      2250   // 最大脉冲宽度（微秒）
#define DEFAULT_PULSE_WIDTH  1500   // 默认脉冲宽度（中位）
#define FRAME_SYNC_PERIOD   20000   // 帧同步周期（20毫秒）
#define NBR_CHANNELS 8               // 最大通道数

// 舵机引脚结构体
typedef struct  {
      uint8_t nbr        :5 ;       // 引脚号（5位足够）
      uint8_t isActive   :1 ;       // 是否激活（1位）
   } ST2_ServoPin_t ;

// 舵机通道结构体
typedef struct {
  ST2_ServoPin_t Pin;    // 引脚信息
  byte counter;          // 计数器值
  byte remainder;        // 余数
}  ST2_servo_t;

// ServoTimer2 类
class ServoTimer2
{
  public:
	ServoTimer2();                    // 构造函数
	uint8_t attach(int);              // 连接舵机到指定引脚
	uint8_t attach(int, int, int);    // 连接舵机（带最小/最大脉冲宽度）
    void detach();                    // 断开舵机连接
    void write(int);                  // 设置脉冲宽度
    int read();                       // 读取当前脉冲宽度
	boolean attached();               // 检查是否已连接
  private:
	uint8_t chanIndex;                // 通道索引
};

#endif
