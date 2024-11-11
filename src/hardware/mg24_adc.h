#ifndef MG24_ADC_H
#define MG24_ADC_H
#if defined(ARDUINO_SILABS)

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
#define NUM_SAMPLES1 50000

class MG24_ADC {
public:
    uint32_t *buffer;
    uint32_t *buffer1;
    int index;
    volatile int dataCount;

    MG24_ADC(uint32_t *buffer, uint32_t *buffer1);
    void initADC();
    void initPRS();
    void initLETIMER();
    void initLDMA(uint32_t size);
    void start();
    void resetData();
    bool dataReady();

private:
    // uint32_t *buffer;
    // uint32_t *buffer1;
    // int index;
    // volatile int dataCount;
};

#endif
#endif
