/*
 *******************************************************************************
 * Title: ads1115.h
 * Description: Device driver (I2C) for ADS1115 ADC used with Fine Sun Sensor
 * Update:
 *          0 - Module created [2020/16/1] [NS]
 * Status - REVIEW REQUIRED
 ******************************************************************************* 
 */

#ifndef ADS1115_H
#define ADS1115_H

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <signal.h>

/* I2C ADDRESSING */
#define ADS1115_S_ADDR 0x48  // Device address for ADS1115
#define I2C_BUS "/dev/i2c-1" // I2C bus name

/* REGISTERS */
#define CONVERSION_REG 0x00 // ADC conversion register
#define CONFIG_REG 0x01     // ADC configuration register
#define CONFIG_REG_OS 0x80
#define CONFIG_REG_MUX_0 0x40
#define CONFIG_REG_MUX_1 0x50
#define CONFIG_REG_MUX_2 0x60
#define CONFIG_REG_MUX_3 0x70

typedef union {
    struct
    {
        uint8_t comp_que : 2;
        uint8_t comp_lat : 1;
        uint8_t comp_mode : 1;
        uint8_t comp_pol : 1;
        uint8_t dr : 3;
        uint8_t mode : 1;
        uint8_t pga : 3;
        uint8_t mux : 3;
        uint8_t os : 1;
    };
    uint16_t raw;
} ads1115_config;

/* ADS1115 DATA STRUCTURE */
typedef struct
{
    int fd;
    char fname[40];
} ads1115;

int ads1115_init(ads1115 *dev, uint8_t s_address);
int ads1115_configure(ads1115 *dev, ads1115_config m_con);
int ads1115_read_data(ads1115 *dev, int16_t *data);
int ads1115_read_cont(ads1115 *dev, int16_t *data);
int ads1115_read_config(ads1115 *dev, uint16_t *data);
void ads1115_destroy(ads1115 *dev);

#endif // ADS1115_H