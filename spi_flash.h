#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware/spi.h"

#define FLASH_CMD_WRITE_ENABLE (0x06)
#define FLASH_CMD_VOLATILE_SR_WRITE_ENABLE (0x50)
#define FLASH_CMD_WRITE_DISABLE (0x04)
#define FLASH_CMD_READ_STATUS_REG_1 (0x05)
#define FLASH_CMD_READ_STATUS_REG_2 (0x35)
#define FLASH_CMD_WRITE_STATUS_REG (0x01)
#define FLASH_CMD_PAGE_PROGRAM (0x02)
#define FLASH_CMD_SECTOR_ERASE (0x20)
#define FLASH_CMD_BLOCK_ERASE_32K (0x52)
#define FLASH_CMD_BLOCK_ERASE_64K (0xD8)
#define FLASH_CMD_CHIP_ERASE (0xC7)
#define FLASH_CMD_CHIP_ERASE_ALT (0x60)
#define FLASH_CMD_ERASE_PROGRAM_SUSPEND (0x75)
#define FLASH_CMD_ERASE_PROGRAM_RESUME (0x7A)
#define FLASH_CMD_POWER_DOWN (0xB9)
#define FLASH_CMD_READ_DATA (0x03)
#define FLASH_CMD_FAST_READ (0x0B)
#define FLASH_CMD_RELEASE_POWERDOWN (0xAB)
#define FLASH_CMD_DEVICE_ID (0x90)
#define FLASH_CMD_JEDEC_ID (0x9F)
#define FLASH_CMD_READ_UNIQUE_ID (0x4B)
#define FLASH_CMD_READ_SFDP_REG (0x5A)
#define FLASH_CMD_ERASE_SECURITY_REG (0x44)
#define FLASH_CMD_PROGRAM_SECURITY_REG (0x42)
#define FLASH_CMD_READ_SECURITY_REG (0x48)
#define FLASH_CMD_ENABLE_QPI (0x38)
#define FLASH_CMD_ENABLE_RESET (0x66)
#define FLASH_CMD_RESET (0x99)
/** Dual spi, quad spi and QPI instructions are not supported yet */

typedef union
{
    struct
    {
        uint8_t BUSY:1;
        uint8_t WEL:1;
        uint8_t BP0:1;
        uint8_t BP1:1;
        uint8_t BP2:1;
        uint8_t TB:1;
        uint8_t SEC:1;
        uint8_t SRP0:1;  
    }__attribute__((packed)) bits;
    uint8_t byte;
} flash_status_register_1;

typedef union
{
    struct
    {
        uint8_t SRP1:1;
        uint8_t QE:1;
        uint8_t R:1;
        uint8_t LB1:1;
        uint8_t LB2:1;
        uint8_t LB3:1;
        uint8_t CMP:1;
        uint8_t SUS:1;
    }__attribute__((packed)) bits;
    uint8_t byte;
} flash_status_register_2;

typedef struct
{
    spi_inst_t* spi;
    struct{
        int WPN;
        int HOLDN;
        int SCK;
        int TX;
        int RX;
        int CSN;
    } pin;
} spi_flash_t;

void spi_flash_init(spi_flash_t* flash);

void __not_in_flash_func(spi_flash_send_cmd)(spi_flash_t* flash,uint8_t cmd, uint8_t* tx_data, int tx_len, uint8_t* rx_data, int rx_len);

bool __not_in_flash_func(spi_flash_is_busy)(spi_flash_t* flash);

#endif