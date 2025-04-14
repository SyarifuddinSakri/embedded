#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/gpio.h>
#include <stdint.h>
#include "sd_spi.h"
#include "diskio.h"
#include "ff.h"
#include "log.h"

uint8_t SPIReadWrite(uint8_t data){
    //Wait for the SPI 1 Status Register Transmit is empty
    while((SPI_SR(SPI1)&SPI_SR_TXE)!= SPI_SR_TXE){
    }

    SPI1_DR=data;

    //Wait until data returned from the peripheral
    while(!(SPI_SR(SPI1)&SPI_SR_RXNE)){
    }
    return SPI1_DR;
}

void sd_select(void){
    gpio_clear(GPIOA, GPIO4);
}

void sd_deselect(void){
    gpio_set(GPIOA, GPIO4);
}

uint8_t sd_read(void){
    uint8_t rb;
    rb=SPIReadWrite(0x00);
    return rb;
}

void sd_write(uint8_t wb){
    SPIReadWrite(wb);
}

void sd_readburst(uint8_t* pBuf, uint16_t len){
    for(uint16_t i=0; i<len; i++){
        pBuf[i] = SPIReadWrite(0x00);
    }
}

void sd_writeburst(uint8_t* pBuf, uint16_t len){
    for(uint16_t i=0; i<len; i++){
        SPIReadWrite(pBuf[i]);
    }
}

uint8_t SD_CARD_InitialiseCard(void){
	UINT i = 0;

	sd_deselect();
	for(i=0; i<0xFFFF; i++)
		sd_write(0xFF);

	for(i=0; i<0xFFFF; i++);

	while(SD_CARD_Cmd(CMD0, 0) != R1_IDLE_STATE);//GO_IDLE_STAT: Reset the card
	
	uint32_t r = SD_CARD_Cmd(CMD8, 0x1AA);

	if(r==0x1AA){
		return SD_CARD_InitialiseCardV2();
	}else if(r==(R1_IDLE_STATE|R1_ILLEGAL_COMMAND)){
		return SD_CARD_InitialiseCardV1();
	}else{
		return SDCARD_FAIL;
	}
}
uint8_t SD_CARD_InitialiseCardV1(void){
    uint8_t cmd;
    UINT i = 0;

    if(SD_CARD_Cmd(ACMD41, 0x40040000) <= 1) //ACMD41 - set to 3V, use 0x40200000 for 3V3
        cmd = ACMD41;
    else
        cmd = CMD1;

    for(i = 0; i < SD_COMMAND_TIMEOUT;i++)
    {
        if(SD_CARD_Cmd(cmd, 0) == 0) //CMD1 or ACMD41
        {   
            // Set block length to 512 (CMD16)
            if(SD_CARD_Cmd(CMD16, 512) != 0) //CMD16
                return SDCARD_FAIL;

            //Init: SEDCARD_V1
            if(cmd == ACMD41)
                return SDCARD_V1;
            else
                return SDCARD_MMCV3;
        }
    }

    //Timeout waiting for v1.x card
    return SDCARD_FAIL;	
}
uint8_t SD_CARD_InitialiseCardV2(void){
    UINT i = 0;
    UINT j = 0;

    for(i = 0;i < SD_COMMAND_TIMEOUT;i++)
    {
        for(j = 0;j < 0xFF;j++);
        if(SD_CARD_Cmd(ACMD41, 0x40040000) == 0) //ACMD41 - set to 3V, use 0x40200000 for 3V3
        {
            uint32_t ocr = SD_CARD_Cmd(CMD58, 0); //CMD58
            return (ocr & 0x40000000) ? SDCARD_V2 | SDCARD_BLOCK : SDCARD_V2;
        }
    }

    //Timed out waiting for v2.x card
    return SDCARD_FAIL;
}

uint32_t SD_CARD_Cmd(UINT cmd, UINT arg) {
    struct command_fields com;
    com.start_bit = 0;
    com.transmitter_bit = 1;
    com.index = cmd;
    com.argument = arg;
    if(cmd == CMD0)
        com.crc = 0x4A;
    else if(cmd == CMD8)
        com.crc = 0x43;
    else
        com.crc = 0x7F;
    com.end_bit = 1;

    if(cmd == ACMD13 || cmd == ACMD23 || cmd == ACMD41) //ACMDx
        SD_CARD_Cmd(CMD55, 0); //CMD55

    SD_CARD_WriteCom(&com);

    if(cmd == CMD8)
        return SD_CARD_RecieveR7();
    else if(cmd == CMD58)
        return SD_CARD_RecieveR3();
    else
        return SD_CARD_RecieveR1();
}

void SD_CARD_WriteCom(struct command_fields *com) {

		sd_select();
    sd_write(0xFF);
    sd_write((0xFF & ((com->start_bit << 7) | (com->transmitter_bit << 6) | com->index)));
    sd_write((0xFF & (com->argument >> 24)));
    sd_write((0xFF & (com->argument >> 16)));
    sd_write((0xFF & (com->argument >> 8)));
    sd_write((0xFF & com->argument));
    sd_write((0xFF & ((com->crc << 1) | com->end_bit)));
}
UINT SD_CARD_Read(BYTE *buffer, UINT length) {
    UINT i = 0;
		sd_select();

    for(i = 0; i < SD_COMMAND_TIMEOUT;i++)
    {
        // read until start byte (0xFF)
        if(SPIReadWrite(0xFF) == 0xFE) {
            // read data
            for(i = 0;i < length;i++)
                buffer[i] = SPIReadWrite(0xFF);

            sd_write(0xFF); // checksum
            sd_write(0xFF);

						sd_deselect();
            sd_write(0xFF);
            return SUCCESS;
        }
    }

    return ERROR;
}
UINT SD_CARD_Write(const BYTE *buffer, BYTE token) {
    UINT i = 0;
	sd_select();

		while(SPIReadWrite(0xFF)!=0xFF);

    // indicate start of block
		sd_write(token);

    if(token != 0xFD)
    {
        // write the data
        for(i = 0;i < 512;i++)
        {
						sd_write(*buffer);
            buffer++;
        }

        // write the checksum
				sd_write(0xFF);
				sd_write(0xFF);

        // check the repsonse token
				uint8_t resp = 0x00;
		        do
        {
            resp = SPIReadWrite(0xFF);
        }
        while(resp == 0x00);

        if((resp & 0x1F) != 0x05)
        {
            // wait for write to finish
            while(SPIReadWrite(0xFF) != 0xFF);

						sd_write(0xFF);
						sd_deselect();
						sd_write(0xFF);

            return SUCCESS;
        }
    }

    // wait for write to finish
    while(SPIReadWrite(0xFF) != 0xFF);

		sd_write(0xFF);
		sd_deselect();
		sd_write(0xFF);

    if(token == 0xFD)
        return SUCCESS;

    return ERROR;
}

uint8_t SD_CARD_RecieveR1(void) {
    UINT i;
    uint8_t response = 0xFF;
    for(i = 0;i < SD_COMMAND_TIMEOUT;i++)
    {
        response = SPIReadWrite(0xFF);
        if((response == 0x00) || (response == 0x01))
        {
						sd_deselect();
						sd_write(0xFF);
            return response;
        }
    }
		sd_select();
		sd_write(0xFF);
    return 0xFF;
}

uint32_t SD_CARD_RecieveR7(void) {
    UINT i = 0, j = 0;
    for(i = 0;i < (SD_COMMAND_TIMEOUT * 1000);i++)
    {
        uint8_t response[5];
        response[0] = SPIReadWrite(0xFF);
        if(!(response[0] & 0x80))
        {
                for(j = 1;j < 5;j++)
                {
                    response[j] = SPIReadWrite(0xFF);
                }
								sd_deselect();
								sd_write(0xFF);
                return ((response[1] << 24) | (response[2] << 16) | (response[3] << 8) | response[4]);
        }
    }
		sd_deselect();
		sd_write(0xFF);
    return 0xFFFFFFFF; // timeout
}

uint32_t SD_CARD_RecieveR3(void) {
    uint32_t ocr = 0;
    UINT response;
    for(int i=0; i < SD_COMMAND_TIMEOUT; i++)
    {
        response = SPIReadWrite(0xFF);
        if(!(response & 0x80))
        {
            ocr = SPIReadWrite(0xFF) << 24;
            ocr |= SPIReadWrite(0xFF) << 16;
            ocr |= SPIReadWrite(0xFF) << 8;
            ocr |= SPIReadWrite(0xFF);
						sd_deselect();
						sd_write(0xFF);
            return ocr;
        }
    }
		sd_deselect();
		sd_write(0xFF);
    return 0xFFFFFFFF; // timeout
}
