#ifndef MG24_ADC_H
#define MG24_ADC_H

#if defined(ARDUINO_XIAO_MG24)

#include "base_mic.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_core.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_iadc.h"
#include "em_ldma.h"
#include <cstdint>
#include <cstring>

#define MIC_ENPIN               PC8
#define MIC_INPUTPIN            PC9
#define CLK_SRC_ADC_FREQ        20000000
#define CLK_ADC_FREQ            10000000
#define IADC_INPUT_0_PORT_PIN   iadcPosInputPortCPin9

class MG24_ADC_Class : public MicClass 
{
public:
    using MicClass::MicClass;
    void initADC();    
    uint8_t initLDMA();

    uint8_t begin();
    void end();
    void pause();
    void resume();

    unsigned int dma_channel;
};

#endif
#endif