# MAX30102_by_RF
Arduino C code for MAX30102 pulse oximetry sensor (MAXIM Integrated, Inc.)

This project originated from a public domain reference design MAXREFDES117# published by Maxim Integrated, Inc., at
https://www.maximintegrated.com/en/design/reference-design-center/system-board/6300.html/tb_tab0
for the System Board 6300 containing MAX30102 sensor for pulse oximetry. I adopted it to work with Adafruit Feather M0 Adalogger (https://www.adafruit.com/product/2796).
First, I copied the firmware code for Arduino platform from that page, then I heavily modified it making the following changes:

1. All the original I2C calls in max30102.cpp that relied on platform-dependent SoftI2C library have been replaced by their equivalents from the generic Wire library. Consequently, SoftI2CMaster.h header file was no longer needed and has been removed.
2. Unnecessary Adafruit_NeoPixel.h and Adafruit_NeoPixel.cpp files have been removed.
3. A software bug in maxim_heart_rate_and_oxygen_saturation() function in algorithm.cpp has been fixed: after applying 4-point moving average, the an_x[] still contained original last three points (an_x[97], an_x[98], an_x[99]). Subsequent code still referred to the full an_x[] array (0-99) instead of its averaged portion (0-96).
4. In algorithm.h the "uint8_t uch_spo2_table[184]" has been replaced by "float uch_spo2_table[184]", where the original integers have been recalculated into floating point numbers with 6 decimal digits precision.
5. A completely new algorithm of signal processing and resulting heart rate and SpO2 calculation has been designed and placed in new files algorithm_by_RF.h and algorithm_by_RF.cpp. The algorithm has been described and published in the following Instructable: https://www.instructables.com/id/Pulse-Oximeter-With-Much-Improved-Precision/.
6. The RD117_ARDUINO.ino file has been rewritten from the ground up. Code for saving oximetry data to an SD card has been added, algorithm_by_RF has been made default, while Maxim's algorithm has been made optional. Integer variables were replaced by floating point variables, where applicable.

The following two crucial constants are defined in algorithm_by_RF.h:

const float min_autocorrelation_ratio;
const float min_pearson_correlation;

Values of these constants determine the severity of quality filter applied to raw signals emitted by MAX30102. Generally, the higher their values, the more strict the filter will be. Details can be found in this project's Instructable.

The RD117_ARDUINO.ino contains several defines that will enable/disable debug printing, testing of the original MAXIM algorithm, saving raw data, and most importantly whether or not you use Adafruit Feather M0 Adalogger as your MCU. Disable the latter option if you want to use an alternative microcontroller. But I have to give you a fair warning: Feather M0's features an ATSAMD21G18 ARM Cortex M0 processor, clocked at 48 MHz and with a whopping 256K of FLASH (8x more than the Atmega328 or 32u4) and 32K of RAM (16x as much). As such, it can handle this code without a drop of sweat. Lesser MCUs may have serious problems with it, especially in terms of sufficient memory.

HOW TO REPORT BUGS

Since I am not a psychic, all inquiries containing some form of vague "your code does not work" and no useful information at all will invariably be referred to this section of the README file. I am sorry, but I have honestly tried being helpful to quite a number of people contacting me either through GitHub or Instructables mail - and in each case I had to waste entire days of e-mail exchanges until I had at least a minimum of useful information and data. Hence, I will welcome a software bug report, but I will not be able to help you with the following issues:

1) Underpowered MCU (see previous paragraphs).
2) Misunderstanding of oximetry, especially motion artifacts. Don't expect a useful signal when the sensor is moving. Don't expect a useful signal from the very first batch, it may take some time - say, a minute - for the whole system to get going.
3) Faulty hardware (see https://reedpaper.wordpress.com/2018/08/22/pulse-oximeter-max30100-max30102-how-to-fix-wrong-board/, for example). Some cloned sensors may have the RED and IR channels swapped; see Rob Cazzaro's post: https://github.com/aromring/MAX30102_by_RF/issues/13#issue-601473302 (his is a prime example of an excellent bug report, by the way).
4) Incorrect connections.

At minimum, in order to accept a bug report I need you to:

A. Obtain at least one batch of the _RAW_ _DATA_ coming from the MAX30102 sensor, both red and IR channels, preferably as two columns of 100 numbers. Turn #DEBUG or #SAVE_RAW_DATA directive on in RD117_ARDUINO.ino to get raw data in the Serial Monitor.

B. _Plot_ the data on your end making sure that the resulting graph matches examples of _good_ signals in this project's Instructable or the ExpectedGoodQualitySignals.png image above. If not, then don't expect a bad signal to produce correct results! 

C. If the signal is of good quality, but the code still refuses to cooperate, then send the data to me, but please don't use Instructables mail for this purpose (data always comes truncated)! Preferably, open a new issue in this GitHub repository and paste the data there. 

Thank you.
