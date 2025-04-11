#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/spi.h>
#include <string.h>
#include "FreeRTOS.h"
#include "libopencm3/stm32/f1/gpio.h"
#include "task.h"
#include "semphr.h"
#include "projdefs.h"
#include "spi_w5500.h"
#include "wizchip_conf.h"
#include "log.h"
#include "open_socket.h"

//Task Handle name
TaskHandle_t httpServerHandle = NULL;
SemaphoreHandle_t mutex_w5500;

void task1(void* args __attribute((unused)));
void task2(void* args __attribute((unused)));
void uart_setup(void);
void spi1_setup(void);
void gpio_setup(void);
void clock_setup(void);
wiz_NetInfo default_net_info = {
    .ip = {192,168,132,3},
    .sn = {255,255,255,0},
    .gw = {192,168,132,1},
    .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef}
};

int main(void) {
	clock_setup();
	gpio_setup();
	uart_setup();
	spi1_setup();

	mutex_w5500 = xSemaphoreCreateMutex();
	if(mutex_w5500==NULL){
		my_printf("Failed to create W5500 mutex\r\n");
	}else {
		my_printf("Created mutex for W5500\r\n");
	}

	W5500Init();
	wizchip_setnetinfo(&default_net_info);
	wizchip_setnetmode(NM_FORCEARP | NM_WAKEONLAN);


	xTaskCreate(task1, "LED1", 32, NULL, 1, NULL);
	xTaskCreate(task2, "LED2", 32, NULL, 1, NULL);
	xTaskCreate(socket1_handle, "LISTEN", 2048, NULL, 5, NULL);
	xTaskCreate(socket2_handle, "HANDLE_COM", 1024, NULL, 5, NULL);

	vTaskStartScheduler();

	for(;;){
		 vTaskDelay(300);
		gpio_toggle(GPIOC, GPIO13);
	}

}

void vApplicationStackOverflowHook(TaskHandle_t xTask __attribute((unused)), char *pcTaskName){
				my_printf("Stack overflow in task %s\n", pcTaskName);
				UBaseType_t remain = uxTaskGetStackHighWaterMark(httpServerHandle);
				my_printf("ramaining stack in http %d\n", remain);
				while(1); //Halt or reset
}


void task1(void* args __attribute((unused))){
	for(;;){
		gpio_toggle(GPIOC, GPIO13);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void task2(void* args __attribute((unused))){
	for(;;){
		gpio_toggle(GPIOC, GPIO14);
        /*uart_send_string("Blink from pin 14\r\n");*/
		vTaskDelay(pdMS_TO_TICKS(1500));
	}
}

void clock_setup(void){
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	rcc_periph_clock_enable(RCC_GPIOC);
	// Enable clock for GPIOA (USART1 and SPI1)
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_SPI1);

}

void gpio_setup(void){
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13 | GPIO14);
	//Select and reset pin of the W5500
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO4 | GPIO3);
	//Deselect the SPI
	gpio_set(GPIOA, GPIO4);
}

void uart_setup(void) {
    // Configure TX (PA9) as alternate function push-pull
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO9);

    // Configure RX (PA10) as input floating
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_FLOAT, GPIO10);

    // Set up USART1 parameters
    usart_set_baudrate(USART1, 115200);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_mode(USART1, USART_MODE_TX_RX);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

    // Enable USART1
    usart_enable(USART1);
}
void spi1_setup(void){

    // Configure SPI1 pins:SCK, MOSI
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                  GPIO_SPI1_SCK | GPIO_SPI1_MOSI);
    // Configure SPI1 pins:MISO
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_FLOAT,
                  GPIO_SPI1_MISO);

    //reset SPI, SPI_CR1 register cleared, SPI is disabled
    rcc_periph_reset_pulse(RST_SPI1);

    // Configure SPI1 parameters
    spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_64,
                    SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_2,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST);

    //enable NSS
    spi_enable_software_slave_management(SPI1);
    spi_set_nss_high(SPI1);

    // Enable SPI1
    spi_enable(SPI1);

}
