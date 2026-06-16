
// ServoTimer2 库实现文件
// 使用定时器2的溢出中断来控制舵机

extern "C" {
  #include <inttypes.h>
  #include <avr/interrupt.h>
}
#include <Arduino.h>
#include "ServoTimer2.h"

// 静态函数声明
static void initISR();                 // 初始化中断服务程序
static void writeChan(uint8_t chan, int pulsewidth);  // 写入通道

// 常数定义
#define FRAME_SYNC_INDEX   0          // 帧同步索引
#define FRAME_SYNC_PERIOD  20000      // 帧同步周期（20ms）
#define FRAME_SYNC_DELAY   ((FRAME_SYNC_PERIOD - ( NBR_CHANNELS * DEFAULT_PULSE_WIDTH))/ 128)
#define DELAY_ADJUST	 8            // 延时调整

// 全局变量
static ST2_servo_t servos[NBR_CHANNELS+1];  // 舵机通道数组（+1是为了帧同步）
static volatile uint8_t Channel;      // 当前通道
static volatile uint8_t ISRCount;     // 中断计数
uint8_t ChannelCount = 0;             // 已分配的通道数
static boolean isStarted = false;     // 是否已启动

// 定时器2溢出中断服务程序
ISR (TIMER2_OVF_vect)
{
  ++ISRCount;  // 增加中断计数
  if (ISRCount ==  servos[Channel].counter)
  {
	TCNT2 =  servos[Channel].remainder;  // 设置定时器初值
  }
  else if(ISRCount >  servos[Channel].counter)
  {
	// 如果通道激活，拉低引脚电平
	if(servos[Channel].Pin.isActive == true)
	    digitalWrite( servos[Channel].Pin.nbr,LOW);

	// 切换到下一个通道
	Channel++;
	ISRCount = 0;
	TCNT2 = 0;
	// 如果不是帧同步通道且通道号在范围内，拉高引脚电平
	if( (Channel != FRAME_SYNC_INDEX) && (Channel <= NBR_CHANNELS) ){
	    if(servos[Channel].Pin.isActive == true)
		 digitalWrite( servos[Channel].Pin.nbr,HIGH);
	}
	else if(Channel > NBR_CHANNELS){
	   Channel = 0;  // 回到帧同步通道
	}
   }
}

// 构造函数
ServoTimer2::ServoTimer2()
{
  // 分配通道号
  if( ChannelCount < NBR_CHANNELS)
	this->chanIndex = ++ChannelCount;
  else
	this->chanIndex = 0;  // 通道已满，返回0
}

// 连接舵机到引脚
uint8_t ServoTimer2::attach(int pin)
{
  // 如果还没启动，初始化中断
  if( isStarted == false)
	 initISR();
  // 如果有可用通道
  if(this->chanIndex > 0)
  {
	 pinMode( pin, OUTPUT);  // 设置引脚为输出模式
	 servos[this->chanIndex].Pin.nbr = pin;  // 保存引脚号
	 servos[this->chanIndex].Pin.isActive = true;  // 标记为激活
  }
  return this->chanIndex;  // 返回通道号
}

// 断开舵机连接
void ServoTimer2::detach()
{
  servos[this->chanIndex].Pin.isActive = false;  // 标记为非激活
}

// 设置舵机脉冲宽度
void ServoTimer2::write(int pulsewidth)
{
  writeChan(this->chanIndex, pulsewidth);
}

// 读取当前脉冲宽度
int ServoTimer2::read()
{
  unsigned int pulsewidth;
  if( this->chanIndex > 0)
	// 计算脉冲宽度
	pulsewidth =  servos[this->chanIndex].counter * 128 + ((255 - servos[this->chanIndex].remainder) / 2) + DELAY_ADJUST;
  else
	 pulsewidth  = 0;
  return pulsewidth;
}

// 检查舵机是否已连接
boolean ServoTimer2::attached()
{
  return servos[this->chanIndex].Pin.isActive;
}

// 写入指定通道的脉冲宽度
static void writeChan(uint8_t chan, int pulsewidth)
{
  if( (chan > 0) && (chan <= NBR_CHANNELS) )
  {
	// 限制脉冲宽度在有效范围内
	if( pulsewidth < MIN_PULSE_WIDTH )
	    pulsewidth = MIN_PULSE_WIDTH;
	else if( pulsewidth > MAX_PULSE_WIDTH )
	    pulsewidth = MAX_PULSE_WIDTH;

	 pulsewidth -= DELAY_ADJUST;  // 调整延时
	// 计算计数器值和余数
	servos[chan].counter = pulsewidth / 128;
	servos[chan].remainder = 255 - (2 * (pulsewidth - ( servos[chan].counter * 128)));
  }
}

// 初始化中断服务程序
static void initISR()
{
  // 初始化所有通道为默认脉冲宽度
  for(uint8_t i=1; i <= NBR_CHANNELS; i++) {
	 writeChan(i, DEFAULT_PULSE_WIDTH);
  }
  // 设置帧同步延时
  servos[FRAME_SYNC_INDEX].counter = FRAME_SYNC_DELAY;

  Channel = 0;       // 从帧同步通道开始
  ISRCount = 0;

  // 配置定时器2
  TIMSK2 = 0;        // 关闭所有中断
  TCCR2A = 0;        // 正常模式
  TCCR2B = _BV(CS21); // 预分频8
  TCNT2 = 0;         // 计数器清零
  TIFR2 = _BV(TOV2); // 清除溢出标志
  TIMSK2 =  _BV(TOIE2); // 启用溢出中断

  isStarted = true;  // 标记已启动
}
