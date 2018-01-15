/** \file algorithm.h ******************************************************
*
* Project: MAXREFDES117#
* Filename: algorithm.h
* Description: This module is the heart rate/SpO2 calculation algorithm header file
*
* Revision History:
*\n 1-18-2016 Rev 01.00 SK Initial release.
*\n
*
* --------------------------------------------------------------------
*
* This code follows the following naming conventions:
*
*\n char              ch_pmod_value
*\n char (array)      s_pmod_s_string[16]
*\n float             f_pmod_value
*\n int32_t           n_pmod_value
*\n int32_t (array)   an_pmod_value[16]
*\n int16_t           w_pmod_value
*\n int16_t (array)   aw_pmod_value[16]
*\n uint16_t          uw_pmod_value
*\n uint16_t (array)  auw_pmod_value[16]
*\n uint8_t           uch_pmod_value
*\n uint8_t (array)   auch_pmod_buffer[16]
*\n uint32_t          un_pmod_value
*\n int32_t *         pn_pmod_value
*
* ------------------------------------------------------------------------- */
/*******************************************************************************
* Copyright (C) 2015 Maxim Integrated Products, Inc., All Rights Reserved.
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
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************
*/
#ifndef ALGORITHM_H_
#define ALGORITHM_H_
#include <arduino.h>

#define true 1
#define false 0
#define FS 25    //sampling frequency
#define BUFFER_SIZE  (FS* 4) 
#define MA4_SIZE  4 // DONOT CHANGE
#define BUFFER_SIZE_MA4 BUFFER_SIZE-MA4_SIZE
#define min(x,y) ((x) < (y) ? (x) : (y))

//uch_spo2_table is approximated as  -45.060*ratioAverage* ratioAverage + 30.354 *ratioAverage + 94.845 ;
//const uint8_t uch_spo2_table[184]={ 95, 95, 95, 96, 96, 96, 97, 97, 97, 97, 97, 98, 98, 98, 98, 98, 99, 99, 99, 99, 
//              99, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 
//              100, 100, 100, 100, 99, 99, 99, 99, 99, 99, 99, 99, 98, 98, 98, 98, 98, 98, 97, 97, 
//              97, 97, 96, 96, 96, 96, 95, 95, 95, 94, 94, 94, 93, 93, 93, 92, 92, 92, 91, 91, 
//              90, 90, 89, 89, 89, 88, 88, 87, 87, 86, 86, 85, 85, 84, 84, 83, 82, 82, 81, 81, 
//              80, 80, 79, 78, 78, 77, 76, 76, 75, 74, 74, 73, 72, 72, 71, 70, 69, 69, 68, 67, 
//              66, 66, 65, 64, 63, 62, 62, 61, 60, 59, 58, 57, 56, 56, 55, 54, 53, 52, 51, 50, 
//              49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 31, 30, 29, 
//              28, 27, 26, 25, 23, 22, 21, 20, 19, 17, 16, 15, 14, 12, 11, 10, 9, 7, 6, 5, 
//              3, 2, 1 } ;
//
//uch_spo2_table is approximated as  -45.060*ratioAverage* ratioAverage + 30.354 *ratioAverage + 94.845 ;
const float uch_spo2_table[184]=
              {94.845,95.144034,95.434056,95.715066,95.987064,96.25005,96.504024,96.748986,96.984936,97.211874,97.4298,97.638714,97.838616,98.029506,
              98.211384,98.38425,98.548104,98.702946,98.848776,98.985594,99.1134,99.232194,99.341976,99.442746,99.534504,99.61725,99.690984,99.755706,
              99.811416,99.858114,99.8958,99.924474,99.944136,99.954786,99.956424,99.94905,99.932664,99.907266,99.872856,99.829434,99.777,99.715554,
              99.645096,99.565626,99.477144,99.37965,99.273144,99.157626,99.033096,98.899554,98.757,98.605434,98.444856,98.275266,98.096664,97.90905,
              97.712424,97.506786,97.292136,97.068474,96.8358,96.594114,96.343416,96.083706,95.814984,95.53725,95.250504,94.954746,94.649976,94.336194,
              94.0134,93.681594,93.340776,92.990946,92.632104,92.26425,91.887384,91.501506,91.106616,90.702714,90.2898,89.867874,89.436936,88.996986,
              88.548024,88.09005,87.623064,87.147066,86.662056,86.168034,85.665,85.152954,84.631896,84.101826,83.562744,83.01465,82.457544,81.891426,
              81.316296,80.732154,80.139,79.536834,78.925656,78.305466,77.676264,77.03805,76.390824,75.734586,75.069336,74.395074,73.7118,73.019514,
              72.318216,71.607906,70.888584,70.16025,69.422904,68.676546,67.921176,67.156794,66.3834,65.600994,64.809576,64.009146,63.199704,62.38125,
              61.553784,60.717306,59.871816,59.017314,58.1538,57.281274,56.399736,55.509186,54.609624,53.70105,52.783464,51.856866,50.921256,49.976634,
              49.023,48.060354,47.088696,46.108026,45.118344,44.11965,43.111944,42.095226,41.069496,40.034754,38.991,37.938234,36.876456,35.805666,
              34.725864,33.63705,32.539224,31.432386,30.316536,29.191674,28.0578,26.914914,25.763016,24.602106,23.432184,22.25325,21.065304,19.868346,
              18.662376,17.447394,16.2234,14.990394,13.748376,12.497346,11.237304,9.96825,8.690184,7.403106,6.107016,4.801914,3.4878,2.164674,0.832536,
              0.0};

//#if defined(ARDUINO_AVR_UNO)
//Arduino Uno doesn't have enough SRAM to store 100 samples of IR led data and red led data in 32-bit format
//To solve this problem, 16-bit MSB of the sampled data will be truncated.  Samples become 16-bit data.
//void maxim_heart_rate_and_oxygen_saturation(uint16_t *pun_ir_buffer, int32_t n_ir_buffer_length, uint16_t *pun_red_buffer, int32_t *pn_spo2, int8_t *pch_spo2_valid, int32_t *pn_heart_rate, int8_t *pch_hr_valid);
//#else
void maxim_heart_rate_and_oxygen_saturation(uint32_t *pun_ir_buffer, int32_t n_ir_buffer_length, uint32_t *pun_red_buffer, float *pn_spo2, int8_t *pch_spo2_valid, int32_t *pn_heart_rate, int8_t *pch_hr_valid);
//#endif
void maxim_find_peaks(int32_t *pn_locs, int32_t *n_npks,  int32_t  *pn_x, int32_t n_size, int32_t n_min_height, int32_t n_min_distance, int32_t n_max_num);
void maxim_peaks_above_min_height(int32_t *pn_locs, int32_t *n_npks,  int32_t  *pn_x, int32_t n_size, int32_t n_min_height);
void maxim_remove_close_peaks(int32_t *pn_locs, int32_t *pn_npks, int32_t *pn_x, int32_t n_min_distance);
void maxim_sort_ascend(int32_t  *pn_x, int32_t n_size);
void maxim_sort_indices_descend(int32_t  *pn_x, int32_t *pn_indx, int32_t n_size);

#endif /* ALGORITHM_H_ */

