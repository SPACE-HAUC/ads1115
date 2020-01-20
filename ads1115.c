
#include "ads1115.h"

int ads1115_init(ads1115 *dev, uint8_t s_address)
{
    dev->fd = open(I2C_BUS, O_RDWR);
    if (dev->fd < 0)
    {
        perror("[ERROR] Could not create a file descriptor.");
        return -1;
    }

    if (ioctl(dev->fd, I2C_SLAVE, s_address) < 0)
    {
        perror("[ERROR] Could not assign device.");
        return -1;
    }

    return 1;
}

int ads1115_configure(ads1115 *dev, ads1115_config m_con)
{
    uint8_t buf[3];
    buf[0] = CONFIG_REG;
    buf[1] = m_con.raw >> 8;
    buf[2] = m_con.raw & 0xFF;

    if (write(dev->fd, buf, 3) < 3)
    {
        perror("[ERROR] Could not configure device.");
        return -1;
    }
    return 1;
}

int ads1115_read_data(ads1115 *dev, int16_t *data)
{
    uint8_t buf[3], status = 1;

    ads1115_config m_con;
    m_con.raw = 0;
    m_con.os = 1;
    m_con.mux = 0x04;
    m_con.pga = 1;
    m_con.mode = 1;
    m_con.dr = 6;

    do
    {
        buf[0] = CONFIG_REG;
        buf[1] = m_con.raw >> 8;
        buf[2] = m_con.raw & 0xFF;

        if (write(dev->fd, buf, 3) < 3)
        {
            perror("[ERROR] Could not access target register.");
            return -1;
        }

        if (write(dev->fd, buf, 1) < 1)
        {
            perror("Could not stat read");
        }

        if (read(dev->fd, &buf[1], 2) < 2)
        {
            perror("Could not stat read command");
        }
        m_con.raw = (((uint16_t)buf[1]) << 8) | buf[2];
    } while (m_con.os > 0);

    usleep(1500);

    do
    {
        buf[0] = CONFIG_REG;
        if (write(dev->fd, buf, 1) < 1)
        {
            perror("Could not stat read");
        }

        if (read(dev->fd, &buf[1], 2) < 2)
        {
            perror("Could not stat read command");
        }
        m_con.raw = (((uint16_t)buf[1]) << 8) | buf[2];
    } while (m_con.os == 0);

    m_con.raw = 0;
    m_con.os = 1;
    m_con.mux = 0x04;
    m_con.pga = 1;
    m_con.mode = 1;
    m_con.dr = 6;
    for (int i = 0; i < 4; i++)
    {
        buf[0] = CONVERSION_REG;
        buf[1] = 0x00;
        buf[2] = 0x00;
        if (write(dev->fd, buf, 1) < 1)
        {
            perror("Could not stat read");
            status = 0;
        }

        if (read(dev->fd, &buf[1], 2) < 2)
        {
            perror("Could not stat read command");
            status = 0;
        }
        data[i] = (((uint16_t)buf[1]) << 8) | buf[2];
        buf[0] = CONFIG_REG;
        m_con.mux++;
        buf[1] = m_con.raw >> 8;

        buf[2] = m_con.raw & 0xff;
        if (write(dev->fd, buf, 3) < 3)
        {
            perror("Could not stat mux change");
            status = 0;
        }
    }
    return status;
}

int ads1115_read_cont(ads1115 *dev, int16_t *data)
{
    uint8_t buf[3], status = 1;

    ads1115_config m_con;
    m_con.raw = 0;
    m_con.os = 0;
    m_con.mux = 0x04;
    m_con.pga = 1;
    m_con.mode = 0;
    m_con.dr = 5;

    buf[0] = CONFIG_REG;
    buf[1] = m_con.raw >> 8;
    buf[2] = m_con.raw & 0xFF;

    if (write(dev->fd, buf, 3) < 3)
    {
        perror("[ERROR] Could not access target register.");
        return -1;
    }

    m_con.raw = 0;
    m_con.os = 0;
    m_con.mux = 0x04;
    m_con.pga = 1;
    m_con.mode = 0;
    m_con.dr = 5;
    for (int i = 0; i < 4; i++)
    {
        buf[0] = CONVERSION_REG;
        buf[1] = 0x00;
        buf[2] = 0x00;
        if (write(dev->fd, buf, 1) < 1)
        {
            perror("Could not stat read");
            status = 0;
        }

        if (read(dev->fd, &buf[1], 2) < 2)
        {
            perror("Could not stat read command");
            status = 0;
        }
        data[i] = (((uint16_t)buf[1]) << 8) | buf[2];
        buf[0] = CONFIG_REG;
        m_con.mux++;
        buf[1] = m_con.raw >> 8;

        buf[2] = m_con.raw & 0xff;
        if (write(dev->fd, buf, 3) < 3)
        {
            perror("Could not stat mux change");
            status = 0;
        }
    }

    return status;
}

int ads1115_read_config(ads1115 *dev, uint16_t *data)
{
    uint8_t buf[3];

    buf[0] = CONFIG_REG;
    buf[1] = 0;
    buf[2] = 0;

    if (write(dev->fd, buf, 1) < 1)
    {
        perror("[ERROR] Could not select config register.");
        return -1;
    }

    if (read(dev->fd, &buf[1], 2) < 2)
    {
        perror("[ERROR] Could not read config register.");
        return -1;
    }
    *data = (((uint16_t)(buf[1]) << 8) | buf[2]);
    return 1;
}

void ads1115_destroy(ads1115 *dev)
{
    close(dev->fd);
    free(dev);
}
