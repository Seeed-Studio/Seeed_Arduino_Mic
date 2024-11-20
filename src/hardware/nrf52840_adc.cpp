#include "nrf52840_adc.h"

#if defined(ARDUINO_ARCH_NRF52840)

#include <hal/nrf_pdm.h>
#define DEFAULT_PDM_GAIN     20
#define PDM_IRQ_PRIORITY     7

#define NRF_PDM_FREQ_1280K  (nrf_pdm_freq_t)(0x0A000000UL)               ///< PDM_CLK= 1.280 MHz (32 MHz / 25) => Fs= 20000 Hz
#define NRF_PDM_FREQ_2000K  (nrf_pdm_freq_t)(0x10000000UL)               ///< PDM_CLK= 2.000 MHz (32 MHz / 16) => Fs= 31250 Hz
#define NRF_PDM_FREQ_2667K  (nrf_pdm_freq_t)(0x15000000UL)               ///< PDM_CLK= 2.667 MHz (32 MHz / 12) => Fs= 41667 Hz
#define NRF_PDM_FREQ_3200K  (nrf_pdm_freq_t)(0x19000000UL)               ///< PDM_CLK= 3.200 MHz (32 MHz / 10) => Fs= 50000 Hz
#define NRF_PDM_FREQ_4000K  (nrf_pdm_freq_t)(0x20000000UL)               ///< PDM_CLK= 4.000 MHz (32 MHz /  8) => Fs= 62500 Hz

NRF52840_ADC_Class::~NRF52840_ADC_Class()
{
    end();
}

uint8_t NRF52840_ADC_Class::begin()
{
  _dinPin = PIN_PDM_DIN;
  _clkPin = PIN_PDM_CLK;
  _pwrPin = PIN_PDM_PWR;
  _gain = -1;

  // Enable high frequency oscillator if not already enabled
  if (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
    NRF_CLOCK->TASKS_HFCLKSTART    = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) { }
  }

  // configure the sample rate and channels
  switch (_sampling_rate) {
    case 16000:
      NRF_PDM->RATIO = ((PDM_RATIO_RATIO_Ratio80 << PDM_RATIO_RATIO_Pos) & PDM_RATIO_RATIO_Msk);
      nrf_pdm_clock_set(NRF_PDM_FREQ_1280K);
      break;
    case 41667:
      nrf_pdm_clock_set(NRF_PDM_FREQ_2667K);
      break;
    default:
      return 0; // unsupported
  }

  switch (_channel_cnt) {
    case 2:
      nrf_pdm_mode_set(NRF_PDM_MODE_STEREO, NRF_PDM_EDGE_LEFTFALLING);
      break;

    case 1:
      nrf_pdm_mode_set(NRF_PDM_MODE_MONO, NRF_PDM_EDGE_LEFTFALLING);
      break;

    default:
      return 0; // unsupported
  }

  if(_gain == -1) {
    _gain = DEFAULT_PDM_GAIN;
  }
  nrf_pdm_gain_set(_gain, _gain);

  // configure the I/O and mux
  pinMode(_clkPin, OUTPUT);
  digitalWrite(_clkPin, LOW);

  pinMode(_dinPin, INPUT);

  nrf_pdm_psel_connect(digitalPinToPinName(_clkPin), digitalPinToPinName(_dinPin));

  // clear events and enable PDM interrupts
  nrf_pdm_event_clear(NRF_PDM_EVENT_STARTED);
  nrf_pdm_event_clear(NRF_PDM_EVENT_END);
  nrf_pdm_event_clear(NRF_PDM_EVENT_STOPPED);
  nrf_pdm_int_enable(NRF_PDM_INT_STARTED | NRF_PDM_INT_STOPPED);

  if (_pwrPin > -1) {
    // power the mic on
    pinMode(_pwrPin, OUTPUT);
    digitalWrite(_pwrPin, HIGH);
  }

  // set the PDM IRQ priority and enable
  NVIC_SetPriority(PDM_IRQn, PDM_IRQ_PRIORITY);
  NVIC_ClearPendingIRQ(PDM_IRQn);
  NVIC_EnableIRQ(PDM_IRQn);
  
  // enable and trigger start task
  nrf_pdm_enable();
  nrf_pdm_event_clear(NRF_PDM_EVENT_STARTED);
  nrf_pdm_task_trigger(NRF_PDM_TASK_START);

  return 1;  
}

void NRF52840_ADC_Class::end()
{
  // disable PDM and IRQ
  nrf_pdm_disable();

  NVIC_DisableIRQ(PDM_IRQn);

  if (_pwrPin > -1) {
    // power the mic off
    digitalWrite(_pwrPin, LOW);
    pinMode(_pwrPin, INPUT);
  }

  // Don't disable high frequency oscillator since it could be in use by RADIO

  // unconfigure the I/O and un-mux
  nrf_pdm_psel_disconnect();

  pinMode(_clkPin, INPUT);
}

void NRF52840_ADC_Class::pause()
{
    NVIC_DisableIRQ(PDM_IRQn);
}

void NRF52840_ADC_Class::resume()
{
    NVIC_EnableIRQ(PDM_IRQn);
}

void NRF52840_ADC_Class::setGain(int gain)
{
  _gain = gain;
  nrf_pdm_gain_set(_gain, _gain);    
}

extern "C" {
  __attribute__((__used__)) void PDM_IRQHandler_v(void)
  {
if (nrf_pdm_event_check(NRF_PDM_EVENT_STARTED)) {
    nrf_pdm_event_clear(NRF_PDM_EVENT_STARTED);

    // Debug: make pin high before copying buffer
    if (*NRF52840_ADC_Class::_debug_pin_ptr) 
        digitalWrite(*NRF52840_ADC_Class::_debug_pin_ptr, HIGH);

    // switch to fill
	/*
		Why use the _buf_size_ptr / 2 not only _buf_size_ptr ?
		Because, we alloc the data buf use the uint16_t.But everytime the PDM
		take sample,it will use 32 bits space in the data buf.So we need the /2.
		Actually sizeof(uint32_t) / sizeof(uint16_t) = 2.
	*/
    if (*NRF52840_ADC_Class::_buf_count_ptr) {
        nrf_pdm_buffer_set((uint32_t*)(NRF52840_ADC_Class::buf_0_ptr), *NRF52840_ADC_Class::_buf_size_ptr / 2);
        if(NRF52840_ADC_Class::_onReceive){
            NVIC_DisableIRQ(PDM_IRQn);
            NRF52840_ADC_Class::_onReceive(NRF52840_ADC_Class::buf_1_ptr, *NRF52840_ADC_Class::_buf_size_ptr / 2);
            NVIC_EnableIRQ(PDM_IRQn);
        }
    } else {
        nrf_pdm_buffer_set((uint32_t*)(NRF52840_ADC_Class::buf_1_ptr), *NRF52840_ADC_Class::_buf_size_ptr / 2);
        if(NRF52840_ADC_Class::_onReceive){
            NVIC_DisableIRQ(PDM_IRQn);
            NRF52840_ADC_Class::_onReceive(NRF52840_ADC_Class::buf_0_ptr, *NRF52840_ADC_Class::_buf_size_ptr / 2);
            NVIC_EnableIRQ(PDM_IRQn);
        }
    }

    // Flip to next buffer
    *NRF52840_ADC_Class::_buf_count_ptr = (*NRF52840_ADC_Class::_buf_count_ptr + 1) % 2;

    // Debug: make pin low after copying buffer
    if (*NRF52840_ADC_Class::_debug_pin_ptr) 
        digitalWrite(*NRF52840_ADC_Class::_debug_pin_ptr, LOW);

  } else if (nrf_pdm_event_check(NRF_PDM_EVENT_STOPPED)) {
    nrf_pdm_event_clear(NRF_PDM_EVENT_STOPPED);
  } else if (nrf_pdm_event_check(NRF_PDM_EVENT_END)) {
    nrf_pdm_event_clear(NRF_PDM_EVENT_END);
  }
  }
}

#endif