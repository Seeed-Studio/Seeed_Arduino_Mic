#include "mic.h"

#define DEFAULT_PDM_GAIN 20
#define DEBUG 1

void (*mic_init[])(uint16_t *buf_0, uint16_t *buf_1) = {dma_adc_init};
void (*mic_deinit[])(void) = {dma_adc_deinit};
void (*mic_pause[])(void) = {dma_adc_pause};
void (*mic_resume[])(void) = {dma_adc_resume};

MicClass::MicClass():
  _onReceive(NULL)
{
}

MicClass::~MicClass()
{

}

int MicClass::begin(mic_config_t *mic_config)
{
  
  _buf_count = 0;
  _channel_cnt = mic_config->channel_cnt;
  _debug_pin = mic_config->debug_pin;
  _mic_type = mic_config->mic_type;

  // configure the sample rate and channels
  switch (mic_config->sampling_rate) {
    case 16000:
      _sampling_rate = 16;
      break;
    case 8000:
       _sampling_rate = 8;
      break;
    default:
      return 0; // unsupported
  }

  switch (mic_config->channel_cnt) {
    case 1:
      break;
    default:
      return 0; // unsupported
  }

#ifdef DEBUG
  pinMode(_debug_pin, OUTPUT);
#endif

  setGain(DEFAULT_PDM_GAIN);  
  mic_init[_mic_type](buf_0, buf_1);
  
  return 1;
}

void MicClass::end()
{
  mic_deinit[_mic_type]();
}

int MicClass::available()
{
  mic_pause[_mic_type]();

  uint8_t buf_count = _buf_count;

  mic_resume[_mic_type]();

  return buf_count;
}

int MicClass::read(void* buffer, uint8_t buf_count, size_t size)
{
  mic_pause[_mic_type]();

  if (buf_count) {
    memcpy(buffer, buf_1, size);
  } else {
    memcpy(buffer, buf_0, size);
  }

  mic_resume[_mic_type]();

  return -1;
}

void MicClass::onReceive(void(*function)(uint16_t *buf, uint32_t buf_len))
{
  _onReceive = function;
}

void MicClass::setGain(uint8_t gain)
{
}

void MicClass::IrqHandler()
{

  // Check if DMAC channel 1 has been suspended (SUSP)
  if (DMAC->Channel[1].CHINTFLAG.bit.SUSP) {

     // Debug: make pin high before copying buffer
#if DEBUG
    digitalWrite(_debug_pin, HIGH);
#endif

    // Restart DMAC on channel 1 and clear SUSP interrupt flag
    DMAC->Channel[1].CHCTRLB.reg = DMAC_CHCTRLB_CMD_RESUME;
    DMAC->Channel[1].CHINTFLAG.bit.SUSP = 1;

    // See which buffer has filled up, and dump results into large buffer
    if (_buf_count) {
      _onReceive(buf_0, ADC_BUF_SIZE);
    } else {
      _onReceive(buf_1, ADC_BUF_SIZE);
    }

    // Flip to next buffer
    _buf_count = (_buf_count + 1) % 2;

    // Debug: make pin low after copying buffer
#if DEBUG
    digitalWrite(_debug_pin, LOW);
#endif
  }
}

extern "C" {
  __attribute__((__used__)) void DMAC_1_Handler(void)
  {
    Mic.IrqHandler();
  }
}

MicClass Mic;