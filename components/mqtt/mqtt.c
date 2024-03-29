#include <stdio.h>
#include "mqtt.h"
static const char *TAG = "MQTT";
esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    // 获取MQTT客户端结构体指针
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    // 通过事件ID来分别处理对应的事件
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:    // MQTT连上事件
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            // MQTT Client发布主题函数，主题是/topic/qos1，服务质量qos1，发布的数据是data-3
            msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            // MQTT Client订阅主题函数，主题是/topic/qos0，服务质量qos0
            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            // MQTT Client订阅主题函数，主题是/topic/qos1，服务质量qos1
            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
            ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
            // MQTT Client取消订阅主题函数
            msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
            ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:    // MQTT断开连接事件
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:    // MQTT发送订阅成功事件
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:    // MQTT取消订阅事件
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:    // MQTT发布成功事件
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:    // MQTT接收数据事件
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%ld", base, event_id);
    mqtt_event_handler_cb(event_data);
}

esp_mqtt_client_handle_t mqtt_app_start(void)
{
     // 1、定义一个MQTT客户端配置结构体，输入MQTT的url
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.hostname = EMQX_host,  // MQTT服务器端口
		.broker.address.port = EMQX_port,
        .broker.address.transport = MQTT_TRANSPORT_OVER_TCP,
		.credentials.client_id = EMQX_client_id,
		.credentials.username = EMQX_username,
		.credentials.authentication.password = EMQX_password,
        
        

    };
#if CONFIG_BROKER_URL_FROM_STDIN
    char line[128];

    if (strcmp(mqtt_cfg.uri, "FROM_STDIN") == 0) {
        int count = 0;
        printf("Please enter url of mqtt broker\n");
        while (count < 128) {
            int c = fgetc(stdin);
            if (c == '\n') {
                line[count] = '\0';
                break;
            } else if (c > 0 && c < 127) {
                line[count] = c;
                ++count;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        mqtt_cfg.uri = line;
        printf("Broker url: %s\n", line);
    } else {
        ESP_LOGE(TAG, "Configuration mismatch: wrong broker url");
        abort();
    }
#endif /* CONFIG_BROKER_URL_FROM_STDIN */

    // 2、通过esp_mqtt_client_init获取一个MQTT客户端结构体指针，参数是MQTT客户端配置结构体
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    // 3、注册MQTT事件
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    // 4、开启MQTT功能
    esp_mqtt_client_start(client);
    return client;
}


void user_mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
		.broker.address.hostname = EMQX_host,
		.broker.address.port = EMQX_port,
        .broker.address.transport = MQTT_TRANSPORT_UNKNOWN,
		.credentials.client_id = EMQX_client_id,
		.credentials.username = EMQX_username,
		.credentials.authentication.password = EMQX_password,

    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

    char mqtt_publish_data3[] = "mqtt i am esp32";
    uint8_t num = 0;
    while(1)
    {
       esp_mqtt_client_publish(client, EMQXPublishTopic_user_state, mqtt_publish_data3, strlen(mqtt_publish_data3), 1, 0);
       vTaskDelay(2000 / portTICK_PERIOD_MS);
	   if(num++ > 5) break;
	}
    vTaskDelete(NULL);
}
