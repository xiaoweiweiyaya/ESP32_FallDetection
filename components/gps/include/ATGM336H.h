#pragma once
#include "driver/uart.h"
#include "driver/gpio.h"

//GPS模块串口号、引脚号和相关计算数据
#define UART_GPS UART_NUM_2
#define UART_GPS_TXD GPIO_NUM_1
#define UART_GPS_RXD GPIO_NUM_2

typedef struct GPS_data_{
	double latitude;
	double longitude;
	double speed_kmh;// km/h
	double speed_ms; // m/s
	double course;
	double altitude; // 海拔
	int hour;
	int minute;
	int second;
	int day;
	int month;
	int year;
}GPS_data;

typedef struct UTC_time
{
	int hour;
	int minute;
	int second;
}UTCtime;

typedef struct UTC_data
{
	int day;
	int month;
	int year;
}UTCdate;

//GPS模块初始化
esp_err_t GPS_init();
//获取GPS数据
GPS_data gps_get_data(void);
