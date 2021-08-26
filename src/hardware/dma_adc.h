#ifndef DMA_ADC_H_INCLUDED
#define DMA_ADC_H_INCLUDED

#include "base_mic.h"

#define _16KHZ (48000000 / 16000) - 1
#define _8KHZ (48000000 / 8000) - 1

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


class DMA_ADC_Class : public MicClass
{

public:
    using MicClass::MicClass;
    virtual ~DMA_ADC_Class();

    uint8_t begin();
    void end();
    void pause();
    void resume();
    void setCallback(void(*function)(uint16_t *buf, uint32_t buf_len));

    static void IrqHandler();

private:

  uint8_t _channel_cnt;

};

#endif