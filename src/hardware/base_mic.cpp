#include "base_mic.h"

MicClass::MicClass(mic_config_t *mic_config)
{

  _channel_cnt = mic_config->channel_cnt;
  _debug_pin = mic_config->debug_pin;
  _buf_size = mic_config->buf_size;
  _sampling_rate = mic_config->sampling_rate;

  switch (mic_config->channel_cnt) {
    case 1:
      break;
    default:
      break;   
  }

  buf_0 = new uint16_t[_buf_size];
  buf_1 = new uint16_t[_buf_size];

  _buf_count_ptr = &_buf_count;
  _buf_size_ptr = &_buf_size; 
  buf_0_ptr = buf_0;
  buf_1_ptr = buf_1;

  if (_debug_pin) {
  pinMode(_debug_pin, OUTPUT);
  _debug_pin_ptr = &_debug_pin;
  }

}

MicClass::~MicClass()
{
  delete buf_0;
  delete buf_1;
}

uint8_t MicClass::begin()
{
  return 1;
//TO BE DEFINED IN HARDWARE CHILD CLASS
}

void MicClass::end()
{
//TO BE DEFINED IN HARDWARE CHILD CLASS
}

void MicClass::resume()
{
//TO BE DEFINED IN HARDWARE CHILD CLASS
}

void MicClass::pause()
{
//TO BE DEFINED IN HARDWARE CHILD CLASS
}

int MicClass::available()
{
  pause();

  uint8_t buf_count = _buf_count;

  resume();

  return buf_count;
}

int MicClass::read(void* buffer, uint8_t buf_count, size_t size)
{
  pause();

  if (buf_count) {
    memcpy(buffer, buf_1, size);
  } else {
    memcpy(buffer, buf_0, size);
  }

  resume();

  return -1;
}

void MicClass::set_callback(void(*function)(uint16_t *buf, uint32_t buf_len))
{

  _onReceive = function;

}

