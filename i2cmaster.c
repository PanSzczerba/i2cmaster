#include "i2cmaster.h"

#define PRESCALER_MAX 3

uint8_t i2c_last_status;

void i2c_init(i2c_bit_rate bit_rate)
{
    i2c_set_bit_rate(bit_rate);
}

void i2c_set_bit_rate(i2c_bit_rate bit_rate)
{
    uint32_t bit_rate_tmp;
    switch(bit_rate)
    {
    case I2C_100KHZ:
        bit_rate_tmp = 100000;
        break;
    case I2C_400KHZ:
        bit_rate_tmp = 400000;
        break;
    case I2C_1MHZ:
        bit_rate_tmp = 1000000;
        break;
    case I2C_3_2MHZ:
        bit_rate_tmp = 3200000;
        break;
    }

    uint8_t prescaler = 0;
    bit_rate_tmp = (F_CPU / bit_rate_tmp - 16) / 2;
    while(bit_rate_tmp >= UINT8_MAX && prescaler <= PRESCALER_MAX)
    {
        bit_rate_tmp >>= 2;
        prescaler++;
    }
    if(bit_rate_tmp > UINT8_MAX)
        bit_rate_tmp = UINT8_MAX;

    TWBR = (uint8_t) bit_rate_tmp;
    TWSR = prescaler;

}

uint16_t i2c_send_vec(const uint8_t* data, uint16_t count)
{
    uint16_t bytes_sent = 0;

    for(; bytes_sent < count; bytes_sent++)
        if(i2c_send_byte(data[bytes_sent]) != TW_MT_DATA_ACK)
            return bytes_sent;

    return bytes_sent;
}

uint16_t i2c_receive_vec(uint8_t* data, uint16_t count, bool last_nack)
{
    uint8_t bytes_received = 0;

    for(; bytes_received < count - 1; bytes_received++)
        if(i2c_receive_byte(&data[bytes_received], true) != TW_MR_DATA_ACK)
            return bytes_received;

    if(i2c_receive_byte(&data[bytes_received], !last_nack) == TW_MR_DATA_NACK)
        bytes_received++;

    return bytes_received;
}
