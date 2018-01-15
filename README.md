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
5. A completely new algorithm of signal processing and resulting heart rate and SpO2 calculation has been designed and placed in new files algorithm_by_RF.h and algorithm_by_RF.cpp. The algorithm has been described and published in the following Instructable: ().
6. The RD117_ARDUINO.ino file has been rewritten from the ground up. Code for saving oximetry data to an SD card has been added, algorithm_by_RF has been made default, while Maxim's algorithm has been made optional. Integer variables were replaced by floating point variables, where applicable.

The following two crucial constants are defined in algorithm_by_RF.h:

const float min_autocorrelation_ratio;
const float min_pearson_correlation;

Values of these constants determine the severity of quality filter applied to raw signals emitted by MAX30102. Generally, the higher their values, the more strict the filter will be. Details can be found in this project's Instructable.
