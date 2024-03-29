#include <stdio.h>
#include "posture.h"
#include <string.h>
#define I2C_MASTER_SCL_IO 19      /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 20      /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_1  /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000 /*!< I2C master clock frequency */

static const char *TAG_P = "POSTURE";

static mpu6050_handle_t mpu6050 = NULL;

CircularArray slide_Window={0}; //滑动窗口
PostureData posture_data={0}; //全局姿态数据
FeatureData feature_data = {0}; //全局特征
/*初始化I2C总线*/
void i2c_bus_init(void)
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    
    conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

    esp_err_t ret = i2c_param_config(I2C_MASTER_NUM, &conf);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, ret, "I2C config returned error");

    ret = i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    TEST_ASSERT_EQUAL_MESSAGE(ESP_OK, ret, "I2C install returned error");
}

/*初始化MPU6050传感器*/
void i2c_sensor_mpu6050_init(void)
{
    esp_err_t ret;

    i2c_bus_init();
    mpu6050 = mpu6050_create(I2C_MASTER_NUM, MPU6050_I2C_ADDRESS);
    TEST_ASSERT_NOT_NULL_MESSAGE(mpu6050, "MPU6050 create returned NULL");

    ret = mpu6050_config(mpu6050, ACCE_FS_4G, GYRO_FS_500DPS);
    TEST_ASSERT_EQUAL(ESP_OK, ret);

    ret = mpu6050_wake_up(mpu6050);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
}

/*获取mpu6050数据*/
void i2c_sensor_mpu6050_read(void)
{
    esp_err_t ret;
    uint8_t mpu6050_deviceid;
    mpu6050_acce_value_t acce;
    mpu6050_gyro_value_t gyro;
    mpu6050_temp_value_t temp;
    complimentary_angle_t angle;

    PostureData pd;

    ret = mpu6050_get_deviceid(mpu6050, &mpu6050_deviceid);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(MPU6050_WHO_AM_I_VAL, mpu6050_deviceid, "Who Am I register does not contain expected data");

     /* code */
    ret = mpu6050_get_acce(mpu6050, &acce);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    ESP_LOGI(TAG_P, "acce_x:%.2f, acce_y:%.2f, acce_z:%.2f\n", acce.acce_x, acce.acce_y, acce.acce_z);

    ret = mpu6050_get_gyro(mpu6050, &gyro);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    ESP_LOGI(TAG_P, "gyro_x:%.2f, gyro_y:%.2f, gyro_z:%.2f\n", gyro.gyro_x, gyro.gyro_y, gyro.gyro_z);

    ret = mpu6050_get_temp(mpu6050, &temp);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    ESP_LOGI(TAG_P, "t:%.2f \n", temp.temp);

    ret = mpu6050_complimentory_filter(mpu6050, &acce,&gyro,&angle);
    TEST_ASSERT_EQUAL(ESP_OK, ret);
    ESP_LOGI(TAG_P, "pitch:%.2f roll:%.2f \n", angle.pitch,angle.roll);

    pd.accx = acce.acce_x;
    pd.accy = acce.acce_y;
    pd.accz = acce.acce_z;
    pd.gyrox = gyro.gyro_x;
    pd.gyroy = gyro.gyro_y;
    pd.gyroz = gyro.gyro_z;
    pd.temp = temp.temp;
    
    //平滑滤波
    Slold_dataing_Mean_Filter_Calc(&slide_Window,pd);
    if(slide_Window.full_flag == 0)
    {
        posture_data = pd;
    } 
    else{
        //取平均值
        posture_data = slide_Window.buffer_average;
    }

}

//姿态判断
void i2c_sensor_mpu6050_judge(void)
{
    static uint8_t T1 = 0;
    static uint8_t T2 = 0;
    //计算特征值
    feature_data.SVM = sqrt(pow(posture_data.accx,2)+pow(posture_data.accy,2)+pow(posture_data.accz,2));
    //T1判断
    if(feature_data.SVM > SVM_T2){
        T1++;
        if(feature_data.SVM > SVM_T1){
            T2++;
        }
    }else{
        feature_data.PostureState = POSTURE_NORMAL;
    }

    //T2判断
    if(T1>=3 && T2 >= 2 )
    {
        feature_data.PostureState = POSTURE_DANGER;
        //计算特征值
        feature_data.Omiga = sqrt(pow(posture_data.gyrox,2)+pow(posture_data.gyroy,2)+pow(posture_data.gyroz,2));
        if(feature_data.Omiga > SVMW_T){
            if((fabs(posture_data.pitch - feature_data.pitch_f)>PITCH_T)||(fabs(posture_data.roll - feature_data.roll_f)>ROLL_T)) {
                feature_data.PostureState = POSTURE_FALL;
                feature_data.SVM = 0;
                }
            else feature_data.PostureState = POSTURE_NORMAL;
        }
        else
        {
            feature_data.PostureState = POSTURE_NORMAL;
        }
    }
}


//滑动平均值滤波器
void Slold_dataing_Mean_Filter_Calc(CircularArray *filter, PostureData Data)
{	
	//减去最旧的数
	filter->buffer_sum.accx -= filter->filter_buffer[filter->old_index].accx;
	filter->buffer_sum.accy -= filter->filter_buffer[filter->old_index].accy;
	filter->buffer_sum.accz -= filter->filter_buffer[filter->old_index].accz;
	filter->buffer_sum.pitch -= filter->filter_buffer[filter->old_index].pitch;
	filter->buffer_sum.roll -= filter->filter_buffer[filter->old_index].roll;
	filter->buffer_sum.yaw -= filter->filter_buffer[filter->old_index].yaw;
	filter->buffer_sum.gyrox -= filter->filter_buffer[filter->old_index].gyrox;
	filter->buffer_sum.gyroy -= filter->filter_buffer[filter->old_index].gyroy;
	filter->buffer_sum.gyroz -= filter->filter_buffer[filter->old_index].gyroz;
    filter->buffer_sum.temp -= filter->filter_buffer[filter->old_index].temp;
	//加进最新的数
	filter->buffer_sum.accx += Data.accx;
	filter->buffer_sum.accy += Data.accy;
	filter->buffer_sum.accz += Data.accz;
	filter->buffer_sum.pitch += Data.pitch;
	filter->buffer_sum.roll += Data.roll;
	filter->buffer_sum.yaw += Data.yaw;
	filter->buffer_sum.gyrox += Data.gyrox;
	filter->buffer_sum.gyroy += Data.gyroy;
	filter->buffer_sum.gyroz += Data.gyroz;
    filter->buffer_sum.temp += Data.temp;
	//将最新的数覆盖最旧的数
	filter->filter_buffer[filter->old_index] = Data;
	//求均值
	filter->buffer_average.accx = filter->buffer_sum.accx / WINDOW_SIZE;
	filter->buffer_average.accy = filter->buffer_sum.accy / WINDOW_SIZE;
	filter->buffer_average.accz = filter->buffer_sum.accz / WINDOW_SIZE;
	filter->buffer_average.pitch = filter->buffer_sum.pitch / WINDOW_SIZE;
	filter->buffer_average.roll = filter->buffer_sum.roll / WINDOW_SIZE;
	filter->buffer_average.yaw = filter->buffer_sum.yaw / WINDOW_SIZE;
	filter->buffer_average.gyrox = filter->buffer_sum.gyrox / WINDOW_SIZE;
	filter->buffer_average.gyroy = filter->buffer_sum.gyroy / WINDOW_SIZE;
	filter->buffer_average.gyroz = filter->buffer_sum.gyroz / WINDOW_SIZE;
    filter->buffer_average.temp = filter->buffer_sum.temp / WINDOW_SIZE;
	
	if(++(filter->old_index) == WINDOW_SIZE){
		filter->old_index = 0;
		filter->full_flag = 1;
	}
}

/*获取初始姿态角*/
uint8_t posture_init(void)
{
    mpu6050_acce_value_t acce;
    mpu6050_gyro_value_t gyro;
    complimentary_angle_t angle;
    float pitch_arr[INIT_SIZE] = {0},roll_arr[INIT_SIZE] = {0};
    slide_Window = InitArray();
    for(int i=0; i<INIT_SIZE; i++)
    {
        mpu6050_complimentory_filter(mpu6050, &acce,&gyro,&angle);
        pitch_arr[i] = angle.pitch;
        roll_arr[i] = angle.roll;
    }
    feature_data.pitch_f = Average(pitch_arr,INIT_SIZE);
    feature_data.roll_f = Average(roll_arr,INIT_SIZE);
    feature_data.yaw_f = 0;
    if(feature_data.pitch_f == 0||feature_data.roll_f == 0) return 1;
    return 0;//OK
}

/*初始化循环队列*/
CircularArray InitArray() {
	CircularArray arr;
	for (int i = 0; i < WINDOW_SIZE; i++){
		memset(&arr.filter_buffer[i], 0, sizeof(arr.filter_buffer[i]));
	}
	memset(&arr.buffer_sum, 0, sizeof(arr.buffer_sum));
	memset(&arr.buffer_average, 0, sizeof(arr.buffer_average));
    arr.old_index = 0;
	arr.full_flag = 0;
	return arr;
}

/*平均值*/
float Average(float s[], int n)
{
    if (n == 0)
        return 0;
    float sum = 0;
    for (int i = 0; i < n; i++)
        sum += s[i]; //求和
    return sum / n;
}