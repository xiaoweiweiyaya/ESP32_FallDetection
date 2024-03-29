#pragma once
#include "driver/i2c.h"
#include "mpu6050.h"
#include "esp_log.h"
#include "esp_system.h"
#include "unity.h"


// extern FeatureData feature_data;

#define WINDOW_SIZE 5

#define INIT_SIZE   10

/*危险程度*/
#define POSTURE_NORMAL 0
#define POSTURE_DANGER 1
#define POSTURE_FALL 2

/*判断阈值*/
#define SVM_T1 1.20f  //危险
#define SVM_T2 0.90f  //警告
#define SVM_T3 0.75f  //正常
#define SVMW_T 47.3f  //角速度阈值
#define PITCH_T 45    //俯仰值
#define ROLL_T 28    //翻滚值
/*原始陀螺仪数据*/
typedef struct{
    float pitch,roll,yaw; //俯仰角 翻滚角 偏航角(暂时不用)
    float accx,accy,accz;
    float gyrox,gyroy,gyroz;
    float temp;
}PostureData;

/*特征数据*/
typedef struct{
    float pitch_f,roll_f,yaw_f;
    uint8_t PostureState;    //状态
    float SVM;  //合加速度
    float Omiga;    //合角速度
    float CV;   //变异指数
    float SD;   //标准方差
    float Elect;    //电量
    int CV_flag;
    unsigned long steps;

}FeatureData;

/**滑动窗口（循环队列）**/
typedef struct {
    PostureData filter_buffer[WINDOW_SIZE];
    uint8_t old_index;
    PostureData buffer_sum;
    PostureData buffer_average;
	uint8_t full_flag;
}CircularArray;


void i2c_bus_init(void);
void i2c_sensor_mpu6050_init(void);
void i2c_sensor_mpu6050_read(void);
void i2c_sensor_mpu6050_judge(void);
uint8_t posture_init(void);
void Slold_dataing_Mean_Filter_Calc(CircularArray*, PostureData);
CircularArray InitArray();
float Average(float *s, int );