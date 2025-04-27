/** \file max30102_settings.h ******************************************************
 * 
 * Project: MAXREFDES117#
 * Filename: max30102_settings.h
 * Description: This module allow to easily set the MAX30102 settings.
 * All the settings are based on the MAX30102 datasheet that can be found
 * at:
 * https://www.analog.com/media/en/technical-documentation/data-sheets/max30102.pdf
 * 
 * Revision History:
 * 20-04-2025 Rev 01.00 GL Initial release.
 * 
 */

 #ifndef MAX30102_SETTINGS_H
 #define MAX30102_SETTINGS_H
 
 #include "Arduino.h"
 
 #pragma region "Control Enumerations"
 
 constexpr uint8_t bitToMask(uint8_t bitNumber, int value = 1) {
     return static_cast<uint8_t>(value << bitNumber);
 }
 
 enum SampleAveraging : uint8_t{
     NO_AVERAGING = bitToMask(7,0) | bitToMask(6,0) | bitToMask(5,0), // No averaging applied
     AVG_2 =  bitToMask(7,0) | bitToMask(6,0) | bitToMask(5,1), // 2 samples averaged
     AVG_4 =  bitToMask(7,0) | bitToMask(6,1) | bitToMask(5,0), // 4 samples averaged
     AVG_8 =  bitToMask(7,0) | bitToMask(6,1) | bitToMask(5,1), // 8 samples averaged
     AVG_16 = bitToMask(7,1) | bitToMask(6,0) | bitToMask(5,0), // 16 samples averaged
     AVG_32 = bitToMask(7,1) | bitToMask(6,0) | bitToMask(5,1)  // 32 samples averaged
 };
 
 enum ModeControl : uint8_t {
     HEART_RATE = bitToMask(2,0) | bitToMask(1,1) | bitToMask(0,0), // Heart Rate mode
     SPO2 = bitToMask(2,0) | bitToMask(1,1) | bitToMask(0,1), // SpO2 mode
     MULTI_LED = bitToMask(2,1) | bitToMask(1,1) | bitToMask(0,1), // Multi-LED mode
 };
 
 enum SPO2_ADC_Range: uint8_t{
     ADC_RANGE_2048 = bitToMask(6,0) | bitToMask(5,0), // 2048nA
     ADC_RANGE_4096 = bitToMask(6,0) | bitToMask(5,1), // 4096nA
     ADC_RANGE_8192 = bitToMask(6,1) | bitToMask(5,0), // 8192nA
     ADC_RANGE_16384 = bitToMask(6,1) | bitToMask(5,1) // 16384nA
 };
 
 enum SPO2_SampleRate: uint8_t{
     SPO2_RATE_50 = bitToMask(4,0) | bitToMask(3,0) | bitToMask(2,0), // 50Hz
     SPO2_RATE_100 = bitToMask(4,0) | bitToMask(3,0) | bitToMask(2,1), // 100Hz
     SPO2_RATE_200 = bitToMask(4,0) | bitToMask(3,1) | bitToMask(2,0), // 200Hz
     SPO2_RATE_400 = bitToMask(4,0) | bitToMask(3,1) | bitToMask(2,1), // 400Hz
     SPO2_RATE_800 = bitToMask(4,1) | bitToMask(3,0) | bitToMask(2,0), // 800Hz
     SPO2_RATE_1000 = bitToMask(4,1) | bitToMask(3,0) | bitToMask(2,1), // 1000Hz
     SPO2_RATE_1600 = bitToMask(4,1) | bitToMask(3,1) | bitToMask(2,0), // 1600Hz
     SPO2_RATE_3200 = bitToMask(4,1) | bitToMask(3,1) | bitToMask(2,1), // 3200Hz
 };
 
 enum SPO2_PulseWidth: uint8_t{
     PW_69 = bitToMask(1,0) | bitToMask(0,0), // 69us - 15bits adc resolution
     PW_118 = bitToMask(1,0) | bitToMask(0,1), // 118us - 16bits adc resolution
     PW_215 = bitToMask(1,1) | bitToMask(0,0), // 215us - 17bits adc resolution
     PW_411 = bitToMask(1,1) | bitToMask(0,1) // 411us - 18bits adc resolution
 };
 
 #pragma endregion
 
 
 #pragma region "Interrupts Status (0x00-0x01)"
 
 // Interrupts Status 1
 /**
  * \brief        Enable or disable the FIFO Almost Full interrupt
  * \param[in]    enable true to enable, false to disable
  * \retval       true on success, false on failure
  */
 bool interruptAFull(bool enable);
 /**
  * \brief        Enable or disable the new FIFO data ready interrupt
  * \param[in]    enable true to enable, false to disable
  * \retval       true on success, false on failure
  */
 bool interruptPPGReady(bool enable);
 /**
  * \brief        Enable or disable the Ambient Light Cancellation overflow interrupt
  * \param[in]    enable true to enable, false to disable
  * \retval       true on success, false on failure
  */
 bool interruptALCOverflow(bool enable);
 
 // Interrupts Status 2
 /**
  * \brief        Enable or disable the internal Temperature sensor ready interrupt
  * \param[in]    enable true to enable, false to disable
  * \retval       true on success, false on failure
  */
 bool interruptDIETempReady(bool enable);
 
 #pragma endregion
 
 #pragma region "FIFO (0x04-0x07)"
 /**
  * \brief        Set the FIFO write pointer
  * \param[in]    pointer FIFO write pointer value
  * \retval       true on success, false on failure
  */
 bool setFifoWritePointer(uint8_t pointer);
 /**
  * \brief        Set the FIFO overflow counter
  * \param[in]    counter FIFO overflow counter value
  * \retval       true on success, false on failure
  */
 bool setFifoOverflowCounter(uint8_t counter);
 /**
  * \brief        Set the FIFO read pointer
  * \param[in]    pointer FIFO read pointer value
  * \retval       true on success, false on failure
  */
 bool setFifoReadPointer(uint8_t pointer);
 /**
  * \brief        Set the FIFO data register
  * \param[in]    data FIFO data register value
  * \retval       true on success, false on failure
  */
 bool setFifoDataRegister(uint8_t data);
 
 #pragma endregion
 
 #pragma region "FIFO Configuration (0x08)"
 /**
  * \brief        Set the sample averaging mode
  * \param[in]    sampleAveraging Sample averaging mode
  * \retval       true on success, false on failure
  */
 bool setSampleAveraging(SampleAveraging sampleAveraging);
 /**
  * \brief        Set the FIFO roll over on full
  * \param[in]    enable true to enable, false to disable
  * \retval       true on success, false on failure
  */
 bool setFifoRollOverOnFull(bool enable);
 /**
  * \brief        Set the FIFO almost full threshold
  * \param[in]    threshold FIFO almost full threshold value
  * \retval       true on success, false on failure
  */
 bool setFifoAlmostFullThreshold(uint8_t threshold);
 
 #pragma endregion
 
 #pragma region "Mode Configuration (0x09)"
 /**
  * \brief        Set the shutdown control
  * \param[in]    enable true to enable, false to disable
  * \retval       true on success, false on failure
  */
 bool setShutdownCtrl(bool enable);
 /**
  * \brief        Set the reset control
  * \param[in]    enable true to enable, false to disable
  * \retval       true on success, false on failure
  */
 bool setResetCtrl(bool enable);
 /**
  * \brief        Set the mode control
  * \param[in]    mode Mode control value
  * \retval       true on success, false on failure
  */
 bool setModeControl(ModeControl modeControl);
 
 #pragma endregion
 
 #pragma region "SPO2 Configuration (0x0A)"
 /**
  * \brief        Set the ADC range
  * \param[in]    adcRange ADC range value
  * \retval       true on success, false on failure
  */
 bool setSPO2ADCRange(SPO2_ADC_Range adcRange);
 /**
  * \brief        Set the sample rate
  * \param[in]    sampleRate Sample rate value
  * \retval       true on success, false on failure
  */
 bool setSPO2SampleRate(SPO2_SampleRate sampleRate);
 /**
  * \brief        Set the pulse width
  * \param[in]    pulseWidth Pulse width value
  * \retval       true on success, false on failure
  */
 bool setSPO2PulseWidth(SPO2_PulseWidth pulseWidth);
 
 #pragma endregion
 
 #pragma region "LED Pulse Amplitude (0x0C-0x0D)"
 /**
  * \brief        Set the LED1 pulse amplitude
  * \param[in]    amplitude LED1 pulse amplitude value
  * \retval       true on success, false on failure
  */
 bool setLED1PulseAmplitude(uint8_t amplitude);
 /**
  * \brief        Set the LED2 pulse amplitude
  * \param[in]    amplitude LED2 pulse amplitude value
  * \retval       true on success, false on failure
  */
 bool setLED2PulseAmplitude(uint8_t amplitude);
 
 #pragma endregion
 
 #pragma region "Temperature Data (0x1F-0x21)"
 /**
  * \brief        Set the temperature enabled
  * \param[in]    enable true to enable, false to disable
  * \retval       true on success, false on failure
  */
 bool setTemperatureEnabled(bool enable);
 
 #pragma endregion
 
 #endif