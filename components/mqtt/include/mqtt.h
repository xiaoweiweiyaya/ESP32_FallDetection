#pragma once
#include "mqtt_client.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "esp_log.h"

#define   EMQX_host       "192.168.102.48"
#define   EMQX_port       18084
/*Client ID：     ${ClientID}|securemode=${Mode},signmethod=${SignMethod}|*/
#define   EMQX_client_id  "esp32"
/*User Name：     ${DeviceName}&${ProductKey}*/
#define   EMQX_username   "FallDetection"
/*使用官网 MQTT_Password 工具生成*/
#define   EMQX_password   "Chenwei00"

///a1Pvdruq4mz/${deviceName}/user/update    //注意把deviceName换成测试的设备名
#define   EMQXPublishTopic_user_state    "fall/state" //发布主题
#define   EMQXPublishTopic_user_temp    "fall/temperature" 
#define   EMQXPublishTopic_user_latitude  "fall/latitude"
#define   EMQXPublishTopic_user_longitude  "fall/longitude"
#define   EMQXPublishTopic_user_speed  "fall/speed"

#define   EMQXSubscribeTopic_user_get     "fall/message" //订阅主题

esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event);
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
//非加密
esp_mqtt_client_handle_t mqtt_app_start(void);
//加密
void user_mqtt_app_start(void);