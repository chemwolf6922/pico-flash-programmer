#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "spi_flash.h"

#define FLASH_SPI_WPN_PIN (0)
#define FLASH_SPI_HOLDN_PIN (1)
#define FLASH_SPI_SCK_PIN (2)
#define FLASH_SPI_TX_PIN (3)
#define FLASH_SPI_RX_PIN (4)
#define FLASH_SPI_CSN_PIN (5)

typedef struct
{
    uint8_t cmd;
    uint8_t wait_busy;
    uint16_t write_len;
    uint16_t read_len;
}__attribute__((packed)) packet_header_t;

typedef struct
{
    uint8_t code;
    uint16_t len;
}__attribute__((packed)) reply_header_t;

#define SPI_FLASH_CODE_SUCCESS (0)
#define SPI_FLASH_CODE_ERROR (1)

static spi_flash_t flash = {
    .spi = spi0,
    .pin = {
        .CSN = FLASH_SPI_CSN_PIN,
        .HOLDN = FLASH_SPI_HOLDN_PIN,
        .WPN = FLASH_SPI_WPN_PIN,
        .SCK = FLASH_SPI_SCK_PIN,
        .TX = FLASH_SPI_TX_PIN,
        .RX = FLASH_SPI_RX_PIN
    }
};


int main()
{

    stdio_init_all();

    spi_flash_init(&flash);

    for(;;)
    {
        uint8_t header_buffer[sizeof(packet_header_t)] = {0};
        /** wait for the first command */
        for(;;)
        {
            /** getchar will block USB CDC serial from opening */
            int c = getchar_timeout_us(0);
            if(c>=0)
            {
                header_buffer[0] = c;
                break;
            }
            sleep_ms(5);
        }
        for(int i=1;i<sizeof(header_buffer);i++){
            int c = getchar();
            header_buffer[i] = c;
        }
        packet_header_t header = {0};
        memcpy(&header,header_buffer,sizeof(packet_header_t));
        uint8_t* tx_buffer = NULL;
        if(header.write_len>0)
        {
            tx_buffer = malloc(header.write_len);
            if(!tx_buffer)
                goto error;
            for(int i=0;i<header.write_len;i++){
                int c = getchar();
                tx_buffer[i] = c;
            }
        }
        uint8_t* rx_buffer = NULL;
        if(header.read_len>0)
        {
            rx_buffer = malloc(header.read_len);
            if(!rx_buffer)
            {
                free(tx_buffer);
                goto error;
            }
        }
        spi_flash_send_cmd(&flash,header.cmd,tx_buffer,header.write_len,rx_buffer,header.read_len);
        if(tx_buffer)
            free(tx_buffer);
        if(header.wait_busy)
        {
            for(;;)
            {
                sleep_us(100);
                if(!spi_flash_is_busy(&flash))
                    break;
            }
        }
        reply_header_t reply = {
            .code = SPI_FLASH_CODE_SUCCESS,
            .len = header.read_len
        };
        for(int i=0;i<sizeof(reply_header_t);i++)
        {
            putchar_raw(((uint8_t*)&reply)[i]);
        }
        for(int i=0;i<reply.len;i++)
        {
            putchar_raw(rx_buffer[i]);
        }
        if(rx_buffer)
            free(rx_buffer);
    }
error:;
    {
        reply_header_t reply = {
            .code = SPI_FLASH_CODE_ERROR,
            .len = 0
        };
        uint8_t* reply_buffer = (uint8_t*)&reply;
        for(int i=0;i<sizeof(reply_header_t);i++)
        {
            putchar_raw(reply_buffer[i]);
        }
    }
    return 0;
}