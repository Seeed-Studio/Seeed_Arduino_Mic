#ifndef MG24_ADC_H
#define MG24_ADC_H

#if defined(ARDUINO_SILABS)

#include "base_mic.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_core.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_iadc.h"
#include "em_ldma.h"
#include "em_letimer.h"
#include "em_prs.h"
#include <cstdint>
#include <cstring>


#define CLK_SRC_ADC_FREQ        20000000
#define CLK_ADC_FREQ            10000000
#define IADC_INPUT_0_PORT_PIN   iadcPosInputPortCPin9
#define IADC_INPUT_0_BUS        CDBUSALLOC
#define IADC_INPUT_0_BUSALLOC   GPIO_CDBUSALLOC_CDODD1_ADC0
#define IADC_LDMA_CH            1
#define PRS_CHANNEL             1
#define ADCFREQ                 16000       

#ifdef __cplusplus
extern "C" {
#endif

extern void LDMA_IRQHandler_use();

#ifdef __cplusplus
}
#endif

class MG24_ADC_Class : public MicClass 
{
public:
    using MicClass::MicClass;
    virtual ~MG24_ADC_Class();
    uint32_t getBufferSize() const {
        return _buf_size;
    }
    void initPRS();
    void initADC();    
    void initLETIMER();
    void initLDMA(uint16_t *buffer, uint32_t size);

    uint8_t begin();
    void end();
    void pause();
    void resume();

    uint32_t *buffer;
    uint32_t *buffer1;
    int index;
    volatile int dataCount;
    int dataconst;
    uint32_t adc_freq;
    uint32_t num_samples1;
    uint32_t NUM_SAMPLES;
    uint32_t letimer_freq;

    void set_callback(void(*function)(uint16_t *buf, uint32_t buf_len));

    void (*_onReceive)(uint16_t *buf, uint32_t buf_len);


private:
int dataConst;
};

#endif
#endif