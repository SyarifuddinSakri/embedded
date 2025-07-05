#include "esp8266.h"
#include "libopencm3/stm32/usart.h"
#include "log.h"
#include <stdint.h>
#include <string.h>

void esp_send_byte(uint8_t data) { usart_send_blocking(USART2, data); }
void esp_send_command(const char *str) {
  while (*str) {
    esp_send_byte(*str++);
  }
}

int response_is_ok(const char *response) {
  return strstr(response, "OK") != NULL;
}

void esp_read_response(char *buff, uint16_t max_len) {
  uint16_t i = 0;
  char c;
  while (i < (max_len - 1)) {
    c = (char)usart_recv_blocking(USART2);
    buff[i++] = c;
    buff[i] = '\0'; // Null terminate every time so strstr() works

    // Check if OK or ERROR is in buffer already
    if (strstr(buff, "OK\r\n") || strstr(buff, "ERROR\r\n")) {
      break;
    }
  }
}
void esp8266_connect(void) {
  char resp_buffer[128];
  // Test communication
  esp_send_command("AT\r\n");
  esp_read_response(resp_buffer, sizeof(resp_buffer));

  my_printf("AT ->: %s\r\n", resp_buffer);
  if (response_is_ok(resp_buffer)) {
    // Optional: Indicate success (eg: blink LED or debug print)
  }

  // Set WiFi mode to Station
  esp_send_command("AT+CWMODE=1\r\n");
  esp_read_response(resp_buffer, sizeof(resp_buffer));
  my_printf("AT+CWMODE=1 ->: %s\r\n", resp_buffer);
  if (response_is_ok(resp_buffer)) {
    // Mode set OK
  }

  // Connect to WiFi AP
  esp_send_command("AT+CWJAP=\"1913@unifi\",\"steven123\"\r\n");
  esp_read_response(resp_buffer, sizeof(resp_buffer));
  my_printf("AT+CWJAP ->: %s\r\n", resp_buffer);
  if (response_is_ok(resp_buffer)) {
    // Connected to AP
  }

  // Get IP address
  esp_send_command("AT+CIFSR\r\n");
  esp_read_response(resp_buffer, sizeof(resp_buffer));
  my_printf("AT_CIFSR ->: %s\r\n", resp_buffer);
  if (response_is_ok(resp_buffer)) {
    // IP address received
  }
}
