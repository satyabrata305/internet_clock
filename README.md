Here’s a complete, runnable ESP-IDF C program that implements an Internet Clock using NTP (SNTP) to fetch accurate time from the internet and display it via the serial monitor.

This works on ESP32 with Wi-Fi and ESP-IDF (v4.x or v5.x).

How It Works:-
Wi-Fi ConnectionConnects ESP32 to your Wi-Fi network using esp_wifi.
NTP Time SyncUses SNTP to fetch accurate time from pool.ntp.org.
Time DisplayPrints the current local time every second to the serial monitor.
Error HandlingRetries NTP sync up to 15 times before giving up.



Dev kit - ESP32 wroom

IDE - Espressif (ESP-IDF)

Language - Embedded C
