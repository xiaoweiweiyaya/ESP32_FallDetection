#include <stdio.h>
#include "alarm.h"

static const char *TAG = "SYN6288";

void init_uart() {
    uart_config_t uart_config = {
        .baud_rate = SYN6288_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(SYN6288_UART_PORT, &uart_config);
    uart_set_pin(SYN6288_UART_PORT, GPIO_NUM_5, GPIO_NUM_6, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(SYN6288_UART_PORT, 1024, 0, 0, NULL, 0);
}

void syn6288_speak(const char *text) {
    const int tx_buffer_size = 1024;
    uint8_t *tx_buffer = (uint8_t *) malloc(tx_buffer_size);
    if (!tx_buffer) {
        ESP_LOGE(TAG, "Failed to allocate memory for TX buffer");
        return;
    }

    // 构造要发送的命令或文本数据
    int length = snprintf((char *) tx_buffer, tx_buffer_size, "%s\r\n", text);

    // 发送数据到 SYN6288 模块
    uart_write_bytes(SYN6288_UART_PORT, (const char *) tx_buffer, length);

    free(tx_buffer);
}