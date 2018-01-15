/*
 * Created by Robert Fraczkiewicz, 01/2018
 * New signal processing methodology for obtaining heart rate and SpO2 data 
 * from the MAX30102 sensor manufactured by MAXIM Integrated Products, Inc.
 */
/*******************************************************************************
* Copyright (C) 2017 Robert Fraczkiewicz, All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL ROBERT FRACZKIEWICZ BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Robert Fraczkiewicz retains all
* ownership rights.
*******************************************************************************
*/
#ifndef ALGORITHM_BY_RF_H_
#define ALGORITHM_BY_RF_H_
#include <arduino.h>

#define FS 25    //sampling frequency
#define BUFFER_SIZE  (FS*4)
#define FS60 (FS*60) // Conversion factor for heart rate
#define MAX_HR 125 // Maximal heart rate
#define MIN_HR 40 // Minimal heart rate

const int32_t LOWEST_PERIOD = FS60/MAX_HR; // Minimal distance between peaks
const int32_t HIGHEST_PERIOD = FS60/MIN_HR; // Maximal distance between peaks
// RF: Sum of squares of numbers from 0 to 99, inclusively, centered around the 49.5 mean
const float sum_X2 = 83325;
const float mean_X = (float)(BUFFER_SIZE-1)/2.0; // 49.5
// Minimal ratio of two autocorrelation sequence elements: one at a considered lag to the one at lag 0.
// Good quality signals must have such ratio greater than this minimum.
const float min_autocorrelation_ratio = 0.5;
// Pearson correlation between red and IR signals.
// Good quality signals must have their correlation coefficient greater than this minimum.
const float min_pearson_correlation = 0.8;

void rf_heart_rate_and_oxygen_saturation(uint32_t *pun_ir_buffer, int32_t n_ir_buffer_length, uint32_t *pun_red_buffer, float *pn_spo2, int8_t *pch_spo2_valid, int32_t *pn_heart_rate, 
                                        int8_t *pch_hr_valid, float *ratio, float *correl);
float rf_linear_regression_beta(float *pn_x, float xmean, float sum_x2);
float rf_autocorrelation(float *pn_x, int32_t n_size, int32_t n_lag);
float rf_rms(float *pn_x, int32_t n_size, float *sumsq);
float rf_Pcorrelation(float *pn_x, float *pn_y, int32_t n_size);
void rf_signal_periodicity(float *pn_x, int32_t n_size, int32_t *p_last_periodicity, int32_t n_min_distance, int32_t n_max_distance, float min_aut_ratio, float aut_lag0, float *ratio);

#endif /* ALGORITHM_BY_RF_H_ */

