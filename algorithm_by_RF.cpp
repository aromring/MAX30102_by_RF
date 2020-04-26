/*
 * Created by Robert Fraczkiewicz, 12/2017
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
#include "algorithm_by_RF.h"
#include <math.h>

void rf_heart_rate_and_oxygen_saturation(uint32_t *pun_ir_buffer, int32_t n_ir_buffer_length, uint32_t *pun_red_buffer, float *pn_spo2, int8_t *pch_spo2_valid, 
                int32_t *pn_heart_rate, int8_t *pch_hr_valid, float *ratio, float *correl)
/**
* \brief        Calculate the heart rate and SpO2 level, Robert Fraczkiewicz version
* \par          Details
*               By detecting  peaks of PPG cycle and corresponding AC/DC of red/infra-red signal, the xy_ratio for the SPO2 is computed.
*
* \param[in]    *pun_ir_buffer           - IR sensor data buffer
* \param[in]    n_ir_buffer_length      - IR sensor data buffer length
* \param[in]    *pun_red_buffer          - Red sensor data buffer
* \param[out]    *pn_spo2                - Calculated SpO2 value
* \param[out]    *pch_spo2_valid         - 1 if the calculated SpO2 value is valid
* \param[out]    *pn_heart_rate          - Calculated heart rate value
* \param[out]    *pch_hr_valid           - 1 if the calculated heart rate value is valid
*
* \retval       None
*/
{
  int32_t k;  
  static int32_t n_last_peak_interval=LOWEST_PERIOD;
  float f_ir_mean,f_red_mean,f_ir_sumsq,f_red_sumsq;
  float f_y_ac, f_x_ac, xy_ratio;
  float beta_ir, beta_red, x;
  float an_x[BUFFER_SIZE], *ptr_x; //ir
  float an_y[BUFFER_SIZE], *ptr_y; //red

  // calculates DC mean and subtracts DC from ir and red
  f_ir_mean=0.0; 
  f_red_mean=0.0;
  for (k=0; k<n_ir_buffer_length; ++k) {
    f_ir_mean += pun_ir_buffer[k];
    f_red_mean += pun_red_buffer[k];
  }
  f_ir_mean=f_ir_mean/n_ir_buffer_length ;
  f_red_mean=f_red_mean/n_ir_buffer_length ;
  
  // remove DC 
  for (k=0,ptr_x=an_x,ptr_y=an_y; k<n_ir_buffer_length; ++k,++ptr_x,++ptr_y) {
    *ptr_x = pun_ir_buffer[k] - f_ir_mean;
    *ptr_y = pun_red_buffer[k] - f_red_mean;
  }

  // RF, remove linear trend (baseline leveling)
  beta_ir = rf_linear_regression_beta(an_x, mean_X, sum_X2);
  beta_red = rf_linear_regression_beta(an_y, mean_X, sum_X2);
  for(k=0,x=-mean_X,ptr_x=an_x,ptr_y=an_y; k<n_ir_buffer_length; ++k,++x,++ptr_x,++ptr_y) {
    *ptr_x -= beta_ir*x;
    *ptr_y -= beta_red*x;
  }
  
    // For SpO2 calculate RMS of both AC signals. In addition, pulse detector needs raw sum of squares for IR
  f_y_ac=rf_rms(an_y,n_ir_buffer_length,&f_red_sumsq);
  f_x_ac=rf_rms(an_x,n_ir_buffer_length,&f_ir_sumsq);

  // Calculate Pearson correlation between red and IR
  *correl=rf_Pcorrelation(an_x, an_y, n_ir_buffer_length)/sqrt(f_red_sumsq*f_ir_sumsq);

  // Find signal periodicity
  if(*correl>=min_pearson_correlation) {
    // At the beginning of oximetry run the exact range of heart rate is unknown. This may lead to wrong rate if the next call does not find the _first_
    // peak of the autocorrelation function. E.g., second peak would yield only 50% of the true rate. 
    if(LOWEST_PERIOD==n_last_peak_interval) 
      rf_initialize_periodicity_search(an_x, BUFFER_SIZE, &n_last_peak_interval, HIGHEST_PERIOD, min_autocorrelation_ratio, f_ir_sumsq);
    // RF, If correlation os good, then find average periodicity of the IR signal. If aperiodic, return periodicity of 0
    if(n_last_peak_interval!=0)
      rf_signal_periodicity(an_x, BUFFER_SIZE, &n_last_peak_interval, LOWEST_PERIOD, HIGHEST_PERIOD, min_autocorrelation_ratio, f_ir_sumsq, ratio);
  } else n_last_peak_interval=0;

  // Calculate heart rate if periodicity detector was successful. Otherwise, reset peak interval to its initial value and report error.
  if(n_last_peak_interval!=0) {
    *pn_heart_rate = (int32_t)(FS60/n_last_peak_interval);
    *pch_hr_valid  = 1;
  } else {
    n_last_peak_interval=LOWEST_PERIOD;
    *pn_heart_rate = -999; // unable to calculate because signal looks aperiodic
    *pch_hr_valid  = 0;
    *pn_spo2 =  -999 ; // do not use SPO2 from this corrupt signal
    *pch_spo2_valid  = 0; 
    return;
  }

  // After trend removal, the mean represents DC level
  xy_ratio= (f_y_ac*f_ir_mean)/(f_x_ac*f_red_mean);  //formula is (f_y_ac*f_x_dc) / (f_x_ac*f_y_dc) ;
  if(xy_ratio>0.02 && xy_ratio<1.84) { // Check boundaries of applicability
    *pn_spo2 = (-45.060*xy_ratio + 30.354)*xy_ratio + 94.845;
    *pch_spo2_valid = 1;
  } else {
    *pn_spo2 =  -999 ; // do not use SPO2 since signal an_ratio is out of range
    *pch_spo2_valid  = 0; 
  }
}

float rf_linear_regression_beta(float *pn_x, float xmean, float sum_x2)
/**
* \brief        Coefficient beta of linear regression 
* \par          Details
*               Compute directional coefficient, beta, of a linear regression of pn_x against mean-centered
*               point index values (0 to BUFFER_SIZE-1). xmean must equal to (BUFFER_SIZE-1)/2! sum_x2 is 
*               the sum of squares of the mean-centered index values. 
*               Robert Fraczkiewicz, 12/22/2017
* \retval       Beta
*/
{
  float x,beta,*pn_ptr;
  beta=0.0;
  for(x=-xmean,pn_ptr=pn_x;x<=xmean;++x,++pn_ptr)
    beta+=x*(*pn_ptr);
  return beta/sum_x2;
}

float rf_autocorrelation(float *pn_x, int32_t n_size, int32_t n_lag) 
/**
* \brief        Autocorrelation function
* \par          Details
*               Compute autocorrelation sequence's n_lag's element for a given series pn_x
*               Robert Fraczkiewicz, 12/21/2017
* \retval       Autocorrelation sum
*/
{
  int16_t i, n_temp=n_size-n_lag;
  float sum=0.0,*pn_ptr;
  if(n_temp<=0) return sum;
  for (i=0,pn_ptr=pn_x; i<n_temp; ++i,++pn_ptr) {
    sum += (*pn_ptr)*(*(pn_ptr+n_lag));
  }
  return sum/n_temp;
}

void rf_initialize_periodicity_search(float *pn_x, int32_t n_size, int32_t *p_last_periodicity, int32_t n_max_distance, float min_aut_ratio, float aut_lag0)
/**
* \brief        Search the range of true signal periodicity
* \par          Details
*               Determine the range of current heart rate by locating neighborhood of 
*               the _first_ peak of the autocorrelation function. If at all lags until  
*               n_max_distance the autocorrelation is less than min_aut_ratio fraction 
*               of the autocorrelation at lag=0, then the input signal is insufficiently 
*               periodic and probably indicates motion artifacts.
*               Robert Fraczkiewicz, 04/25/2020
* \retval       Average distance between peaks
*/
{
  int32_t n_lag;
  float aut,aut_right;
  // At this point, *p_last_periodicity = LOWEST_PERIOD. Start walking to the right,
  // two steps at a time, until lag ratio fulfills quality criteria or HIGHEST_PERIOD
  // is reached.
  n_lag=*p_last_periodicity;
  aut_right=aut=rf_autocorrelation(pn_x, n_size, n_lag);
  // Check sanity
  if(aut/aut_lag0 >= min_aut_ratio) {
    // Either quality criterion, min_aut_ratio, is too low, or heart rate is too high.
    // Are we on autocorrelation's downward slope? If yes, continue to a local minimum.
    // If not, continue to the next block.
    do {
      aut=aut_right;
      n_lag+=2;
      aut_right=rf_autocorrelation(pn_x, n_size, n_lag);
    } while(aut_right/aut_lag0 >= min_aut_ratio && aut_right<aut && n_lag<=n_max_distance);
    if(n_lag>n_max_distance) {
      // This should never happen, but if does return failure
      *p_last_periodicity=0;
      return;
    }
    aut=aut_right;
  }
  // Walk to the right.
  do {
    aut=aut_right;
    n_lag+=2;
    aut_right=rf_autocorrelation(pn_x, n_size, n_lag);
  } while(aut_right/aut_lag0 < min_aut_ratio && n_lag<=n_max_distance);
  if(n_lag>n_max_distance) {
    // This should never happen, but if does return failure
    *p_last_periodicity=0;
  } else
    *p_last_periodicity=n_lag;
}

void rf_signal_periodicity(float *pn_x, int32_t n_size, int32_t *p_last_periodicity, int32_t n_min_distance, int32_t n_max_distance, float min_aut_ratio, float aut_lag0, float *ratio)
/**
* \brief        Signal periodicity
* \par          Details
*               Finds periodicity of the IR signal which can be used to calculate heart rate.
*               Makes use of the autocorrelation function. If peak autocorrelation is less
*               than min_aut_ratio fraction of the autocorrelation at lag=0, then the input 
*               signal is insufficiently periodic and probably indicates motion artifacts.
*               Robert Fraczkiewicz, 01/07/2018
* \retval       Average distance between peaks
*/
{
  int32_t n_lag;
  float aut,aut_left,aut_right,aut_save;
  bool left_limit_reached=false;
  // Start from the last periodicity computing the corresponding autocorrelation
  n_lag=*p_last_periodicity;
  aut_save=aut=rf_autocorrelation(pn_x, n_size, n_lag);
  // Is autocorrelation one lag to the left greater?
  aut_left=aut;
  do {
    aut=aut_left;
    n_lag--;
    aut_left=rf_autocorrelation(pn_x, n_size, n_lag);
  } while(aut_left>aut && n_lag>=n_min_distance);
  // Restore lag of the highest aut
  if(n_lag<n_min_distance) {
    left_limit_reached=true;
    n_lag=*p_last_periodicity;
    aut=aut_save;
  } else n_lag++;
  if(n_lag==*p_last_periodicity) {
    // Trip to the left made no progress. Walk to the right.
    aut_right=aut;
    do {
      aut=aut_right;
      n_lag++;
      aut_right=rf_autocorrelation(pn_x, n_size, n_lag);
    } while(aut_right>aut && n_lag<=n_max_distance);
    // Restore lag of the highest aut
    if(n_lag>n_max_distance) n_lag=0; // Indicates failure
    else n_lag--;
    if(n_lag==*p_last_periodicity && left_limit_reached) n_lag=0; // Indicates failure
  }
  *ratio=aut/aut_lag0;
  if(*ratio < min_aut_ratio) n_lag=0; // Indicates failure
  *p_last_periodicity=n_lag;
}

float rf_rms(float *pn_x, int32_t n_size, float *sumsq) 
/**
* \brief        Root-mean-square variation 
* \par          Details
*               Compute root-mean-square variation for a given series pn_x
*               Robert Fraczkiewicz, 12/25/2017
* \retval       RMS value and raw sum of squares
*/
{
  int16_t i;
  float r,*pn_ptr;
  (*sumsq)=0.0;
  for (i=0,pn_ptr=pn_x; i<n_size; ++i,++pn_ptr) {
    r=(*pn_ptr);
    (*sumsq) += r*r;
  }
  (*sumsq)/=n_size; // This corresponds to autocorrelation at lag=0
  return sqrt(*sumsq);
}

float rf_Pcorrelation(float *pn_x, float *pn_y, int32_t n_size)
/**
* \brief        Correlation product
* \par          Details
*               Compute scalar product between *pn_x and *pn_y vectors
*               Robert Fraczkiewicz, 12/25/2017
* \retval       Correlation product
*/
{
  int16_t i;
  float r,*x_ptr,*y_ptr;
  r=0.0;
  for (i=0,x_ptr=pn_x,y_ptr=pn_y; i<n_size; ++i,++x_ptr,++y_ptr) {
    r+=(*x_ptr)*(*y_ptr);
  }
  r/=n_size;
  return r;
}

