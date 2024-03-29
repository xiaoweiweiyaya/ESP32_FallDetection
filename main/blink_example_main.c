/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "led_blink.h"
#include "mpu6050.h"
#include "driver/i2c.h"
#include "posture.h"
#include "unity.h"
#include "driver/gpio.h"
#include "oled.h"
#include "mqtt.h"
#include "wifi.h"
#include "ATGM336H.h"
#include "alarm.h"
// static const char *TAG = "fall";
extern FeatureData feature_data;
extern PostureData posture_data;

/*GPS information*/
char latitude_str[10]; //经度
char longitude_str[10]; //纬度
char speed_str[20]; //速度
char time_str[10]; //时间
/////////////////////////任务设置///////////////////////////////////
#define MAIN_TASK_PRIO		3//任务优先级
#define MAIN_STK_SIZE 		4096  //任务堆栈大小
TaskHandle_t MainTask_Handler;//任务句柄
void main_task(void *pvParameters);//任务函数

#define LED_TASK_PRIO		1//任务优先级
#define LED_STK_SIZE 	    1024  //任务堆栈大小
TaskHandle_t LedTask_Handler;//任务句柄
void led_task(LedState *pvParameters);//任务函数

#define OLED_TASK_PRIO		3
#define OLED_STK_SIZE 	    4096  
TaskHandle_t OledTask_Handler;
void oled_task(void *pvParameters);

#define MQTT_TASK_PRIO		1
#define MQTT_STK_SIZE 	    4096  
TaskHandle_t MqttTask_Handler;
void mqtt_task(void *pvParameters);

#define GPS_TASK_PRIO		1
#define GPS_STK_SIZE 	    1024  
TaskHandle_t GpsTask_Handler;
void gps_task(void *pvParameters);

void wifi_connect(void);

void led_task(LedState *lds)
{
    /* Configure the peripheral according to the LED type */
    configure_led();

    while (1) {
        // ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        blink_led(lds);
        /* Toggle the LED state */
        if(lds->blink)
        {
            s_led_state = !s_led_state;
            vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        }
         
    }
}

/*主任务获取mpu6050数据*/
void main_task(void *pvParameters)
{
    i2c_sensor_mpu6050_init();
    posture_init();
    
    while(1)
    {
        i2c_sensor_mpu6050_read();
        i2c_sensor_mpu6050_judge();
        if(feature_data.PostureState == POSTURE_FALL)
        {
            ESP_LOGI(TAG, "Fall Happening");
            // syn6288_speak("警报！警报！有人发生跌倒！");
            LedState lds = {250,10,10,true};
            led_task(&lds);
        }
        // lds = {0,100,0,true};
        // LedState lds = {0,200,0,false};
        vTaskDelay(80);
        
    }
}

/*实时显示数据到oled--12864*/
void oled_task(void *pvParameters)
{
    I2C_Oled_Init();
    // OLED_GPIO_Init();
    OLED_Init();
    OLED_Clear();
    while(1)
    {
        OLED_ShowString(0, 1, (u8 *)"fall state:");OLED_ShowString(100, 1,feature_data.PostureState == 2? (u8 *)"Y":(u8 *)"N");
        OLED_ShowString(0, 3, (u8 *)"temperature:");//OLED_ShowNum(100, 3, posture_data.temp, 3,1);
        OLED_ShowNumss(96, 3, posture_data.temp);
        OLED_ShowString(0, 5, (u8 *)"gx:");OLED_ShowNum(25, 5, posture_data.gyrox, 2,1);
        OLED_ShowString(40, 5, (u8 *)"gy:");OLED_ShowNum(65, 5, posture_data.gyroy, 2,1);
        OLED_ShowString(80, 5, (u8 *)"gz:");OLED_ShowNum(105, 5, posture_data.gyroz, 2,1);
        delay_ms(100);
    }
    

}
/*mqtt数据上传任务*/
void mqtt_task(void *pvParameters)
{
    /*连接wifi*/
    wifi_connect();
    esp_mqtt_client_handle_t client = mqtt_app_start();
    const char* mqtt_publish_fall_state;
    const char* mqtt_publish_temperature;
    char temperature[5];
    snprintf(temperature, sizeof(temperature), "%.1f", posture_data.temp); //float -> string
    while(1)
    {
       mqtt_publish_fall_state = feature_data.PostureState==2?"Fall":"No Fall"; 
       mqtt_publish_temperature = temperature;
       esp_mqtt_client_publish(client, EMQXPublishTopic_user_state, mqtt_publish_fall_state, strlen(mqtt_publish_fall_state), 1, 0); //发布跌倒状态信息
       esp_mqtt_client_publish(client, EMQXPublishTopic_user_temp, mqtt_publish_temperature, strlen(mqtt_publish_temperature), 0, 0);//发布温度信息
       esp_mqtt_client_publish(client, EMQXPublishTopic_user_latitude, latitude_str, strlen(latitude_str), 0, 0);//发布经度信息
       esp_mqtt_client_publish(client, EMQXPublishTopic_user_longitude, longitude_str, strlen(longitude_str), 0, 0);//发布纬度信息
       esp_mqtt_client_publish(client, EMQXPublishTopic_user_speed, speed_str, strlen(speed_str), 0, 0);//发布速度信息
       vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}

/*连接wifi*/
void wifi_connect(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init(); //初始化wifi
    }
    ESP_ERROR_CHECK(ret);
 
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    // wifi_init_sta(); //connect
    while (1)
    {
        if(wifi_init_sta())
        {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
}

/*gps数据*/
void gps_task(void *pvParameters)
{
    GPS_init();
    GPS_data gps_data;
    
    while(1)
    {
        gps_data = gps_get_data();
        sprintf(latitude_str, "%f", gps_data.latitude);
        sprintf(longitude_str, "%f", gps_data.longitude);
        sprintf(speed_str, "%f", gps_data.speed_kmh);
        time_str[0] = gps_data.hour / 10 + '0';
        time_str[1] = gps_data.hour % 10 + '0';
        time_str[2] = ':';
        time_str[3] = gps_data.minute / 10 + '0';
        time_str[4] = gps_data.minute % 10 + '0';
        time_str[5] = '\0';
        
    }
}
void app_main(void)
{
    
    xTaskCreate(main_task, "main_task", MAIN_STK_SIZE, NULL, MAIN_TASK_PRIO,&MainTask_Handler);
    xTaskCreate(oled_task, "oled_task", OLED_STK_SIZE, NULL, OLED_TASK_PRIO,&OledTask_Handler);
    xTaskCreate(mqtt_task, "mqtt_task", MQTT_STK_SIZE, NULL, MQTT_TASK_PRIO,&MqttTask_Handler);
    // xTaskCreate(gps_task, "gps_task", GPS_STK_SIZE, NULL, GPS_TASK_PRIO,&GpsTask_Handler);
    // xTaskCreate(led_task, "led_task", LED_STK_SIZE, NULL, LED_TASK_PRIO, &LedTask_Handler);
    // vTaskStartScheduler();
    
}
