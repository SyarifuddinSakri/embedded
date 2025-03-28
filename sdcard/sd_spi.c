#include <libopencm3/stm32/spi.h>
#include "sd_spi.h"
#include "log.h"
#include "libopencm3/stm32/f1/rcc.h"

// Chip Select (CS) Control
static void cs_high(void) { gpio_set(GPIOA, GPIO4); }
static void cs_low(void)  { gpio_clear(GPIOA, GPIO4); }

static uint8_t spi_read_write(uint8_t data){
    //Wait for the SPI 1 Status Register Transmit is empty
    while((SPI_SR(SPI1)&SPI_SR_TXE)!= SPI_SR_TXE){
    }

    SPI1_DR=data;

    //Wait until data returned from the peripheral
    while(!(SPI_SR(SPI1)&SPI_SR_RXNE)){
    }
    return SPI1_DR;
}

// Initialize SPI for SD Card
void sd_spi_init(void) {
    
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
    spi_init_master(SPI1, SPI_CR1_BAUDRATE_FPCLK_DIV_256,
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
// Send a command to the SD card
uint8_t sd_send_cmd(uint8_t cmd, uint32_t arg) {
    uint8_t res;
    cs_low();
    spi_read_write(cmd);                     // Command
    spi_read_write(arg >> 24);               // Argument [31:24]
    spi_read_write(arg >> 16);               // Argument [23:16]
    spi_read_write(arg >> 8);                // Argument [15:8]
    spi_read_write(arg);                     // Argument [7:0]
    spi_read_write(0x95);                    // CRC (dummy for SPI mode)

    // Wait for response (up to 8 bytes)
    for (uint8_t i = 0; i < 10; i++) {
        res = spi_read_write(0xFF);
        if (!(res & 0x80)) break;      // Response received
    }
    cs_high();
    spi_read_write(0xFF);                    // Extra clocks
    return res;
}

// Initialize SD Card in SPI Mode
uint8_t sd_init(void) {
    sd_spi_init();

    // Send 80 dummy clocks to stabilize the card
    cs_high();
    for (uint8_t i = 0; i < 10; i++) spi_read_write(0xFF);

    // Send CMD0 (GO_IDLE_STATE)
	// step 1: reset (CMD0)
		my_printf("Intializing sd card\r\n");
    if (sd_send_cmd(SD_CMD0, 0) != SD_R1_IDLE){
		 my_printf("CMD0 failed\r\n");
		 return 1;
		}
		 my_printf("CMD0 OK\r\n");

    // Send CMD8 (SEND_IF_COND) to check SDv2
	// step 2: check voltage (CMD8)
    if (sd_send_cmd(SD_CMD8, 0x1AA) & SD_R1_ILLEGAL_CMD) {
        // SDv1 or MMC card (not handling MMC here)
			 my_printf("CMD8 not supported (SDv1 card?)\r\n");
    } else {
        // SDv2 card: Wait for ACMD41 (APP_SEND_OP_COND)
				my_printf("CMD8 OK (SDv2 card)\r\n");
        for (uint32_t retry = 0; retry < 1000; retry++) {
            if (sd_send_cmd(SD_ACMD41, 0x40000000) == 0) break;
        }
    }
  // Check if card is SDHC (high capacity)
  uint32_t ocr;
  if (sd_send_cmd(SD_CMD58, 0) == 0) {  // READ_OCR
    spi_read_write(0xFF);  // Read OCR[31:24]
    spi_read_write(0xFF);  // OCR[23:16]
    ocr = (spi_read_write(0xFF) << 8) | spi_read_write(0xFF);  // OCR[15:0]
    cs_high();
    if (ocr & (1 << 30)) {  // CCS bit = 1 (SDHC/SDXC)
      my_printf("SDHC card detected, skipping CMD16\r\n");
      return 0;  // Success (block size fixed to 512B)
    }
  }

  // Only send CMD16 for non-SDHC cards
  if (sd_send_cmd(SD_CMD16, 512) != 0) {
    my_printf("CMD16 failed (non-SDHC card)\r\n");
    return 1;
  }
		my_printf("SD Init Complete\r\n");
    return 0; // Success
}
// Read a single sector (512 bytes)
uint8_t sd_read_sector(uint32_t sector, uint8_t *data, uint32_t count) {

    for (uint32_t i = 0; i < count; i++) {
        cs_low();
        if (sd_send_cmd(SD_CMD17, sector + i) != 0) {
						my_printf("CMD17 failed for sector %d",sector);
            cs_high();
            return 1; // Error
        }

        // Wait for data token (0xFE)
        while (spi_read_write(0xFF) != 0xFE);

        // Read 512 bytes
        for (uint16_t j = 0; j < 512; j++) data[j] = spi_read_write(0xFF);
        spi_read_write(0xFF); // Discard CRC
        cs_high();
        data += 512;
    }
    return 0; // Success
}

// Write a single sector (512 bytes)
uint8_t sd_write_sector(uint32_t sector, const uint8_t *data, uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        cs_low();
        if (sd_send_cmd(SD_CMD24, sector + i) != 0) {
            cs_high();
            return 1; // Error
        }

        spi_read_write(0xFE); // Data token
        for (uint16_t j = 0; j < 512; j++) spi_read_write(data[j]);
        spi_read_write(0xFF); // Dummy CRC
        spi_read_write(0xFF); // Wait for busy clear
        cs_high();
        data += 512;
    }
    return 0; // Success
}
