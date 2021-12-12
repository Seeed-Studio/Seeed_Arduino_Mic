#ifndef NRF52840_ADC_H
#define NRF52840_ADC_H

#include "base_mic.h"

class NRF52840_ADC_Class : public MicClass
{

public:
    using MicClass::MicClass;
    virtual ~NRF52840_ADC_Class();

    uint8_t begin();
    void end();
    void pause();
    void resume();

    //PORTENTA_H7 min -12 max 51
    //NANO 33 BLE SENSe min 0 max 80
    void setGain(int gain);

private:
    int _dinPin;
    int _clkPin;
    int _pwrPin;

    int8_t _gain;
};

#endif