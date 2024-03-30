#include "spi_flash.h"
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

/** 5ns of enalbe disable time */
#define SPI_CS_ENABLE(pin) {\
    asm volatile("nop \n nop \n nop");\
    gpio_put((pin),false);\
    asm volatile("nop \n nop \n nop");\
}
#define SPI_CS_DISABLE(pin) {\
    asm volatile("nop \n nop \n nop");\
    gpio_put((pin),true);\
    asm volatile("nop \n nop \n nop");\
}

void spi_flash_init(spi_flash_t* flash)
{
    /** init spi */
    spi_init(flash->spi,10000000);    /** 10MHz */
    gpio_set_function(flash->pin.SCK,GPIO_FUNC_SPI);
    gpio_set_function(flash->pin.TX,GPIO_FUNC_SPI);
    gpio_set_function(flash->pin.RX,GPIO_FUNC_SPI);
    /** init gpios */
    gpio_init(flash->pin.WPN);
    gpio_set_dir(flash->pin.WPN,GPIO_OUT);
    gpio_put(flash->pin.WPN,1);
    gpio_init(flash->pin.HOLDN);
    gpio_set_dir(flash->pin.HOLDN,GPIO_OUT);
    gpio_put(flash->pin.HOLDN,1);
    gpio_init(flash->pin.CSN);
    gpio_set_dir(flash->pin.CSN,GPIO_OUT);
    gpio_put(flash->pin.CSN,1);
}

void __not_in_flash_func(spi_flash_send_cmd)(spi_flash_t* flash,uint8_t cmd, uint8_t* tx_data, int tx_len, uint8_t* rx_data, int rx_len)
{
    SPI_CS_ENABLE(flash->pin.CSN);
    spi_write_blocking(spi0,&cmd,1);
    if((tx_data != NULL) && (tx_len > 0))
        spi_write_blocking(spi0,tx_data,tx_len);
    if((rx_data != NULL) && (rx_len > 0))
        spi_read_blocking(spi0,0,rx_data,rx_len);
    SPI_CS_DISABLE(flash->pin.CSN);
}

bool __not_in_flash_func(spi_flash_is_busy)(spi_flash_t* flash)
{
    flash_status_register_1 reg = {0};
    spi_flash_send_cmd(flash,FLASH_CMD_READ_STATUS_REG_1,NULL,0,&reg.byte,1);
    return !!reg.bits.BUSY;
}