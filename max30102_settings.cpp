
#include "max30102_settings.h"
#include "max30102.h"

namespace{
    /**
     * \brief        Alter a specific bit in a register value
     * \param[in]    reg     - register value
     * \param[in]    bit     - bit position to alter
     * \param[in]    value   - new value for the bit (true for 1, false for 0)
     * \retval       new register value with the altered bit
     */
    uint8_t alterBitValue(uint8_t reg, uint8_t bit, bool value) {
        if (value) 
            return reg | (1 << bit);
        else 
            return reg & ~(1 << bit);
    }

    /**
     * \brief        Set bits in a field of a register according to a mask
     * \param[in]    regValue   - current register value
     * \param[in]    value      - new value to set in the field
     * \param[in]    mask       - mask to specify which bits to set
     * \retval       new register value with the bits setted
     */
    uint8_t setBitsInField(uint8_t regValue, uint8_t value, uint8_t mask) {
        return (regValue & ~mask) | (value & mask);
    }

    /**
     * \brief        Check if a value fits in a mask
     * \param[in]    value   - value to check
     * \param[in]    mask    - mask to check against
     * \retval       true if the value fits in the mask, false otherwise
     */
    bool checkValueInMask(uint8_t value, uint8_t mask) {
        return (value & ~mask) == 0;
    }

    /**
     * \brief        Change a specific bit in a register value
     * \param[in]    regAddr   - register address
     * \param[in]    bit       - bit position to change
     * \param[in]    value     - new value for the bit (true for 1, false for 0)
     * \retval       true on success, false on failure
     */
    bool changeRegBitValue(uint8_t regAddr, uint8_t bit, bool value) {
        uint8_t actualRegValue;
        maxim_max30102_read_reg(regAddr, &actualRegValue);
        return maxim_max30102_write_reg(regAddr, alterBitValue(actualRegValue, bit, value));
    }

    /**
     * \brief        Change a specific mask in a register value
     * \param[in]    regAddr   - register address
     * \param[in]    mask      - mask to change
     * \param[in]    value     - new value for the mask
     * \param[in]    doCheckValueInMask - whether to check if the value fits in the mask
     * \retval       true on success, false on failure
     */
    bool changeRegMaskValue(uint8_t regAddr, uint8_t mask, uint8_t value, bool doCheckValueInMask = true) {
        if (doCheckValueInMask && !checkValueInMask(value, mask)) 
            return false;
        

        uint8_t actualRegValue;
        maxim_max30102_read_reg(regAddr, &actualRegValue);
        actualRegValue = setBitsInField(actualRegValue, value, mask);
        return maxim_max30102_write_reg(regAddr, actualRegValue);
    }
}


// Interrupts Enable 1
// Reg: REG_INTR_ENABLE_1
const uint8_t INTR_A_FULL_BIT = 7;
const uint8_t INTR_PPG_RDY_BIT = 6;
const uint8_t INTR_ALC_OVF_BIT = 5;

bool interruptAFull(bool enable){
    return changeRegBitValue(REG_INTR_ENABLE_1, INTR_A_FULL_BIT, enable);
}
bool interruptPPGReady(bool enable){
    return changeRegBitValue(REG_INTR_ENABLE_1, INTR_PPG_RDY_BIT, enable);
}
bool interruptALCOverflow(bool enable){
    return changeRegBitValue(REG_INTR_ENABLE_1, INTR_ALC_OVF_BIT, enable);
}


// Interrupts Enable 2
// Reg: REG_INTR_ENABLE_2
const uint8_t INTR_DIE_TEMP_RDY_BIT = 1;

bool interruptDIETempReady(bool enable){
    return changeRegBitValue(REG_INTR_ENABLE_2, INTR_DIE_TEMP_RDY_BIT, enable);
}


//FIFO
// Reg: REG_FIFO_CONFIG
const uint8_t FIFO_WR_PTR_MASK = 0x1F;
const uint8_t FIFO_OVF_COUNTER_MASK = 0x1F;
const uint8_t FIFO_RD_PTR_MASK = 0x1F;
const uint8_t FIFO_DATA_MASK = 0xFF;

bool setFifoWritePointer(uint8_t value) {
    return changeRegMaskValue(REG_FIFO_WR_PTR, FIFO_WR_PTR_MASK, value);
}
bool setFifoOverflowCounter(uint8_t value) {
    return changeRegMaskValue(REG_OVF_COUNTER, FIFO_OVF_COUNTER_MASK, value);
}
bool setFifoReadPointer(uint8_t value) {
    return changeRegMaskValue(REG_FIFO_RD_PTR, FIFO_RD_PTR_MASK, value);
}
bool setFifoDataRegister(uint8_t value) {
    return changeRegMaskValue(REG_FIFO_DATA, FIFO_DATA_MASK, value);
}


// FIFO Configuration
// Reg: REG_FIFO_CONFIG
const uint8_t FIFO_SAMPLE_AVERAGE_MASK = 0xE0;
const uint8_t FIFO_ROLLOVER_EN_BIT = 4;
const uint8_t FIFO_A_FULL_MASK = 0x0F;

bool setSampleAveraging(SampleAveraging sampleAveraging){
    return changeRegMaskValue(REG_FIFO_CONFIG, FIFO_SAMPLE_AVERAGE_MASK, sampleAveraging);
}
bool setFifoRollOverOnFull(bool enable){
    return changeRegBitValue(REG_FIFO_CONFIG, FIFO_ROLLOVER_EN_BIT, enable);
}
bool setFifoAlmostFullThreshold(uint8_t threshold){
    return changeRegMaskValue(REG_FIFO_CONFIG, FIFO_A_FULL_MASK, threshold);
}

// Mode Configuration
// Reg: REG_MODE_CONFIG
const uint8_t MODE_SHUTDOWN_BIT = 7;
const uint8_t MODE_RESET_BIT = 6;
const uint8_t MODE_MASK = 0x07; 

bool setShutdownCtrl(bool enable){
    return changeRegBitValue(REG_MODE_CONFIG, MODE_SHUTDOWN_BIT, enable);
}
bool setResetCtrl(bool enable){
    return changeRegBitValue(REG_MODE_CONFIG, MODE_RESET_BIT, enable);
}
bool setModeControl(ModeControl mode){
    return changeRegMaskValue(REG_MODE_CONFIG, MODE_MASK, mode);
}

// SPO2 Configuration
// Reg: REG_SPO2_CONFIG
const uint8_t SPO2_ADC_RANGE_MASK = 0x60;
const uint8_t SPO2_SAMPLE_RATE_MASK = 0x1C;
const uint8_t SPO2_PULSE_WIDTH_MASK = 0x03;

bool setSPO2ADCRange(SPO2_ADC_Range range){
    return changeRegMaskValue(REG_SPO2_CONFIG, SPO2_ADC_RANGE_MASK, range);
}
bool setSPO2SampleRate(SPO2_SampleRate rate){
    return changeRegMaskValue(REG_SPO2_CONFIG, SPO2_SAMPLE_RATE_MASK, rate);
}
bool setSPO2PulseWidth(SPO2_PulseWidth width){
    return changeRegMaskValue(REG_SPO2_CONFIG, SPO2_PULSE_WIDTH_MASK, width);
}

// LED Configuration
// Reg: REG_LED1_PA, REG_LED2_PA
const uint8_t LED_PA_MASK = 0xFF;
bool setLED1PulseAmplitude(uint8_t amplitude){
    return changeRegMaskValue(REG_LED1_PA, LED_PA_MASK, amplitude);
}
bool setLED2PulseAmplitude(uint8_t amplitude){
    return changeRegMaskValue(REG_LED2_PA, LED_PA_MASK, amplitude);
}

// Temperature Configuration
// Reg: REG_TEMP_CONFIG
const uint8_t TEMP_EN_BIT = 0;
bool setTemperatureEnabled(bool enable){
    return changeRegBitValue(REG_TEMP_CONFIG, TEMP_EN_BIT, enable);
}
