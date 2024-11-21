// UDP Client with WiFi connection communication via Socket

#include <stdio.h>
#include <string.h>
#include <nvs_flash.h>
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_mac.h"
#include "esp_eth.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "esp_http_client.h"
#include "esp_event.h"
#include "esp_system.h"
#include <errno.h>
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"

#include "nvs_flash.h"
#include "ping/ping_sock.h"
#include "driver/gpio.h"
#include <fcntl.h>
// #define DUY_WIFI_CONNECT 1
#define AT_HOME 1
#ifdef  DUY_WIFI_CONNECT
#define SSID "Leduy"
#define PASS "250920032003"
#define PORT 3333
#define HOST_IP_ADDR "192.168.217.149"
#endif
#ifdef  AT_HOME
#define SSID "Anhson"
#define PASS "0978915672"
#define PORT 12345
#define HOST_IP_ADDR "192.168.1.162"
#endif
#define FLASH 4 

#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif
#include "esp_camera.h"
#define BOARD_ESP32CAM_AITHINKER 1

#ifdef BOARD_ESP32CAM_AITHINKER

#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1 //software reset will be performed
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22

#endif
#define UDP_PACKET_SIZE 1024    
static const char *TAG = "UDP SOCKET CLIENT";
static const char *payload = "Message from ESP32 UDP Client";
int command = 0;
int timeSent = 0;
/*
NOCOMMAND = 0


*/

void configIO(){
     gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;       // Không sử dụng ngắt
    io_conf.mode = GPIO_MODE_OUTPUT;             // Đặt chế độ là đầu ra
    io_conf.pin_bit_mask = (1ULL << FLASH);  // Chỉ định chân GPIO4
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // Không bật pull-down
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;     // Không bật pull-up
    gpio_config(&io_conf);
    gpio_set_level(FLASH, 0);
}
#if ESP_CAMERA_SUPPORTED
static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG, //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_UXGA,    //QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.

    .jpeg_quality = 12, //0-63, for OV series camera sensors, lower number means higher quality
    .fb_count = 2,       //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_LATEST,
};

static esp_err_t init_camera(void)
{
    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }

    return ESP_OK;
}
#endif
static void send_string(const char *message) {
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(3333);

    // Create UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return;
    }
    ESP_LOGI(TAG, "STRING SENT");
    // Send message
    int err = sendto(sock, message, strlen(message), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err < 0) {
        ESP_LOGE(TAG, "Error occurred while sending message: errno %d", errno);
    } else {
        ESP_LOGI(TAG, "Message sent: %s", message);
    }

    // Close the socket
    close(sock);
}
void set_socket_blocking_mode(int sock) {
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl(F_GETFL) failed");
        return;
    }

    flags &= ~O_NONBLOCK;  // Clear the non-blocking flag
    if (fcntl(sock, F_SETFL, flags) == -1) {
        perror("fcntl(F_SETFL) failed");
    }
}
void send_image(camera_fb_t *pic) {
    ESP_LOGE(TAG, "time sent: %d", timeSent);
    timeSent++;
    struct sockaddr_in server_addr;
    int sock, bytes_sent;

    // Create a UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        printf("Failed to create socket\n");
        return;
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3333);
    inet_pton(AF_INET, HOST_IP_ADDR, &server_addr.sin_addr);

    // Send the image in chunks if it is too large for a single packet
    size_t remaining = pic->len;      // Remaining data to send
    size_t offset = 0;  
    size_t fail[4] = {'f', 'a', 'i', 'l'};

                  // Current position in the buffer
    while (remaining > 0) {
        // Determine the size of the current chunk
        size_t chunk_size = (remaining > UDP_PACKET_SIZE) ? UDP_PACKET_SIZE : remaining;
        // ESP_LOGI(TAG, "CHUNK %d", chunk_size);
        // Send the chunk
        bytes_sent = sendto(sock, pic->buf + offset, chunk_size, 0,
                            (struct sockaddr *)&server_addr, sizeof(server_addr));
        // ESP_LOGI(TAG, "errno %d", errno);

        if (bytes_sent < 0) {
            printf("Failed to send data\n");
                ESP_LOGI(TAG, "errno: %d", errno);
            bytes_sent = sendto(sock, fail, chunk_size, 0,
                            (struct sockaddr *)&server_addr, sizeof(server_addr));  
            break;
        }

        printf("Sent %d bytes\n", bytes_sent);

        // Update the offset and remaining size
        offset += bytes_sent;
        remaining -= bytes_sent;
    }
    const char *tempCommand = "successful";
    bytes_sent = sendto(sock, tempCommand, strlen(tempCommand), 0,
                            (struct sockaddr *)&server_addr, sizeof(server_addr));
    ESP_LOGI(TAG, "IMAGE SENT WIDTH %d, HEIGHT %d", pic->width, pic->height);

    // Close the socket
    close(sock);
}
static void udp_client_task(void *pvParameters)
{
    char rx_buffer[6];
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;

    while (1) {
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(HOST_IP_ADDR);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(4444);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        
 
        set_socket_blocking_mode(sock);
        // Set timeout
        // struct timeval timeout;
        // timeout.tv_sec = 2;
        // timeout.tv_usec = 0;
        // setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

        ESP_LOGI(TAG, "Socket created, sending to %s:%d", host_ip, PORT);
        while (1) {
            struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
            socklen_t socklen = sizeof(source_addr);
            if (bind(sock, (struct sockaddr *)&source_addr, sizeof(source_addr)) < 0) {
                ESP_LOGE(TAG, "Failed to bind socket: errno %d", errno);
                close(sock);
                return;
            } 
            sendto(sock, "check port", strlen("check port"), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
            ESP_LOGI(TAG, "LEN RECEIVE: %d", len);
            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
            }
            // Data received
            else {
                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, host_ip);
                ESP_LOGI(TAG, "%s", rx_buffer);
                if (strncmp(rx_buffer, "OK: ", 4) == 0) {
                    ESP_LOGI(TAG, "Received expected message, reconnecting");
                    break;
                }
            }
            vTaskDelay(20 / portTICK_PERIOD_MS);
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
}
static void udp_server_task(void *pvParameters)
{
    char rx_buffer[128];
    char addr_str[128];
    int addr_family = (int)pvParameters;
    int ip_protocol = 0;
    struct sockaddr_in6 dest_addr;

    while (1)
    {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(12345);
        ip_protocol = IPPROTO_IP;

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

        // Set timeout
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0)
        {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        }
        ESP_LOGI(TAG, "Socket bound, port %d", PORT);

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t socklen = sizeof(source_addr);

        while (1)
        {
            ESP_LOGI(TAG, "Waiting for data");
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            if (len > 0)
            {
                // Get the sender's ip address as string
                inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
                rx_buffer[len] = 0; // Null-terminate
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(TAG, "%s", rx_buffer);
                if (strcmp(rx_buffer, "capture image") == 0) {
                    printf("start capturing\n");
                    gpio_set_level(FLASH, 1);
                    vTaskDelay(1000/ portTICK_PERIOD_MS);
                    camera_fb_t *capTemp = esp_camera_fb_get();
                    if (capTemp) {
                        send_image(capTemp);  // Replace with your server's IP and port
                        esp_camera_fb_return(capTemp);
                    }
                    vTaskDelay(1000/ portTICK_PERIOD_MS);
                    gpio_set_level(FLASH, 0);
                    printf("end capturing\n");
                }
                // sendto(sock, rx_buffer, len, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
            }
            else
            {
                ESP_LOGI(TAG, "Did not received data");
            }
        }

        if (sock != -1)
        {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}
static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting WIFI_EVENT_STA_START ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected WIFI_EVENT_STA_CONNECTED ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection WIFI_EVENT_STA_DISCONNECTED ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        break;
    }
}

void wifi_connection()
{
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASS}};
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_connect();
}
// static void imageTask(){
//     while(1){
//         camera_fb_t *pic = esp_camera_fb_get();
//     ESP_LOGI(TAG, "CAPTURED %d bytes", pic->len);
//     if (!pic) {
//         printf("Camera capture failed\n");
//         return;
//     }

//     // Send the image
//     send_image(pic);

//     // Return the frame buffer to free memory
//     esp_camera_fb_return(pic);
//     vTaskDelay(1000 / portTICK_PERIOD_MS);
//     }
// }
// static void verifyTask(){
//     while(true){
//         if(command == 1){
            
//         }

//     }
// }
void app_main(void)
{
    if(ESP_OK != init_camera()) {
        return;
    }
    configIO();
    wifi_connection();
    const char *assign = "esp";
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    // xTaskCreate(udp_client_task, "udp_client", 4096, NULL, 5, NULL);
    send_string(assign);
    // char rx_buffer[120];
    // memset(rx_buffer, 0, sizeof(rx_buffer)); // Clear the buffer
    // int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&server_addr, &addr_len);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    //xTaskCreate(imageTask, "capture and sending", 8*1024, NULL, 5, NULL);
    // xTaskCreate(udp_server_task, "receive command task", 4*1024, NULL, 5, NULL);
    xTaskCreate(udp_server_task, "udp_server", 4096, (void *)AF_INET, 5, NULL);
    // xTaskCreate(verifyTask, "verify", 4*1024, NULL, 5, NULL);
    
}