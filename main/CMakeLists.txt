idf_component_register(SRCS "blink_example_main.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES "posture" "gps" "alarm"
                       PRIV_REQUIRES "oled" "mqtt" "espmqtt" "esp_wifi" "wifi" "nvs_flash" )

set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} components")
