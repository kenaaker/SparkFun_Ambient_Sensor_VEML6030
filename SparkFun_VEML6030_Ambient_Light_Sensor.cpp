/*
  This is a library for SparkFun's VEML6030 Ambient Light Sensor (Qwiic)
  By: Elias Santistevan
  Date: July 2019
  License: This code is public domain but you buy me a beer if you use this and 
  we meet someday (Beerware license).

  Feel like supporting our work? Buy a board from SparkFun!
 */

#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <QMessageBox>
#include <QDebug>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"

/* File hexDump.c created by Ken Aaker on Fri Aug  8 2003. */
extern "C" void hex_dump(const char *title, void *mem, int len) {

    unsigned short i;
    unsigned char *memByte;
    unsigned char *printBytes;
    int print_column_start;
    int cur_print_column;

    printf("%s at %p for %d bytes\n", title, mem, len);
    printBytes = (unsigned char *)mem;
    print_column_start = (16 * 2) + 4;

    cur_print_column = print_column_start;
    for (i = 0, memByte = (unsigned char *)mem; i < len; ++i, ++memByte) {
        if ((i & 15) == 0) { /* new line each 16 bytes */
            if (i != 0) {    /* But skip the line end for 0. */
                printf(" |");
                while (printBytes < memByte) {
                    if ((*printBytes > ' ') && (*printBytes < 'z')) {
                        printf("%c", *printBytes);
                    } else {
                        printf("%c", '.');
                    } /* endif */
                    ++printBytes;
                } /* endwhile */
                printf("|\n");
            } /* endif */
            printf("%p 0x%04x |", (unsigned char *)mem + i, i);
            cur_print_column = print_column_start;
        }                   /* endif */
        if ((i & 3) == 0) { /* a blank each 4 bytes */
            printf(" ");
            --cur_print_column;
        } /* endif */
        printf("%02x", *memByte);
        cur_print_column -= 2;
    } /* endfor */
    /* print some blanks to align the characters with the other character columns */
    for (; cur_print_column > 0; --cur_print_column) {
        printf(" ");
    }
    printf(" |");
    while (printBytes < memByte) {
        if ((*printBytes > ' ') && (*printBytes < 'Z')) {
            printf("%c", *printBytes);
        } else {
            printf("%c", '.');
        } /* endif */
        ++printBytes;
    } /* endwhile */
    printf("|\n");
}

static void delay(unsigned int delay_for_milliseconds) {
    usleep(delay_for_milliseconds * 1000);
}

SparkFun_Ambient_Light::SparkFun_Ambient_Light(int address) {
    static const char *i2c_bus_name = "/dev/i2c-1";

    fd_i2c_file = open(i2c_bus_name, O_RDWR);
    if (fd_i2c_file < 0) {
        QMessageBox file_open_msg;
        file_open_msg.setText("i2c bus didn't open. Quitting");
        file_open_msg.exec();
    } else {
        slave_address = address;
        if (ioctl(fd_i2c_file, I2C_SLAVE, slave_address) < 0) {
            QMessageBox i2c_addr_set_msg;
            i2c_addr_set_msg.setText("Couldn't set light sensor i2c address. Quitting");
            i2c_addr_set_msg.exec();
        } else {
            /*
             * Possible values: .125(1/8), .25(1/4), 1, 2
             * Both .125 and .25 should be used in most cases except darker rooms.
             * A gain of 2 should only be used if the sensor will be covered by a dark
             * glass.
             */
            float gain = .125;
            /*
             * Possible integration times in milliseconds: 800, 400, 200, 100, 50, 25
             * Higher times give higher resolutions and should be used in darker light.
             */
            int integration_time = 50;

            power_on();
            set_gain(gain);
            set_integration_time(integration_time);
        }
    }
} //Constructor for I2C

// REG0x00, bits [12:11]
// This function sets the gain for the Ambient Light Sensor. Possible values
// are 1/8, 1/4, 1, and 2. The highest setting should only be used if the
// sensors is behind dark glass, where as the lowest setting should be used in
// dark rooms. The datasheet suggests always leaving it at around 1/4 or 1/8.
void SparkFun_Ambient_Light::set_gain(float gain_val) {

    uint16_t bits;

    if (gain_val == 1.00)
        bits = 0;
    else if (gain_val == 2.00)
        bits = 1;
    else if (gain_val == .125)
        bits = 2;
    else if (gain_val == .25)
        bits = 3;
    else
        return;

    write_register(SETTING_REG, bits, -GAIN_POS, GAIN_MASK);
}

// REG0x00, bits [12:11]
// This function reads the gain for the Ambient Light Sensor. Possible values
// are 1/8, 1/4, 1, and 2. The highest setting should only be used if the
// sensors is behind dark glass, where as the lowest setting should be used in
// dark rooms. The datasheet suggests always leaving it at around 1/4 or 1/8.
float SparkFun_Ambient_Light::read_gain() {
    uint16_t reg_value; /* Hardware register value. */
    float ret_value;

    reg_value = read_register(SETTING_REG, GAIN_POS, GAIN_MASK); // Get register

    if (reg_value == 0)
        ret_value = 1;
    else if (reg_value == 1)
        ret_value = 2;
    else if (reg_value == 2)
        ret_value = 0.125;
    else if (reg_value == 3)
        ret_value = .25;
    else
        return UNKNOWN_ERROR;

    return ret_value;
}

// REG0x00, bits[9:6]
// This function sets the integration time (the saturation time of light on the
// sensor) of the ambient light sensor. Higher integration time leads to better
// resolution but slower sensor refresh times.
void SparkFun_Ambient_Light::set_integration_time(uint16_t time) {

    uint16_t bits;
    uint8_t reg_value;

    if (time == 100) // Default setting.
        bits = 0;
    else if (time == 200)
        bits = 1;
    else if (time == 400)
        bits = 2;
    else if (time == 800)
        bits = 3;
    else if (time == 50)
        bits = 8;
    else if (time == 25)
        bits = 12;
    else
        return;

    write_register(SETTING_REG, bits, -INTEGRATION_TIME_POS, INTEGRATION_TIME_MASK);
}

// REG0x00, bits[9:6]
// This function reads the integration time (the saturation time of light on the
// sensor) of the ambient light sensor. Higher integration time leads to better
// resolution but slower sensor refresh times.
uint16_t SparkFun_Ambient_Light::read_integtration_time() {

    uint16_t reg_value = read_register(SETTING_REG, INTEGRATION_TIME_POS, INTEGRATION_TIME_MASK);

    if (reg_value == 0)
        return 100;
    else if (reg_value == 1)
        return 200;
    else if (reg_value == 2)
        return 400;
    else if (reg_value == 3)
        return 800;
    else if (reg_value == 8)
        return 50;
    else if (reg_value == 12)
        return 25;
    else
        return UNKNOWN_ERROR;
}

// REG0x00, bits[5:4]
// This function sets the persistence protect number.
void SparkFun_Ambient_Light::set_protect(uint8_t prot_value) {

    uint16_t bits;

    if (prot_value == 1)
        bits = 0;
    else if (prot_value == 2)
        bits = 1;
    else if (prot_value == 4)
        bits = 2;
    else if (prot_value == 8)
        bits = 3;
    else
        return;

    write_register(SETTING_REG, bits, -PERSISTENT_PROTECT_POS, PERSISTENCE_PROTECT_MASK);
}

// REG0x00, bits[5:4]
// This function reads the persistence protect number.
uint8_t SparkFun_Ambient_Light::read_protect() {

    uint16_t reg_value = read_register(SETTING_REG, PERSISTENT_PROTECT_POS, PERSISTENCE_PROTECT_MASK);

    if (reg_value == 0)
        return 1;
    else if (reg_value == 1)
        return 2;
    else if (reg_value == 2)
        return 4;
    else if (reg_value == 3)
        return 8;
    else
        return UNKNOWN_ERROR;
}

// REG0x00, bit[1]
// This function enables the Ambient Light Sensor's interrupt.
void SparkFun_Ambient_Light::enable_interrupt() {

    write_register(SETTING_REG, ENABLE, -INTERRUPT_ENABLE_POS, INTERRUPT_ENABLE_MASK);
}

// REG0x00, bit[1]
// This function disables the Ambient Light Sensor's interrupt.
void SparkFun_Ambient_Light::disable_interrupt() {

    write_register(SETTING_REG, DISABLE, -INTERRUPT_ENABLE_POS, INTERRUPT_ENABLE_MASK);
}

// REG0x00, bit[1]
// This function checks if the interrupt is enabled or disabled.
uint8_t SparkFun_Ambient_Light::read_interrrupt_setting() {

    uint16_t reg_value = read_register(SETTING_REG, INTERRUPT_ENABLE_POS, INTERRUPT_ENABLE_MASK);
    return reg_value;
}

// REG0x00, bit[0]
// This function powers down the Ambient Light Sensor. The light sensor will
// hold onto the last light reading which can be acessed while the sensor is
// shut down. 0.5 micro Amps are consumed while shutdown.
void SparkFun_Ambient_Light::shut_down() {
    write_register(SETTING_REG, SHUTDOWN, -SHUTDOWN_POS, SHUTDOWN_MASK);
}

// REG0x00, bit[0]
// This function powers up the Ambient Light Sensor. The last value that was
// read during shut down will be overwritten on the sensor's subsequent read.
// After power up, a small 4ms delay is applied to give time for the internal
// osciallator and signal processor to power up.
void SparkFun_Ambient_Light::power_on() {

    write_register(SETTING_REG, POWER, -SHUTDOWN_POS, SHUTDOWN_MASK);
    delay(4);
}

// REG0x03, bit[0]
// This function enables the current power save mode value and puts the Ambient
// Light Sensor into power save mode.
void SparkFun_Ambient_Light::enable_power_save() {

    write_register(POWER_SAVE_REG, ENABLE, -POWER_SAVE_MODE_POS, POWER_SAVE_MODE_ENABLE_MASK);
}

// REG0x03, bit[0]
// This function disables the current power save mode value and pulls the Ambient
// Light Sensor out of power save mode.
void SparkFun_Ambient_Light::disable_power_save() {

    write_register(POWER_SAVE_REG, DISABLE, -POWER_SAVE_MODE_POS, POWER_SAVE_MODE_ENABLE_MASK);
}

// REG0x03, bit[0]
// This function checks to see if power save mode is enabled or disabled.
uint8_t SparkFun_Ambient_Light::read_power_save_enabled() {

    uint16_t reg_value = read_register(POWER_SAVE_REG, POWER_SAVE_MODE_ENABLE_POS, POWER_SAVE_MODE_ENABLE_MASK);
    return reg_value;
}

// REG0x03, bit[2:1]
// This function sets the power save mode value. It takes a value of 1-4. Each
// incrementally higher value descreases the sampling rate of the sensor and so
// increases power saving. The datasheet suggests enabling these modes when
// continually sampling the sensor.
void SparkFun_Ambient_Light::set_power_save_mode(uint16_t mode_value) {

    uint16_t bits;

    if (mode_value == 1)
        bits = 0;
    else if (mode_value == 2)
        bits = 1;
    else if (mode_value == 3)
        bits = 2;
    else if (mode_value == 4)
        bits = 3;
    else
        return;

    write_register(POWER_SAVE_REG, bits, -POWER_SAVE_MODE_ENABLE_POS, POWER_SAVE_MODE_MASK);
}

// REG0x03, bit[2:1]
// This function reads the power save mode value. The function above takes a value of 1-4. Each
// incrementally higher value descreases the sampling rate of the sensor and so
// increases power saving. The datasheet suggests enabling these modes when
// continually sampling the sensor.
uint8_t SparkFun_Ambient_Light::read_power_save_mode() {

    uint16_t reg_value = read_register(POWER_SAVE_REG, POWER_SAVE_MODE_ENABLE_POS, POWER_SAVE_MODE_ENABLE_MASK);

    if (reg_value == 0)
        return 1;
    else if (reg_value == 1)
        return 2;
    else if (reg_value == 2)
        return 3;
    else if (reg_value == 3)
        return 4;
    else
        return UNKNOWN_ERROR;
}

// REG0x06, bits[15:14]
// This function reads the interrupt register to see if an interrupt has been
// triggered. There are two possible interrupts: a lower limit and upper limit
// threshold, both set by the user.
uint8_t SparkFun_Ambient_Light::read_interrupt() {

    uint16_t reg_value = read_register(INTERRUPT_STATUS_REG, INTERRUPT_STATUS_POS, INTERRUPT_STATUS_MASK);

    if (reg_value == 0)
        return NO_INT;
    else if (reg_value == 1)
        return INT_HIGH;
    else if (reg_value == 2)
        return INT_LOW;
    else
        return UNKNOWN_ERROR;
}

// REG0x02, bits[15:0]
// This function sets the lower limit for the Ambient Light Sensor's interrupt.
// It takes a lux value as its paramater.
void SparkFun_Ambient_Light::set_interrupt_low_threshold(uint32_t lux_value) {

    if ((lux_value < 0) || (lux_value > 120000)) {
        return;
    } else {
        uint16_t lux_bits;

        lux_bits = calculate_bits(lux_value);
        write_register(L_THRESHOLD_REG, lux_bits, -L_THRESHOLD_POS, L_THRESHOLD_MASK);
    }
}

// REG0x02, bits[15:0]
// This function reads the lower limit for the Ambient Light Sensor's interrupt.
uint32_t SparkFun_Ambient_Light::read_low_threshold() {

    uint16_t thresh_value = read_register(L_THRESHOLD_REG, L_THRESHOLD_POS, L_THRESHOLD_MASK);
    uint32_t thresh_lux = calculate_lux(thresh_value);
    return thresh_lux;
}

// REG0x01, bits[15:0]
// This function sets the upper limit for the Ambient Light Sensor's interrupt.
// It takes a lux value as its paramater.
void SparkFun_Ambient_Light::set_interrupt_high_threshold(uint32_t lux_value) {

    if ((lux_value < 0) || (lux_value > 120000)) {
        return;
    } else {
        uint16_t lux_bits = calculate_bits(lux_value);
        write_register(H_THRESHOLD_REG, lux_bits, -H_THRESHOLD_POS, H_THRESHOLD_MASK);
    }
}

// REG0x01, bits[15:0]
// This function reads the upper limit for the Ambient Light Sensor's interrupt.
uint32_t SparkFun_Ambient_Light::read_high_threshold() {

    uint16_t thresh_value = read_register(H_THRESHOLD_REG, H_THRESHOLD_POS, H_THRESHOLD_MASK);
    uint32_t thresh_lux = calculate_lux(thresh_value);
    return thresh_lux;
}

// REG[0x04], bits[15:0]
// This function gets the sensor's ambient light's lux value. The lux value is
// determined based on current gain and integration time settings. If the lux
// value exceeds 1000 then a compensation formula is applied to it.
uint32_t SparkFun_Ambient_Light::read_light() {

    uint16_t light_bits = read_register(AMBIENT_LIGHT_DATA_REG, AMBIENT_LIGHT_DATA_POS, AMBIENT_LIGHT_DATA_MASK);
    uint32_t lux_value = calculate_lux(light_bits);

    if (lux_value > 1000) {
        uint32_t compensated_lux = lux_compensation(lux_value);
        return compensated_lux;
    } else {
        return lux_value;
    }
}

// REG[0x05], bits[15:0]
// This function gets the sensor's ambient light's lux value. The lux value is
// determined based on current gain and integration time settings. If the lux
// value exceeds 1000 then a compensation formula is applied to it.
uint32_t SparkFun_Ambient_Light::read_white_light() {

    uint16_t light_bits = read_register(WHITE_LIGHT_DATA_REG, WHITE_LIGHT_DATA_POS, WHITE_LIGHT_DATA_MASK);
    uint32_t lux_value = calculate_lux(light_bits);

    if (lux_value > 1000) {
        uint32_t compLux = lux_compensation(lux_value);
        return compLux;
    } else {
        return lux_value;
    }
}

// This function compensates for lux values over 1000. From datasheet:
// "Illumination values higher than 1000 lx show non-linearity. This
// non-linearity is the same for all sensors, so a compensation forumla..."
// etc. etc.
uint32_t SparkFun_Ambient_Light::lux_compensation(uint32_t lux_value) {

    // Polynomial is pulled from pg 10 of the datasheet.
    uint32_t compensated_lux = (.00000000000060135 * (pow(lux_value, 4))) -
                        (.0000000093924 * (pow(lux_value, 3))) +
                        (.000081488 * (pow(lux_value, 2))) +
                        (1.0023 * lux_value);
    return compensated_lux;
}

// The lux value of the Ambient Light sensor depends on both the gain and the
// integration time settings. This function determines which conversion value
// to use by using the bit representation of the gain as an index to look up
// the conversion value in the correct integration time array. It then converts
// the value and returns it.
uint32_t SparkFun_Ambient_Light::calculate_lux(uint16_t light_bits) {

    float lux_conversion;
    uint8_t conv_position;
    float gain = read_gain();
    uint16_t integration_time = read_integtration_time();

    // Here the gain is checked to get the position of the conversion value
    // within the integration time arrays. These values also represent the bit
    // values for setting the gain.
    if (gain == 2.00)
        conv_position = 0;
    else if (gain == 1.00)
        conv_position = 1;
    else if (gain == .25)
        conv_position = 2;
    else if (gain == .125)
        conv_position = 3;
    else
        return UNKNOWN_ERROR;

    // Here we check the integration time which determines which array we probe
    // at the position determined above.
    if (integration_time == 800)
        lux_conversion = eight_high_integration_time[conv_position];
    else if (integration_time == 400)
        lux_conversion = four_high_integration_time[conv_position];
    else if (integration_time == 200)
        lux_conversion = two_high_integration_time[conv_position];
    else if (integration_time == 100)
        lux_conversion = one_high_integration_time[conv_position];
    else if (integration_time == 50)
        lux_conversion = fifty_integration_time[conv_position];
    else if (integration_time == 25)
        lux_conversion = twenty_integration_time[conv_position];
    else
        return UNKNOWN_ERROR;

    /* Multiply the value from the 16 bit register to the conversion value and return it.*/
    uint32_t calculated_lux = (lux_conversion * light_bits);
    return calculated_lux;
}

// This function does the opposite calculation then the function above. The interrupt
// threshold values given by the user are dependent on the gain and
// intergration time settings. As a result the lux value needs to be
// calculated with the current settings and this function accomplishes
// that.
uint16_t SparkFun_Ambient_Light::calculate_bits(uint32_t lux_value) {

    float lux_conversion;
    uint8_t conv_position;

    float gain = read_gain();
    float integration_time = read_integtration_time();
    // Here the gain is checked to get the position of the conversion value
    // within the integration time arrays. These values also represent the bit
    // values for setting the gain.
    if (gain == 2.00)
        conv_position = 0;
    else if (gain == 1.00)
        conv_position = 1;
    else if (gain == .25)
        conv_position = 2;
    else if (gain == .125)
        conv_position = 3;
    else
        return UNKNOWN_ERROR;

    // Here we check the integration time which determines which array we probe
    // at the position determined above.
    if (integration_time == 800)
        lux_conversion = eight_high_integration_time[conv_position];
    else if (integration_time == 400)
        lux_conversion = four_high_integration_time[conv_position];
    else if (integration_time == 200)
        lux_conversion = two_high_integration_time[conv_position];
    else if (integration_time == 100)
        lux_conversion = one_high_integration_time[conv_position];
    else if (integration_time == 50)
        lux_conversion = fifty_integration_time[conv_position];
    else if (integration_time == 25)
        lux_conversion = twenty_integration_time[conv_position];
    else
        return UNKNOWN_ERROR;

    // Divide the value of lux bythe conversion value and return
    // it.
    uint16_t calculated_bits = (lux_value / lux_conversion);
    return calculated_bits;
}

// This function reads a 16 bit register. It takes the register's address as its parameter.
uint16_t SparkFun_Ambient_Light::raw_read_register(VEML6030_16BIT_REGISTERS read_reg) {
    uint16_t reg_value;
    struct i2c_msg messages[2];
    struct i2c_rdwr_ioctl_data message_set[1];
    uint8_t in_buffer[2];
    uint8_t out_buffer[1];

    /* Set up the output operation first. */
    messages[0].addr = slave_address;
    messages[0].flags = 0;
    messages[0].len = sizeof(out_buffer);
    messages[0].buf = out_buffer;

    /* Now set up the input operation. */
    messages[1].addr = slave_address;
    messages[1].flags = (I2C_M_RD | I2C_M_NOSTART);
    messages[1].len = sizeof(in_buffer);
    messages[1].buf = in_buffer;

    /* Now arrange things for the ioctl. */
    message_set[0].msgs = messages;
    message_set[0].nmsgs = 2;

    out_buffer[0] = read_reg;

    in_buffer[0] = read_reg;
    in_buffer[1] = 0;
    if (ioctl(fd_i2c_file, I2C_RDWR, &message_set) < 0) {
        perror("ioctl(I2C_RDWR) in read_register");
        reg_value =0xffff;
    } else {
        reg_value = (in_buffer[1] << 8) | in_buffer[0];
    }
    return reg_value;
}

// This function writes to a 16 bit register. Paramaters include the register's address and the reg_value to write.
void SparkFun_Ambient_Light::raw_write_register(VEML6030_16BIT_REGISTERS write_reg, uint16_t output_reg_value) {
    struct i2c_msg messages[1];
    struct i2c_rdwr_ioctl_data message_set[1];
    uint8_t out_buffer[3];

    /* Set up the output operation. */
    messages[0].addr = slave_address;
    messages[0].flags = 0;
    messages[0].len = sizeof(out_buffer);
    messages[0].buf = out_buffer;

    /* Now arrange things for the ioctl. */
    message_set[0].msgs = messages;
    message_set[0].nmsgs = 1;

    out_buffer[0] = (uint8_t)(write_reg & 0xff);
    out_buffer[1] = (output_reg_value & 0xff);
    out_buffer[2] = ((output_reg_value >> 8) & 0xff);
    if (ioctl(fd_i2c_file, I2C_RDWR, &message_set) < 0) {
        perror("ioctl(I2C_RDWR) in write_register");
    }
}

// This function reads a 16 bit register. It takes the register's address as its parameter.
// The mask_value is used to mask the raw register value, then the value is shifted by the shift_value.
uint16_t SparkFun_Ambient_Light::read_register(VEML6030_16BIT_REGISTERS read_reg,
                                               const int shift_value, const uint16_t mask_value) {
    uint16_t reg_value;
    int shift_amount;

    reg_value = raw_read_register(read_reg);
    shift_amount = abs(shift_value);

    reg_value = (reg_value & mask_value);
    if (shift_value < 0) {
        reg_value = reg_value << shift_amount;
    } else if (shift_value > 0) {
        reg_value = reg_value >> shift_amount;
    }
    return reg_value;
}

// This function writes to a 16 bit register. Paramaters include the register's address, a mask
// for bits that are ignored, and the bits to write.
void SparkFun_Ambient_Light::write_register(VEML6030_16BIT_REGISTERS write_reg, uint16_t output_bits,
                                            const int shift_value, const uint16_t output_mask) {
    int shift_amount;
    uint16_t existing_register;
    uint16_t updated_register;

    existing_register = raw_read_register(write_reg);
    updated_register = existing_register & ~output_mask;

    shift_amount = abs(shift_value);
    if (shift_value < 0) {
        updated_register = updated_register | (output_bits << shift_amount);
    } else if (shift_value > 0) {
        updated_register = updated_register | (output_bits >> shift_amount);
    }

    raw_write_register(write_reg, updated_register);
}
