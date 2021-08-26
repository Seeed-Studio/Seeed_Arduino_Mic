#ifndef BASE_MIC_H_INCLUDED
#define BASE_MIC_H_INCLUDED

#include <Arduino.h>

/**
 * @brief Data structure of mic config
 */

typedef struct {
    uint8_t channel_cnt;
    uint32_t sampling_rate;
    uint32_t buf_size;
    uint8_t debug_pin;

} mic_config_t;


/**
 * @brief Initialize the mic config and recording buffers
 *
 * Class constructor takes mic_config_t as parameter 
 * and creates object of the Mic class. The class methods are to be
 * extended if users would like to add their own microphone hardware.
 * See dma_adc.h and dma_adc.cpp for example.
 *
 * @param &mic_config_t  Mic configuration parameters
 *
 */
class MicClass
{
public:
  MicClass(mic_config_t *mic_config);
  virtual ~MicClass();


/**
 * Initialize the microphone and start sound recording 
 *
 * @note call setCallback() before calling this function
 * 
 * @return 0 if successfull, ERROR CODE if failed. Generic ERROR is 0.
 *
 */
  uint8_t begin();

/**
 * Deinitialize the microphone and permanently stop sound recording 
 * The recording is not meant to continue after calling end() method.
 * Rather the object is prepared for destruction.
 * 
 * @return 0 if successfull, ERROR CODE if failed. Generic ERROR is 0.
 *
 */
  void end();

/**
 * Pause sound transfer from ADC buffer to user defined callback. 
 * Designed for temporary stopping sound collection.
 */  
  void pause();

 /**
 * Resume sound transfer from ADC buffer to user defined callback. 
 */ 
  void resume();

 /**
 * Must be called before begin() method. Sets user-defined function to
 * to execute every time one of ADC buffers is filled and need to be
 * copied.
 */ 
  void set_callback(void(*function)(uint16_t *buf, uint32_t buf_len));

/**
 * NOT IMPLEMENTED. Blocking method for getting number of bytes in
 * ADC buffer.
 * 
 * @return num of bytes in ADC buffer
 *
 */
  virtual int available();

 /**
 * NOT IMPLEMENTED. Blocking method for reading number of bytes from
 * ADC buffer.
 * 
 * @return data from ADC buffer
 *
 */ 
  virtual int read(void* buffer, uint8_t buf_count, size_t size);

  uint16_t *buf_0;    // ADC results array 0
  uint16_t *buf_1;    // ADC results array 1

  inline static uint8_t *_buf_count_ptr = NULL;
  inline static uint32_t *_buf_size_ptr = NULL;
  inline static uint16_t *buf_0_ptr = NULL;
  inline static uint16_t *buf_1_ptr = NULL;
  inline static uint8_t *_debug_pin_ptr = NULL;

  inline static void (*_onReceive)(uint16_t *buf, uint32_t buf_len) = NULL;

protected:
  uint8_t _channel_cnt = 1;
  uint8_t _buf_count = 0;
  uint8_t _debug_pin = 0;

  uint32_t _sampling_rate;
  uint32_t _buf_size;  
private:

};

#endif