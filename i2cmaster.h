#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/twi.h>

extern uint8_t i2c_last_status;

typedef enum
{
    I2C_READ = TW_READ,
    I2C_WRITE = TW_WRITE
} transfer_mode;

typedef enum
{
    I2C_100KHZ,
    I2C_400KHZ,
    I2C_1MHZ,
    I2C_3_2MHZ
} i2c_bit_rate;

static inline void i2c_stop()
{
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

static inline uint8_t i2c_start()
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

    loop_until_bit_is_set(TWCR, TWINT);
    return (i2c_last_status = TW_STATUS);
}

static inline uint8_t i2c_rep_start()
{
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

    loop_until_bit_is_set(TWCR, TWINT);
    return (i2c_last_status = TW_STATUS);
}

static inline uint8_t i2c_send_sla(uint8_t slave_address, transfer_mode mode)
{
    TWDR = (slave_address << 1) | mode;

    TWCR = _BV(TWINT) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);
    return (i2c_last_status = TW_STATUS);
}

static inline uint8_t i2c_send_byte(uint8_t data)
{
    TWDR = data;

    TWCR = _BV(TWINT) | _BV(TWEN);
    loop_until_bit_is_set(TWCR, TWINT);
    return (i2c_last_status = TW_STATUS);
}

static inline uint8_t i2c_receive_byte(uint8_t* data, bool send_ack)
{
    TWCR = _BV(TWINT) | (send_ack ? _BV(TWEA) : 0) | _BV(TWEN);

    loop_until_bit_is_set(TWCR, TWINT);
    *data = TWDR;
    return (i2c_last_status = TW_STATUS);
}

/* Just sets bitrate */
void i2c_init(i2c_bit_rate bit_rate);

/* Sets bitrate of hardware I2C */
void i2c_set_bit_rate(i2c_bit_rate bit_rate);

/* Sends byte vector via I2C
 * Returns number of sent bytes (in case of error it will be lower than count) */
uint16_t i2c_send_vec(const uint8_t* data, uint16_t count);

/* Recives byte vector via I2C, if last vector byte should be
 * the one terminating receive operation set last_nack to true
 * Returns number of received bytes (in case of error will it be lower than count) */
uint16_t i2c_receive_vec(uint8_t* data, uint16_t count, bool last_nack);
