#include "base_mic.h"

MicClass::MicClass(mic_config_t *mic_config)
 // _onReceive(NULL)
{
  _buf_count = 0;
  _channel_cnt = mic_config->channel_cnt;
  _debug_pin = mic_config->debug_pin;
  _buf_size = mic_config->buf_size;

  // configure the sample rate and channels
  switch (mic_config->sampling_rate) {
    case 16000:
      _sampling_rate = 16;
      break;
    case 8000:
       _sampling_rate = 8;
      break;
    default:
      _sampling_rate = 8;    
  }

  switch (mic_config->channel_cnt) {
    case 1:
      break;
    default:
      _sampling_rate = 8;    
  }

  pinMode(_debug_pin, OUTPUT);

  buf_0 = new uint16_t[_buf_size];
  buf_1 = new uint16_t[_buf_size];

  _buf_count_ptr = &_buf_count;
  _buf_size_ptr = &_buf_size; 
  buf_0_ptr = buf_0;
  buf_1_ptr = buf_1;

}

MicClass::~MicClass()
{

}

uint8_t MicClass::begin()
{

}

void MicClass::end()
{

}

int MicClass::available()
{
  //mic_pause[_mic_type]();

  uint8_t buf_count = _buf_count;

  //mic_resume[_mic_type]();

  return buf_count;
}

int MicClass::read(void* buffer, uint8_t buf_count, size_t size)
{
  //mic_pause[_mic_type]();

  if (buf_count) {
    memcpy(buffer, buf_1, size);
  } else {
    memcpy(buffer, buf_0, size);
  }

  //mic_resume[_mic_type]();

  return -1;
}

void MicClass::setCallback(void(*function)(uint16_t *buf, uint32_t buf_len))
{

  _onReceive = function;

}

//MicClass Mic;
