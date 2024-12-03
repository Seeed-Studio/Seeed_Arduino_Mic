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

#define NUM_SAMPLES 250

#define ADC_FREQ                16000//The sampling frequency can be set to either 8000 or 16000.

#define CLK_SRC_ADC_FREQ        20000000
#define CLK_ADC_FREQ            10000000
#define IADC_INPUT_0_PORT_PIN   iadcPosInputPortCPin9
#define IADC_INPUT_0_BUS        CDBUSALLOC
#define IADC_INPUT_0_BUSALLOC   GPIO_CDBUSALLOC_CDODD1_ADC0
#define IADC_LDMA_CH            1
#define PRS_CHANNEL             1

#if ADC_FREQ == 16000
#define LETIMER_FREQ            20000
#define DATACONST               66
#define NUM_SAMPLES1 16000 * 3
#elif ADC_FREQ == 8000
#define LETIMER_FREQ            9000
#define DATACONST               33
#define NUM_SAMPLES1 8000 * 3
#else
#error "Unsupported LETIMER_FREQ value!"
#endif


class MG24_ADC 
{
public:
    uint32_t *buffer;
    uint32_t *buffer1;
    int index;
    volatile int dataCount;
    int dataconst;

    MG24_ADC(uint32_t *buffer, uint32_t *buffer1);
    void initADC();
    void initPRS();
    void initLETIMER();
    void initLDMA(uint32_t size);
    uint8_t begin();
    void end();
    void pause();
    void resume();
    void resetData();
    bool dataReady();
    void (*getOnReceive())(uint16_t *buf, uint32_t buf_len) {
        return _onReceive;
    }
    void set_callback(void(*function)(uint16_t *buf, uint32_t buf_len));
    void (*_onReceive)(uint16_t *buf, uint32_t buf_len);  


private:


};

#endif
#endif
