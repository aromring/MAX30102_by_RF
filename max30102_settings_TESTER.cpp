#include "max30102_settings.h"
#include "max30102.h"

namespace
{
    template<typename Func, typename ParamType>
    
    /**
     * \brief        Run a test on a function and update the test results
     * \param[in]    testName          - name of the test
     * \param[in]    functionToCall    - function to call
     * \param[in]    functionParamValue - parameter value for the function
     * \param[in]    regToRead         - register to read the result from
     * \param[in]    expectedResult     - expected result from the register
     * \param[in]    expectedSuccess    - expected success of the function call
     * 
     * \retval       true if the test passed, false otherwise
     */
    bool RUN_TEST(const String testName, Func functionToCall, ParamType functionParamValue,
        uint8_t regToRead, uint8_t expectedResult, bool expectedSuccess = true){
        uint8_t testingReg;
        bool functionRes = functionToCall(functionParamValue);
        if (!functionRes && expectedSuccess) {
            Serial.println("Setting " + testName + " failed. Function call failed.");
            return false;
        }
        maxim_max30102_read_reg(regToRead, &testingReg);
        if (testingReg != expectedResult) {
            Serial.println("Setting " + testName + " failed. Expected: " + String(expectedResult, HEX) + ", got: " + String(testingReg, HEX));
            return false;
        }
        return true;
    }

    /**
     * \brief        Run a function (test) with parameters and update the test results
     */
    #define RUN_TEST(...) (RUN_TEST(__VA_ARGS__) ? passedTests++ : failedTests++)
} 

bool testerSetter(){
    //uint8_t testingReg;
    int failedTests = 0;
    int passedTests = 0;

    // Interrupt Enable 1
    maxim_max30102_write_reg(REG_INTR_ENABLE_1, 0x00); // FIFO_CONFIG[7:0]
    RUN_TEST("interruptAFull True", interruptAFull, true, REG_INTR_ENABLE_1, 0x80);
    RUN_TEST("interruptAFull True", interruptAFull, true, REG_INTR_ENABLE_1, 0x80);
    RUN_TEST("interruptAFull False", interruptAFull, false, REG_INTR_ENABLE_1, 0x00);
    RUN_TEST("interruptPPGReady True", interruptPPGReady, true, REG_INTR_ENABLE_1, 0x40);
    RUN_TEST("interruptPPGReady False", interruptPPGReady, false, REG_INTR_ENABLE_1, 0x00);
    RUN_TEST("interruptALCOverflow True", interruptALCOverflow, true, REG_INTR_ENABLE_1, 0x20);
    RUN_TEST("interruptALCOverflow False", interruptALCOverflow, false, REG_INTR_ENABLE_1, 0x00);

    interruptAFull(true);
    interruptPPGReady(true);
    RUN_TEST("multipleInter True", interruptALCOverflow, true, REG_INTR_ENABLE_1, 0xE0);
    RUN_TEST("removingInterruptAFull", interruptAFull, false, REG_INTR_ENABLE_1, 0x60);

    // Interrupt Enable 2
    maxim_max30102_write_reg(REG_INTR_ENABLE_2, 0x00);
    RUN_TEST("interruptDIETempReady True", interruptDIETempReady, true, REG_INTR_ENABLE_2, 0x02);
    RUN_TEST("interruptDIETempReady False", interruptDIETempReady, false, REG_INTR_ENABLE_2, 0x00);

    // Mode Configuration
    RUN_TEST("setModeControl MULTI_LED", setModeControl, ModeControl::MULTI_LED, REG_MODE_CONFIG, 0x07);
    RUN_TEST("setModeControl SPO2", setModeControl, ModeControl::SPO2, REG_MODE_CONFIG, 0x03);
    RUN_TEST("setModeControl HEART_RATE", setModeControl, ModeControl::HEART_RATE, REG_MODE_CONFIG, 0x02);

    // FIFO Write Pointer
    maxim_max30102_write_reg(REG_FIFO_WR_PTR, 0x00);
    RUN_TEST("setFifoWritePointer 0x00", setFifoWritePointer, 0x00, REG_FIFO_WR_PTR, 0x00);
    RUN_TEST("setFifoWritePointer 0x1F", setFifoWritePointer, 0x1F, REG_FIFO_WR_PTR, 0x1F);
    RUN_TEST("setFifoWritePointer 0x20 (saturates to 0x1F. Function call should fail!)", setFifoWritePointer, 0x20, REG_FIFO_WR_PTR, 0x1F, false);
    RUN_TEST("setFifoWritePointer 0x00 again", setFifoWritePointer, 0x00, REG_FIFO_WR_PTR, 0x00);

    // FIFO Overflow Counter
    maxim_max30102_write_reg(REG_OVF_COUNTER, 0x00);
    RUN_TEST("setFifoOverflowCounter 0x00", setFifoOverflowCounter, 0x00, REG_OVF_COUNTER, 0x00);

    // FIFO Read Pointer
    maxim_max30102_write_reg(REG_FIFO_RD_PTR, 0x00);
    RUN_TEST("setFifoReadPointer 0x00", setFifoReadPointer, 0x00, REG_FIFO_RD_PTR, 0x00);
    RUN_TEST("setFifoReadPointer 0x1F", setFifoReadPointer, 0x1F, REG_FIFO_RD_PTR, 0x1F);
    RUN_TEST("setFifoReadPointer 0x20 (saturates to 0x1F. Function call should fail!)", setFifoReadPointer, 0x20, REG_FIFO_RD_PTR, 0x1F, false);

    // FIFO Configuration
    maxim_max30102_write_reg(REG_FIFO_CONFIG, 0x00);
    RUN_TEST("SampleAveraging NO_AVERAGING", setSampleAveraging, SampleAveraging::NO_AVERAGING, REG_FIFO_CONFIG, 0x00);
    RUN_TEST("SampleAveraging AVG_2", setSampleAveraging, SampleAveraging::AVG_2, REG_FIFO_CONFIG, 0x20);
    RUN_TEST("SampleAveraging AVG_4", setSampleAveraging, SampleAveraging::AVG_4, REG_FIFO_CONFIG, 0x40);
    RUN_TEST("FIFO RollOver Enabled", setFifoRollOverOnFull, true, REG_FIFO_CONFIG, 0x50);
    RUN_TEST("FIFO RollOver Disabled", setFifoRollOverOnFull, false, REG_FIFO_CONFIG, 0x40);
    RUN_TEST("FIFO Almost Full Threshold 0x00", setFifoAlmostFullThreshold, 0x00, REG_FIFO_CONFIG, 0x40);
    RUN_TEST("FIFO Almost Full Threshold 0x0F", setFifoAlmostFullThreshold, 0x0F, REG_FIFO_CONFIG, 0x4F);

    // SpO2 Configuration
    maxim_max30102_write_reg(REG_SPO2_CONFIG, 0x00);
    RUN_TEST("SPO2 ADC_RANGE_2048", setSPO2ADCRange, SPO2_ADC_Range::ADC_RANGE_2048, REG_SPO2_CONFIG, 0x00);
    RUN_TEST("SPO2 ADC_RANGE_4096", setSPO2ADCRange, SPO2_ADC_Range::ADC_RANGE_4096, REG_SPO2_CONFIG, 0x20);
    RUN_TEST("SPO2 ADC_RANGE_8192", setSPO2ADCRange, SPO2_ADC_Range::ADC_RANGE_8192, REG_SPO2_CONFIG, 0x40);
    RUN_TEST("SPO2 ADC_RANGE_16384", setSPO2ADCRange, SPO2_ADC_Range::ADC_RANGE_16384, REG_SPO2_CONFIG, 0x60);

    maxim_max30102_write_reg(REG_SPO2_CONFIG, 0x00);
    RUN_TEST("SPO2 SampleRate 50Hz", setSPO2SampleRate, SPO2_SampleRate::SPO2_RATE_50, REG_SPO2_CONFIG, 0x00);
    RUN_TEST("SPO2 SampleRate 100Hz", setSPO2SampleRate, SPO2_SampleRate::SPO2_RATE_100, REG_SPO2_CONFIG, 0x04);
    RUN_TEST("SPO2 SampleRate 200Hz", setSPO2SampleRate, SPO2_SampleRate::SPO2_RATE_200, REG_SPO2_CONFIG, 0x08);
    RUN_TEST("SPO2 SampleRate 400Hz", setSPO2SampleRate, SPO2_SampleRate::SPO2_RATE_400, REG_SPO2_CONFIG, 0x0C);
    RUN_TEST("SPO2 SampleRate 800Hz", setSPO2SampleRate, SPO2_SampleRate::SPO2_RATE_800, REG_SPO2_CONFIG, 0x10);
    RUN_TEST("SPO2 SampleRate 1000Hz", setSPO2SampleRate, SPO2_SampleRate::SPO2_RATE_1000, REG_SPO2_CONFIG, 0x14);
    RUN_TEST("SPO2 SampleRate 1600Hz", setSPO2SampleRate, SPO2_SampleRate::SPO2_RATE_1600, REG_SPO2_CONFIG, 0x18);
    RUN_TEST("SPO2 SampleRate 3200Hz", setSPO2SampleRate, SPO2_SampleRate::SPO2_RATE_3200, REG_SPO2_CONFIG, 0x1C);

    maxim_max30102_write_reg(REG_SPO2_CONFIG, 0x00);
    RUN_TEST("SPO2 PulseWidth 69us", setSPO2PulseWidth, SPO2_PulseWidth::PW_69, REG_SPO2_CONFIG, 0x00);
    RUN_TEST("SPO2 PulseWidth 118us", setSPO2PulseWidth, SPO2_PulseWidth::PW_118, REG_SPO2_CONFIG, 0x01);
    RUN_TEST("SPO2 PulseWidth 215us", setSPO2PulseWidth, SPO2_PulseWidth::PW_215, REG_SPO2_CONFIG, 0x02);
    RUN_TEST("SPO2 PulseWidth 411us", setSPO2PulseWidth, SPO2_PulseWidth::PW_411, REG_SPO2_CONFIG, 0x03);

    // LED Configuration
    RUN_TEST("LED1 Pulse Amplitude 0x00", setLED1PulseAmplitude, 0x00, REG_LED1_PA, 0x00);
    RUN_TEST("LED1 Pulse Amplitude 0xFF", setLED1PulseAmplitude, 0xFF, REG_LED1_PA, 0xFF);
    RUN_TEST("LED2 Pulse Amplitude 0x00", setLED2PulseAmplitude, 0x00, REG_LED2_PA, 0x00);
    RUN_TEST("LED2 Pulse Amplitude 0xFF", setLED2PulseAmplitude, 0xFF, REG_LED2_PA, 0xFF);


    Serial.println("Total tests: " + String(passedTests + failedTests) + "\nPassed: " + String(passedTests) + "\nFailed: " + String(failedTests));
    return true;
}   