/*
syn6288模块 实现跌倒播报
*/

#pragma once
#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define TXD_ALARM_PIN (GPIO_NUM_17) //发送
#define RXD_ALARM_PIN (GPIO_NUM_16) //接收
#define SYN6288_UART_PORT UART_NUM_1 //使用UART1端口 避免和ATGM冲突
#define SYN6288_UART_BAUD_RATE (9600) //波特率
#define SYN6288_UART_TIMEOUT_MS (1000) //超时时间

void init_uart();
void syn6288_speak(const char *voice);
