// Copyright 2008-2021 Seeed Studio(STU)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Created by Dmitry Maslov 8/23/2021 
// Adapted from Arduino PDM library

#ifndef MIC_H_INCLUDED
#define MIC_H_INCLUDED

#include <Arduino.h>
#include "hardware/dma_adc.h"

#define ADC_BUF_DEFAULT 1600

#ifndef ADC_BUF_SIZE
#define ADC_BUF_SIZE ADC_BUF_DEFAULT
#endif

#if !defined(WIO_TERMINAL)
#error "This library targets only Wio Terminal boards at the moment"
#endif

typedef struct {
    uint8_t mic_type;
    uint8_t channel_cnt;
    uint32_t sampling_rate;
    uint32_t buf_size;

    Adc* adc_pin;
    uint8_t debug_pin;

} mic_config_t;


class MicClass
{
public:
  MicClass();
  virtual ~MicClass();

  int begin(mic_config_t *mic_config);
  void end();

  virtual int available();
  virtual int read(void* buffer, uint8_t buf_count, size_t size);

  void onReceive(void(*)(uint16_t *buf, uint32_t buf_len));

  void setGain(uint8_t gain);

// private:
  void IrqHandler();

private:

  uint8_t _channel_cnt;
  uint32_t _sampling_rate;

  uint8_t _buf_count = 0;
  uint8_t _debug_pin;
  uint8_t _mic_type;

  uint16_t buf_0[ADC_BUF_SIZE];    // ADC results array 0
  uint16_t buf_1[ADC_BUF_SIZE];    // ADC results array 1

  void (*_onReceive)(uint16_t *buf, uint32_t buf_len);
};

extern MicClass Mic;

#endif