//esp internet clock
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

static const char *TAG = "NTP_CLOCK";

//wifi credentials
#define WIFI_SSID "Muna"
#define WIFI_PASSWORD "9776600123"

//SNTP server
#define NTP_SERVER "pool.ntp.org"


void time_sync_notification_cb(struct timeval *tv){
ESP_LOGI(TAG,"Time synchronized with NTP server.");	
	//you can add further actions here,like updating a display
}

void wifi_event_handler(void *arg,esp_event_base_t event_base,int32_t event_id,void* event_data)
{
	
if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
{
 esp_wifi_connect();	
}	
else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
{
ESP_LOGI(TAG,"WiFi disconnected, reconnecting...");	
esp_wifi_connect();	
}
else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
{
ESP_LOGI(TAG,"Got ip address, starting SNTP client");		
esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
esp_sntp_setservername(0,NTP_SERVER);
esp_sntp_init();

//set timezone to india standard time(IST)-UTC+5:30
setenv("TZ","IST-5:30",1);
tzset();
}
}
	
	
void app_main(void)
{
	
//initialize nvs
esp_err_t ret = nvs_flash_init();
if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
{
ESP_ERROR_CHECK(nvs_flash_erase());
ret = nvs_flash_init();	
}	

ESP_ERROR_CHECK(ret);

//Initialize network interface and event loop
ESP_ERROR_CHECK(esp_netif_init());
ESP_ERROR_CHECK(esp_event_loop_create_default());

//create WiFi station interface
esp_netif_create_default_wifi_sta();

//register WiFi event handler
ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&wifi_event_handler,NULL));
ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP,&wifi_event_handler,NULL));

//configure and start Wi-Fi
wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
ESP_ERROR_CHECK(esp_wifi_init(&cfg));

wifi_config_t wifi_config = {
	.sta = {
		.ssid =WIFI_SSID,
		.password = WIFI_PASSWORD,
		.threshold.authmode = WIFI_AUTH_WPA2_PSK,
		.sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
	},
	
};

ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA,&wifi_config));
ESP_ERROR_CHECK(esp_wifi_start());

//register sntp time synchronization callback
sntp_set_time_sync_notification_cb(time_sync_notification_cb);

//Main loop to periodically print time
while(1)
{
	
time_t now;
struct tm timeinfo;
time(&now);
localtime_r(&now,&timeinfo);


if(timeinfo.tm_year < (2000 - 1900))	{//check if time is valid(after 2000)
ESP_LOGI(TAG,"Time not yet set from NTP server");
}
else
{
char strftime_buf[64];
strftime(strftime_buf,sizeof(strftime_buf),"%c",&timeinfo);
ESP_LOGI(TAG,"current time: %s",strftime_buf);	
}
vTaskDelay(pdMS_TO_TICKS(5000));//delay for 5 seconds
}//while end	
}//app main end



