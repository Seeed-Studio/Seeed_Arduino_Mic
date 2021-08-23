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

#if !defined(WIO_TERMINAL)
#error "This library targets only Wio Terminal boards at the moment"
#endif

/**
 * @brief DMAC descriptor structure
 */

typedef struct {
    uint16_t btctrl;
    uint16_t btcnt;
    uint32_t srcaddr;
    uint32_t dstaddr;
    uint32_t descaddr;
} dmacdescriptor;


class MicClass
{
public:
  MicClass();
  virtual ~MicClass();

  int begin(int channels, long sampleRate);
  void end();

  virtual int available();
  virtual int read(void* buffer, size_t size);

  void onReceive(void(*)(uint16_t *buf, uint32_t buf_len));

  void setGain(int gain);
  void setBufferSize(int bufferSize);

// private:
  void IrqHandler();

private:

  int _channels;
  
  enum {ADC_BUF_LEN = 1600};    // Size of one of the DMA double buffers

  uint16_t adc_buf_0[ADC_BUF_LEN];    // ADC results array 0
  uint16_t adc_buf_1[ADC_BUF_LEN];    // ADC results array 1

  void (*_onReceive)(uint16_t *buf, uint32_t buf_len);
};

extern MicClass Mic;

#endif