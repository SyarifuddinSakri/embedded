#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>
#include <stdint.h>
#include "libopencm3/stm32/common/spi_common_v1.h"
#include "libopencm3/stm32/f1/gpio.h"
#include "sysout.h"
#include "st77xx.h"
#include "blink.h"
#include "donut.h"
void gpio_init(void);
void clock_setup(void);
static void spi2_setup(void);

int main(void){
    clock_setup();
    enable_sysout();
    gpio_init();
    spi2_setup();
    st77xx_init();
    st77xx_fill_screen(0x0000);
    /*draw_circle(60, 80, 20, 0x8899, 3, 0xF6E0);  // Red outline, width 2, filled with Green*/
    /*draw_text(26, 10, "MY DONUTT!!", 0xFFFF);*/
    st77xx_write_command(0x21);
    /*draw_stylish_number(40, 20, '8', 0x002F, 0x0670, 5);  // Red number with black shadow*/
    /*draw_stylish_number(40, 120, '9', 0x002F, 0x0670, 3);  // Red number with black shadow*/
    /*draw_line(0, 0, 100, 90, 0x002F);*/
    /*draw_line_width(0, 0, 90, 100, 0x004F, 3);*/
    draw_stylish_text(30, 120, "DONUT", 0xe9e2, 0xFF50, 2);
    set_rotation(1);
    print_donut();
    delay(100);


    while(1){
        blink();
    }

    return 0;
}

void gpio_init(void){
    //Enable general blinking LED
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
}

void clock_setup(void){
    //General clock.
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
    //Enable clock for SPI1, USART1 through GPIOA
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART1);
    rcc_periph_clock_enable(RCC_SPI1);
    //Enable clock for SPI2, through GPIOB
    rcc_periph_clock_enable(RCC_SPI2);
    rcc_periph_clock_enable(RCC_GPIOB);
    //Enable clock for GPIOC
    rcc_periph_clock_enable(RCC_GPIOC);
}

static void spi2_setup(void){
    // Configure SPI2 pins:SCK, MOSI
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                  GPIO_SPI2_SCK | GPIO_SPI2_MOSI);

    // Configure CS, DC, RST as output GPIOs
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, ST77XX_CS | ST77XX_RST | ST77XX_DC);

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
