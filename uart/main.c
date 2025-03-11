#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/i2c.h>
#include <stddef.h>
#include "blink.h"
#include "libopencm3/stm32/common/spi_common_v1.h"
#include "spi_w5500.h"
#include "wizchip_conf.h"
#include "st7735.h"

static void clock_setup(void);
static void gpio_setup(void);
static void usart_setup(void);
static void spi1_setup(void);
static void spi2_setup(void);
wiz_NetInfo default_net_info = {
    .ip = {192,168,132,3},
    .sn = {255,255,255,0},
    .gw = {192,168,132,1},
    .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef}
};

int main(void) {
    //Initialize all the setup
    clock_setup();
    gpio_setup();
    usart_setup();
    spi1_setup();
    spi2_setup();
    W5500Init();
    st7735_init();
    wizchip_setnetinfo(&default_net_info);
    st77xx_fill_screen(0x9C0F);

    //Code begins here
    while (1) {
        delay(1000000);
        blink();
    }

    return 0;
}

static void clock_setup(void) {
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART1);
    rcc_periph_clock_enable(RCC_SPI1);
    // Enable I2C1 and GPIOB clock
    rcc_periph_clock_enable(RCC_SPI2);
    rcc_periph_clock_enable(RCC_GPIOB);
    // Enable clock for LED blinking
    rcc_periph_clock_enable(RCC_GPIOC);
}

static void gpio_setup(void){
    //General Blue LED
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
    //Reset pin of the W5500
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO4);
    gpio_set(GPIOA, GPIO4);
}

void usart_setup(void) {
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT,
                  GPIO_USART1_RX);

    usart_set_baudrate(USART1, 115200);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_mode(USART1, USART_MODE_TX_RX);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

    usart_enable(USART1);
}

static void spi1_setup(void){

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
static void spi2_setup(void){
    // Configure SPI2 pins:SCK, MOSI
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                  GPIO_SPI2_SCK | GPIO_SPI2_MOSI);

    // Configure CS, DC, RST as output GPIOs
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, ST7735_CS | ST7735_RST | ST7735_DC);

    //reset SPI, SPI_CR2 register cleared, SPI is disabled
    rcc_periph_reset_pulse(RST_SPI2);

    // Configure SPI2 parameters
    spi_init_master(SPI2, SPI_CR1_BAUDRATE_FPCLK_DIV_2,
                    SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_2,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST);

    //Enable SPI2
    spi_enable(SPI2);
}
