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
#include "hardware/nrf52840_adc.h"
#include "hardware/mg24_adc.h"

#if !defined(WIO_TERMINAL) && !defined(ARDUINO_ARCH_NRF52840) && !defined(ARDUINO_SILABS)
#error "This library targets Wio Terminal boards, XIAO BLE and XIAO MG24 at the moment"
#endif

#endif