
// 机械臂控制模块
// 控制三个舵机实现机械臂的运动和爪子的开合

#include "ServoTimer2.h"  // 使用 ServoTimer2 库控制舵机

// 舵机引脚定义
#define SERVO1_PIN  3  // 舵机1引脚（上臂）
#define SERVO2_PIN  4  // 舵机2引脚（下臂）
#define SERVO3_PIN  8  // 舵机3引脚（爪子）

// 舵机角度限制和初始值
#define SERVO3_MIN_PULSE 700  // 爪子打开的脉冲宽度（微秒）
#define SERVO3_MAX_PULSE 1300 // 爪子闭合的脉冲宽度（微秒）
#define SERVO1_MIN_ANGLE 0    // 大臂最小角度
#define SERVO1_MAX_ANGLE 270  // 大臂最大角度
#define SERVO2_MIN_ANGLE 0    // 小臂最小角度
#define SERVO2_MAX_ANGLE 225  // 小臂最大角度（因为有45度补偿）
#define SERVO1_INIT  110      // 舵机1初始角度
#define SERVO2_INIT  35       // 舵机2初始角度
#define SERVO3_INIT  1000     // 舵机3初始脉冲宽度（微秒）
#define ANGLE_STEP 10         // 每次调节的角度步长

// 舵机角度变量
int servo1Angle = SERVO1_INIT, servo2Angle = SERVO2_INIT, servo3Angle = SERVO3_INIT;
ServoTimer2 servo1, servo2, servo3;  // 三个舵机对象

// 将角度值（0-270度）映射为舵机脉冲宽度（750-2250微秒）
int mapToRange(int value, int max_angle=270) {
  return map(value, 0, max_angle, 750, 2250);
}

// 舵机2角度补偿（增加45度）
int mapServo2Angle(int a) {
  return a + 45;
}

// 初始化机械臂
void arm_init() {
  Serial.println(F("[ARM] Init"));
  // 连接舵机到引脚
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);

  // 设置初始角度
  int initAngle1 = mapToRange(servo1Angle);
  int initAngle2 = mapToRange(mapServo2Angle(servo2Angle));

  // 移动到初始位置
  servo1.write(initAngle1);
  servo2.write(initAngle2);
  servo3.write(servo3Angle);  // 爪子直接使用脉冲宽度
  delay(800);  // 等待舵机到达位置
  Serial.println(F("[ARM] OK"));
}

// 机械臂复位到初始位置
void arm_reset() {
  servo1Angle = SERVO1_INIT;
  servo2Angle = SERVO2_INIT;
  servo3Angle = SERVO3_INIT;

  int angle1 = mapToRange(servo1Angle);
  int angle2 = mapToRange(mapServo2Angle(servo2Angle));

  servo1.write(angle1);
  servo2.write(angle2);
  servo3.write(servo3Angle);  // 爪子直接使用脉冲宽度
  delay(800);
}

// 设置机械臂到位置A（抓取前的预备位置）
void armSetPositionA() {
  servo1Angle = 130;
  servo2Angle = 110;

  int angle1 = mapToRange(servo1Angle);
  int angle2 = mapToRange(mapServo2Angle(servo2Angle));

  servo1.write(angle1);
  servo2.write(angle2);  
}

// 设置机械臂到位置B（抬起位置）
void armSetPositionB() {
  servo1Angle = 200;
  servo2Angle = 175;

  int angle1 = mapToRange(servo1Angle);
  int angle2 = mapToRange(mapServo2Angle(servo2Angle));

  servo1.write(angle1);
  servo2.write(angle2);
}

// 打开爪子
void gripperOpen() {
  servo3Angle = SERVO3_MIN_PULSE;
  servo3.write(servo3Angle);
}

// 闭合爪子
void gripperClose() {
  servo3Angle = SERVO3_MAX_PULSE;
  servo3.write(servo3Angle);
}

// 大臂升（增加角度）
void upperArmUp() {
  servo1Angle = constrain(servo1Angle + ANGLE_STEP, SERVO1_MIN_ANGLE, SERVO1_MAX_ANGLE);
  int angle1 = mapToRange(servo1Angle);
  servo1.write(angle1);
}

// 大臂降（减少角度）
void upperArmDown() {
  servo1Angle = constrain(servo1Angle - ANGLE_STEP, SERVO1_MIN_ANGLE, SERVO1_MAX_ANGLE);
  int angle1 = mapToRange(servo1Angle);
  servo1.write(angle1);
}

// 小臂升（增加角度）
void lowerArmUp() {
  servo2Angle = constrain(servo2Angle + ANGLE_STEP, SERVO2_MIN_ANGLE, SERVO2_MAX_ANGLE);
  int angle2 = mapToRange(mapServo2Angle(servo2Angle));
  servo2.write(angle2);
}

// 小臂降（减少角度）
void lowerArmDown() {
  servo2Angle = constrain(servo2Angle - ANGLE_STEP, SERVO2_MIN_ANGLE, SERVO2_MAX_ANGLE);
  int angle2 = mapToRange(mapServo2Angle(servo2Angle));
  servo2.write(angle2);
}
