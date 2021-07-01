#ifndef _SPARKFUN_VEML6030_H_
#define _SPARKFUN_VEML6030_H_

#define ENABLE 0x01
#define DISABLE 0x00
#define SHUTDOWN 0x01
#define POWER 0x00
#define NO_INT 0x00
#define INT_HIGH 0x01
#define INT_LOW 0x02
#define UNKNOWN_ERROR 0xFF

// 7-Bit address options
static const int default_light_sensor_address = 0x48;
static const int alternate_light_sensor_address = 0x10;

enum VEML6030_16BIT_REGISTERS {
    SETTING_REG = 0x00,
    H_THRESHOLD_REG,
    L_THRESHOLD_REG,
    POWER_SAVE_REG,
    AMBIENT_LIGHT_DATA_REG,
    WHITE_LIGHT_DATA_REG,
    INTERRUPT_STATUS_REG
};

enum VEML6030_16BIT_REG_MASKS {
    GAIN_MASK = 0x1800,             /* Register 0 SETTING_REG */
    INTEGRATION_TIME_MASK = 0x03C0, /* Register 0 SETTING_REG */
    PERSISTENCE_PROTECT_MASK = 0x030,/* Register 0 SETTING_REG */
    INTERRUPT_ENABLE_MASK = 0x2,    /* Register 0 SETTING_REG */
    SHUTDOWN_MASK = 0x01,           /* Register 0 SETTING_REG */
    H_THRESHOLD_MASK = 0xFFFF,      /* Register 1 (High H_THRESHOLD_REG) */
    L_THRESHOLD_MASK = 0xFFFF,      /* Register 2 (Low L_THRESHOLD_REG) */
    POWER_SAVE_MODE_MASK = 0x06,    /* Register 3 POWER_SAVE_REG */
    POWER_SAVE_MODE_ENABLE_MASK = 0x01,/* Register 3 POWER_SAVE_REG */
    AMBIENT_LIGHT_DATA_MASK = 0xFFFF,/* Register 4 AMBIENT_LIGHT_DATA_REG */
    WHITE_LIGHT_DATA_MASK = 0xFFFF, /* Register 5 WHITE_LIGHT_DATA_REG */
    INTERRUPT_STATUS_MASK = 0xC000  /* Register 6 INTERRUPT_STATUS_REG */
};

enum REGISTER_BIT_POSITIONS {
    NO_SHIFT = 0,
    INTERRUPT_ENABLE_POS = 1,       /* Register 0 SETTING_REG */
    PERSISTENT_PROTECT_POS = 4,              /* Register 0 SETTING_REG */
    INTEGRATION_TIME_POS = 6,       /* Register 0 SETTING_REG */
    GAIN_POS = 11,                  /* Register 0 SETTING_REG */
    INTERRUPT_POS = 14,             /* Register 0 SETTING_REG */
    SHUTDOWN_POS = 0,               /* Register 0 SETTING_REG */
    H_THRESHOLD_POS = 0,            /* Register 1 (High H_THRESH_REG) */
    L_THRESHOLD_POS = 0,            /* Register 2 (Low L_THRESH_REG) */
    POWER_SAVE_MODE_POS = 1,        /* Register 3 POWER_SAVE_REG */
    POWER_SAVE_MODE_ENABLE_POS = 0, /* Register 3 POWER_SAVE_REG */
    AMBIENT_LIGHT_DATA_POS = 0,     /* Register 4 AMBIENT_LIGHT_DATA_REG */
    WHITE_LIGHT_DATA_POS = 0,       /* Register 5 WHITE_LIGHT_DATA_REG */
    INTERRUPT_STATUS_POS = 14       /* Register 6 INTERRUPT_STATUS_REG */
};

// Table of lux conversion values depending on the integration time and gain.
// The arrays represent the all possible integration times and the index of the
// arrays represent the register's gain settings, which is directly analgous to
// their bit representations.
static const float eight_high_integration_time[] = {.0036, .0072, .0288, .0576};
static const float four_high_integration_time[] = {.0072, .0144, .0576, .1152};
static const float two_high_integration_time[] = {.0144, .0288, .1152, .2304};
static const float one_high_integration_time[] = {.0288, .0576, .2304, .4608};
static const float fifty_integration_time[] = {.0576, .1152, .4608, .9216};
static const float twenty_integration_time[] = {.1152, .2304, .9216, 1.8432};

class SparkFun_Ambient_Light {
  public:
    SparkFun_Ambient_Light(int address = 0x48); // I2C Constructor

    bool begin(void); // begin function

    // REG0x00, bits [12:11]
    // This function sets the gain for the Ambient Light Sensor. Possible values
    // are 1/8, 1/4, 1, and 2. The highest setting should only be used if the
    // sensors is behind dark glass, where as the lowest setting should be used in
    // dark rooms. The datasheet suggests always leaving it at around 1/4 or 1/8.
    void set_gain(float gain_val);

    // REG0x00, bits [12:11]
    // This function reads the gain for the Ambient Light Sensor. Possible values
    // are 1/8, 1/4, 1, and 2. The highest setting should only be used if the
    // sensors is behind dark glass, where as the lowest setting should be used in
    // dark rooms. The datasheet suggests always leaving it at around 1/4 or 1/8.
    float read_gain();

    // REG0x00, bits[9:6]
    // This function sets the integration time (the saturation time of light on the
    // sensor) of the ambient light sensor. Higher integration time leads to better
    // resolution but slower sensor refresh times.
    void set_integration_time(uint16_t time);

    // REG0x00, bits[9:6]
    // This function reads the integration time (the saturation time of light on the
    // sensor) of the ambient light sensor. Higher integration time leads to better
    // resolution but slower sensor refresh times.
    uint16_t read_integtration_time();

    // REG0x00, bits[5:4]
    // This function sets the persistence protect number i.e. the number of
    // values needing to crosh the interrupt thresholds.
    void set_protect(uint8_t prot_value);

    // REG0x00, bits[5:4]
    // This function reads the persistence protect number i.e. the number of
    // values needing to crosh the interrupt thresholds.
    uint8_t read_protect();

    // REG0x00, bit[1]
    // This function enables the Ambient Light Sensor's interrupt.
    void enable_interrupt();

    // REG0x00, bit[1]
    // This function disables the Ambient Light Sensor's interrupt.
    void disable_interrupt();

    // REG0x00, bit[1]
    // This function checks if the interrupt is enabled or disabled.
    uint8_t read_interrrupt_setting();

    // REG0x00, bit[0]
    // This function powers down the Ambient Light Sensor. The light sensor will
    // hold onto the last light reading which can be acessed while the sensor is
    // shut down. 0.5 micro Amps are consumed while shutdown.
    void shut_down();

    // REG0x00, bit[0]
    // This function powers up the Ambient Light Sensor. The last value that was
    // read during shut down will be overwritten on the sensor's subsequent read.
    // After power up, a small 4ms delay is applied to give time for the internal
    // osciallator and signal processor to power up.
    void power_on();

    // REG0x03, bit[0]
    // This function enables the current power save mode value and puts the Ambient
    // Light Sensor into power save mode.
    void enable_power_save();

    // REG0x03, bit[0]
    // This function disables the current power save mode value and pulls the Ambient
    // Light Sensor out of power save mode.
    void disable_power_save();

    // REG0x03, bit[0]
    // This function checks to see if power save mode is enabled or disabled.
    uint8_t read_power_save_enabled();

    // REG0x03, bit[2:1]
    // This function sets the power save mode value. It takes a value of 1-4. Each
    // incrementally higher value descreases the sampling rate of the sensor and so
    // increases power saving. The datasheet suggests enabling these modes when
    // continually sampling the sensor.
    void set_power_save_mode(uint16_t mode_value);

    // REG0x03, bit[2:1]
    // This function reads the power save mode value. The function above takes a value of 1-4. Each
    // incrementally higher value descreases the sampling rate of the sensor and so
    // increases power saving. The datasheet suggests enabling these modes when
    // continually sampling the sensor.
    uint8_t read_power_save_mode();

    // REG0x06, bits[15:14]
    // This function reads the interrupt register to see if an interrupt has been
    // triggered. There are two possible interrupts: a lower limit and upper limit
    // threshold, both set by the user.
    uint8_t read_interrupt();

    // REG0x02, bits[15:0]
    // This function sets the lower limit for the Ambient Light Sensor's interrupt.
    // It takes a lux value as its paramater.
    void set_interrupt_low_threshold(uint32_t lux_value);

    // REG0x02, bits[15:0]
    // This function reads the lower limit for the Ambient Light Sensor's interrupt.
    uint32_t read_low_threshold();

    // REG0x01, bits[15:0]
    // This function sets the upper limit for the Ambient Light Sensor's interrupt.
    // It takes a lux value as its paramater.
    void set_interrupt_high_threshold(uint32_t lux_value);

    // REG0x01, bits[15:0]
    // This function reads the upper limit for the Ambient Light Sensor's interrupt.
    uint32_t read_high_threshold();

    // REG[0x04], bits[15:0]
    // This function gets the sensor's ambient light's lux value. The lux value is
    // determined based on current gain and integration time settings. If the lux
    // value exceeds 1000 then a compensation formula is applied to it.
    uint32_t read_light();

    // REG[0x05], bits[15:0]
    // This function gets the sensor's ambient light's lux value. The lux value is
    // determined based on current gain and integration time settings. If the lux
    // value exceeds 1000 then a compensation formula is applied to it.
    uint32_t read_white_light();

  private:
    int fd_i2c_file;
    int slave_address;

    // This function compensates for lux values over 1000. From datasheet:
    // "Illumination values higher than 1000 lx show non-linearity. This
    // non-linearity is the same for all sensors, so a compensation forumla..."
    // etc. etc.
    uint32_t lux_compensation(uint32_t _lux_value);

    // The lux value of the Ambient Light sensor depends on both the gain and the
    // integration time settings. This function determines which conversion value
    // to use by using the bit representation of the gain as an index to look up
    // the conversion value in the correct integration time array. It then converts
    // the value and returns it.
    uint32_t calculate_lux(uint16_t _light_bits);

    // This function does the opposite calculation then the function above. The interrupt
    // threshold values given by the user are dependent on the gain and
    // intergration time settings. As a result the lux value needs to be
    // calculated with the current settings and this function accomplishes
    // that.
    uint16_t calculate_bits(uint32_t _lux_value);

    // This function reads a 16 bit register. It takes the register's address as its' parameter.
    uint16_t raw_read_register(VEML6030_16BIT_REGISTERS read_reg);

    // This function writes to a 16 bit register. Paramaters include the register's address,
    // the value to write, and the register value.
    void raw_write_register(VEML6030_16BIT_REGISTERS write_reg, uint16_t output_reg_value);

    // This function reads a 16 bit register, then shifts and masks the value before returning it.
    // A negative count is a left logical shift. A positive count is a right logical shift.
    uint16_t read_register(VEML6030_16BIT_REGISTERS read_reg, const int shift_count, uint16_t reg_mask);

    // This function writes to a 16 bit register. Paramaters include the register's address,
    // a mask for bits that are ignored, the bits to write, and the bits' starting position.
    // A negative count is a left logical shift. A positive count is a right logical shift.
    void write_register(VEML6030_16BIT_REGISTERS write_reg, uint16_t output_bits,
                        const int shift_count, const uint16_t output_mask);
};
#endif
