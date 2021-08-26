#ifndef BASE_MIC_H_INCLUDED
#define BASE_MIC_H_INCLUDED

#include <Arduino.h>

typedef struct {
    uint8_t channel_cnt;
    uint32_t sampling_rate;
    uint32_t buf_size;

    Adc* adc_pin;
    uint8_t debug_pin;

} mic_config_t;

class MicClass
{
public:
  MicClass(mic_config_t *mic_config);
  virtual ~MicClass();

  uint8_t begin();
  void end();

  virtual int available();
  virtual int read(void* buffer, uint8_t buf_count, size_t size);

  void setCallback(void(*)(uint16_t *buf, uint32_t buf_len));

  uint16_t *buf_0;    // ADC results array 0
  uint16_t *buf_1;    // ADC results array 1

  inline static uint8_t *_buf_count_ptr = NULL;
  inline static uint32_t *_buf_size_ptr = NULL;
  inline static uint16_t *buf_0_ptr = NULL;
  inline static uint16_t *buf_1_ptr = NULL;

  inline static void (*_onReceive)(uint16_t *buf, uint32_t buf_len) = NULL;

protected:
  uint8_t _channel_cnt;
  uint32_t _sampling_rate;

  uint8_t _buf_count = 0;
  uint32_t _buf_size;  
  uint8_t _debug_pin;

private:

};

#endif