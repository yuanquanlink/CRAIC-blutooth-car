
// 小车电机驱动模块
// 负责控制小车的前进、后退、转向等运动功能

// 电机引脚定义
#define MOTOR1_PIN1  5   // 电机1的引脚1（左电机正向）
#define MOTOR1_PIN2  6   // 电机1的引脚2（左电机反向）
#define MOTOR2_PIN1  9   // 电机2的引脚1（右电机正向）
#define MOTOR2_PIN2  10  // 电机2的引脚2（右电机反向）

// 速度参数设置
#define DEFAULT_SPEED     150  // 默认速度值
#define MAX_SPEED         255  // 最大速度（PWM值）
#define MIN_SPEED         80   // 最小速度
#define SPEED_STEP        20   // 速度调节步长
#define TURN_SPEED        180  // 转向时的速度
#define FULL_SPEED        250  // 全速前进

int currentSpeed = DEFAULT_SPEED;  // 当前速度变量

// 初始化小车电机
void car_init() {
  Serial.println(F("[CAR] Init"));
  // 设置电机引脚为输出模式
  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);
  Serial.println(F("[CAR] OK"));
}

// 停止所有电机（断电）
void stopMotors() {
  analogWrite(MOTOR1_PIN1, 0);
  analogWrite(MOTOR1_PIN2, 0);
  analogWrite(MOTOR2_PIN1, 0);
  analogWrite(MOTOR2_PIN2, 0);
}

// 刹车（短时间接通正反向，使电机快速停止）
void brakeMotors() {
  analogWrite(MOTOR1_PIN1, 255);
  analogWrite(MOTOR1_PIN2, 255);
  analogWrite(MOTOR2_PIN1, 255);
  analogWrite(MOTOR2_PIN2, 255);
  delay(100);  // 刹车持续100毫秒
  stopMotors();  // 刹车结束后停止电机
}

// 前进
void forward() {
  analogWrite(MOTOR1_PIN1, currentSpeed);  // 左电机正转
  analogWrite(MOTOR1_PIN2, 0);
  analogWrite(MOTOR2_PIN1, currentSpeed);  // 右电机正转
  analogWrite(MOTOR2_PIN2, 0);
}

// 后退
void backward() {
  analogWrite(MOTOR1_PIN1, 0);
  analogWrite(MOTOR1_PIN2, currentSpeed);  // 左电机反转
  analogWrite(MOTOR2_PIN1, 0);
  analogWrite(MOTOR2_PIN2, currentSpeed);  // 右电机反转
}

// 原地左转（左电机反转，右电机正转）
void turnLeft() {
  analogWrite(MOTOR1_PIN1, 0);
  analogWrite(MOTOR1_PIN2, TURN_SPEED);
  analogWrite(MOTOR2_PIN1, TURN_SPEED);
  analogWrite(MOTOR2_PIN2, 0);
}

// 原地右转（左电机正转，右电机反转）
void turnRight() {
  analogWrite(MOTOR1_PIN1, TURN_SPEED);
  analogWrite(MOTOR1_PIN2, 0);
  analogWrite(MOTOR2_PIN1, 0);
  analogWrite(MOTOR2_PIN2, TURN_SPEED);
}

// 轻微左转（只开右电机）
void turnSlightLeft() {
  analogWrite(MOTOR1_PIN1, 0);
  analogWrite(MOTOR1_PIN2, 0);
  analogWrite(MOTOR2_PIN1, currentSpeed);  // 只有右电机转动
  analogWrite(MOTOR2_PIN2, 0);
}

// 轻微右转（只开左电机）
void turnSlightRight() {
  analogWrite(MOTOR1_PIN1, currentSpeed);  // 只有左电机转动
  analogWrite(MOTOR1_PIN2, 0);
  analogWrite(MOTOR2_PIN1, 0);
  analogWrite(MOTOR2_PIN2, 0);
}

// 停止小车
void stop() {
  stopMotors();
}

// 加速
void speedUp() {
  currentSpeed = constrain(currentSpeed + SPEED_STEP, MIN_SPEED, MAX_SPEED);
}

// 减速
void speedDown() {
  currentSpeed = constrain(currentSpeed - SPEED_STEP, MIN_SPEED, MAX_SPEED);
}

// 设置为最大速度
void speedMax() {
  currentSpeed = FULL_SPEED;
}

// 获取当前速度
int getSpeed() {
  return currentSpeed;
}
